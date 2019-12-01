#include <GameServer/Network/GameHttpServer.hpp>

#include <Commons/Containers.hpp>
#include <Commons/Ticker.hpp>
#include <GameServer/Player.hpp>
#include <GameServer/Network/Packets.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/lexical_cast.hpp>
#include <nlohmann/json.hpp>

namespace Merrie {
    const std::string HttpUrlDecodeError = "<h1>Failed to decode URL</h1>";
    const std::string Http404Error = "<h1>Failed to find the requested resource</h1>";

    GameHttpServer::GameHttpServer(GameServer* gameServer, HttpServerSettings settings) : HttpServer(std::move(settings)), m_gameServer(gameServer) {

    }

    void GameHttpServer::HandleRequest(std::shared_ptr<HttpConnection> connection) {
        // CORS
        connection->GetResponse().set(http::field::access_control_allow_credentials, "true");
        connection->GetResponse().set(http::field::access_control_allow_methods, "POST, GET");
        connection->GetResponse().set(http::field::access_control_allow_origin, "http://classic.margonem.pl");
        connection->GetResponse().set(http::field::access_control_allow_headers, "*");
        connection->GetResponse().set(http::field::access_control_max_age, "86400");


        // OPTIONS requests
        if (connection->GetRequest().method() == http::verb::options) {
            connection->GetResponse().set(http::field::allow, "POST, GET, OPTIONS");
            connection->GetResponse().result(http::status::ok);
            connection->SendResponse();
            return;
        }

        connection->GetResponse().set(http::field::content_type, "text/html");
        connection->GetResponse().result(http::status::ok);

        DecodedUrl url;

        try {
            url = DecodeUrlQueryString(std::string_view(connection->GetRequest().target().data(), connection->GetRequest().target().size()));
        }
        catch (const UrlDecodeException& e) {
            connection->GetResponse().body() = HttpUrlDecodeError + "<p>" + e.what() + "</p>";
            connection->GetResponse().result(http::status::bad_request);
            connection->SendResponse();
            return;
        }

        #ifdef M_ENABLE_DEBUG
        if (boost::starts_with(url.Path, "/__DEBUGREQUEST")) {
            std::string r;
            r += "<h4>Path</h4>";
            r += "<p>" + url.Path + "</p>";

            r += "<br><br><h4>Query parameters: </h4>";

            for (const auto&[key, value] : url.Parameters) {
                r += "<p>";
                r += key;
                r += "<b> = </b>";
                r += value;
                r += "</p>";
            }

            connection->GetResponse().body() = r;
            connection->SendResponse();
            return;
        }
        #endif

        if (url.Path == "/engine") {
            HandleEnginePacket(std::move(connection), url);
        } else {
            connection->GetResponse().body() = Http404Error;
            connection->GetResponse().result(http::status::not_found);
            connection->SendResponse();
        }
    }

    std::string _CreateSimpleStopPacket(std::string_view reason) {
        nlohmann::json json;
        json["t"] = "stop";
        json["e"] = reason;
        return json.dump();
    }

    inline HandleResult _ProcessPacketHandlerChain(const std::vector<std::shared_ptr<PacketHandlerData>>& chain, const IncomingPacket& in, OutgoingPacket& out) {
        // _ is a 'no action' action, it will never be handled by and of the handlers
        HandleResult result = in.Action == "_" ? HandleResult::ContinueHandling : HandleResult::Ignored;

        for (const std::shared_ptr<PacketHandlerData>& data : chain) {
            if (!data->ActionFilters.empty()) {
                if (in.Action == "_" || !Contains(data->ActionFilters, in.Action)) {
                    continue;
                }
            }

            switch (data->Handler(in.Player_, in, out)) {
                case HandleResult::ContinueHandling:
                    if (!data->ActionFilters.empty())
                        result = HandleResult::ContinueHandling;
                    break;
                case HandleResult::Ignored:
                    break;
                case HandleResult::StopHandling:
                    return HandleResult::StopHandling;
            }
        }

        return result;
    }

    void GameHttpServer::HandleEnginePacket(std::shared_ptr<HttpConnection> connection, const DecodedUrl& url) {
        connection->GetResponse().set(http::field::content_type, "application/json; charset=utf-8");

        const auto action = FindInMap(url.Parameters, "t"s);

        if (!action) {
            connection->GetResponse().body() = _CreateSimpleStopPacket("invalid action");
            connection->SendResponse();
            return;
        }

        // getvar_addon is a special action, it does not require aid and it does not return JSON like all the other actions
        if (action == "getvar_addon") {
            const auto callback = FindInMap(url.Parameters, "callback"s);

            connection->GetResponse().set(http::field::content_type, "application/javascript; charset=utf-8");
            connection->GetResponse().body() = callback.value_or("invalid") + "(\"\")";
            connection->SendResponse();
            return;
        }


        const auto aid_s = FindInMap(url.Parameters, "aid"s);
        uint64_t aid;

        if (!aid_s || !boost::conversion::try_lexical_convert(aid_s.value(), aid)) {
            connection->GetResponse().body() = _CreateSimpleStopPacket("no aid");
            connection->SendResponse();
            return;
        }

        // TODO: Authentication (async!)
        // TODO: Validate via cookies

        IncomingPacket in = {
                m_gameServer->GetPlayer(aid),
                action.value(),
                url.Parameters
        };
        OutgoingPacket out;
        const HandleResult asyncResult = _ProcessPacketHandlerChain(GetRegisteredAsyncPacketHandlers(), in, out);

        {
            std::shared_lock lock(in.Player_->GetDataMutex());
            in.Player_->SetTimeout();
        }

        if (asyncResult == HandleResult::StopHandling) {
            connection->GetResponse().body() = _CreateSimpleStopPacket("StopHandling was returned");
            connection->SendResponse();
            return;
        }

        m_gameServer->GetTicker()->DoInMainThread([asyncResult, in = std::move(in), out = std::move(out), connection = std::move(connection)](const std::shared_ptr<Task>&) mutable {
            const HandleResult syncResult = _ProcessPacketHandlerChain(GetRegisteredSyncPacketHandlers(), in, out);

            if (syncResult == HandleResult::StopHandling) {
                connection->GetResponse().body() = _CreateSimpleStopPacket("StopHandling was returned");
                connection->SendResponse();
                return;
            }

            if (syncResult == HandleResult::Ignored && asyncResult == HandleResult::Ignored) {
                connection->GetResponse().body() = _CreateSimpleStopPacket("invalid action");
                connection->SendResponse();
                return;
            }

            connection->GetResponse().body() = out.GetJson().dump();
            connection->SendResponse();
        }, false);
    }
}
