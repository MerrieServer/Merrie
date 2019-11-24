#include <GameServer/Network/GameHttpServer.hpp>

#include <boost/algorithm/string/predicate.hpp>

namespace Merrie {
    const std::string HttpUrlDecodeError = "<h1>Failed to decode URL</h1>";
    const std::string Http404Error = "<h1>Failed to find the requested resource</h1>";

    GameHttpServer::GameHttpServer(GameServer* gameServer, HttpServerSettings settings) : HttpServer(std::move(settings)), m_gameServer(gameServer) {

    }

    void GameHttpServer::HandleRequest(std::shared_ptr<HttpConnection> connection, http::request<http::string_body>& request, http::response<http::string_body>& response) {
        response.set(http::field::content_type, "text/html");
        response.result(http::status::ok);

        DecodedUrl url;

        try {
            url = DecodeUrlQueryString(std::string_view(request.target().data(), request.target().size()));
        }
        catch (const UrlDecodeException& e) {
            response.body() = HttpUrlDecodeError + "<p>" + e.what() + "</p>";
            response.result(http::status::bad_request);
            return;
        }

        #ifdef M_ENABLE_DEBUG
        if (boost::starts_with(url.Path, "/__DEBUGREQUEST")) {
            std::string r;
            r += "<h4>Path</h4>";
            r += "<p>" + url.Path + "</p>";

            r += "<br><br><h4>Query parameters: </h4>";

            for (const auto&[key, value] : url.Parameters) {
                r += "<p>" + key + "<b> = </b>" + value + "</p>";
            }

            response.body() = r;
            return;
        }
        #endif

        if (url.Path == "/engine") {
            response.body() = "{}";
            response.set(http::field::content_type, "application/json");
        } else {
            response.body() = Http404Error;
            response.result(http::status::not_found);
        }
    }
}
