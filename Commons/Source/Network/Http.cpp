#include <Commons/Network/Http.hpp>

namespace Merrie {

    namespace {
        constexpr const char InvalidCharacter = std::numeric_limits<char>::max();

        char _DecodeHexNibble(char c) {
            if (c >= '0' && c <= '9')
                return static_cast<char>(c - '0');
            else if (c >= 'a' && c <= 'f')
                return static_cast<char>(c - 'a' + 10);
            else if (c >= 'A' && c <= 'F')
                return static_cast<char>(c - 'A' + 10);
            else
                return InvalidCharacter;
        }

        void _DecodeComponent(std::string_view component, std::string& output) {
            output = "";
            if (component.empty()) {
                return;
            }

            if (component.find('%') == std::string::npos && component.find('+') == std::string::npos) {
                output = component;
                return;
            }

            output.reserve(component.length());
            size_t i = 0;

            while (i < component.size()) {
                switch (component[i]) {
                    case '+':
                        output += ' ';
                        break;
                    case '%': {
                        if (i == component.size() - 1)
                            throw UrlDecodeException("unterminated escape sequence at the end of string");

                        const char character = component[++i];
                        if (character == '%') {
                            output += '%';
                            ++i;
                            continue;
                        }

                        if (i == component.size() - 1)
                            throw UrlDecodeException("partial escape sequence and the end of string");

                        const char c1 = _DecodeHexNibble(character);
                        const char c2 = _DecodeHexNibble(component[++i]);

                        if (c1 == InvalidCharacter || c2 == InvalidCharacter)
                            throw UrlDecodeException("invalid escape sequence");

                        output += static_cast<char>(c1 * 16 + c2);
                        break;
                    }
                    default:
                        output += component[i];
                        break;
                }
                i++;
            }
        }
    }


    DecodedUrl DecodeUrlQueryString(std::string_view queryString) {
        DecodedUrl url;

        auto pathEnd = queryString.find('?');

        if (pathEnd == std::string::npos) {
            url.Path = queryString;
            return url;
        }

        url.Path = queryString.substr(0, pathEnd);

        std::string name;
        size_t i = pathEnd + 1;
        size_t pos = i;

        char character;

        while (i < queryString.length()) {
            character = queryString[i];

            if (character == '=' && name.empty()) {
                if (pos != i)
                    _DecodeComponent(queryString.substr(pos, i - pos), name);

                pos = i + 1;
            } else if (character == '&') {
                if (name.empty() && pos != i)
                {
                    std::string key;
                    _DecodeComponent(queryString.substr(pos, i - pos), key);
                    url.Parameters.try_emplace(std::move(key), "");
                }
                else if (!name.empty()) {
                    auto[iterator, success] = url.Parameters.try_emplace(std::move(name), "");

                    if (!success)
                        iterator->second.clear();

                    _DecodeComponent(queryString.substr(pos, i - pos), iterator->second);
                    name.clear();
                }

                pos = i + 1;
            }

            i++;
        }

        if (pos != i) {
            if (name.empty()) {
                std::string key;
                _DecodeComponent(queryString.substr(pos, i - pos), key);
                url.Parameters.try_emplace(std::move(key), "");
            } else {
                auto[iterator, success] = url.Parameters.try_emplace(std::move(name), "");
                if (!success)
                    iterator->second.clear();

                _DecodeComponent(queryString.substr(pos, i - pos), iterator->second);
            }
        } else if (!name.empty()) {
            url.Parameters.try_emplace(std::move(name), "");
        }

        return url;
    }

    HttpServer::HttpServer(HttpServerSettings settings)
            : NetworkServer(settings.NetworkServerSettings),
              m_settings(std::move(settings)),
              m_keepAliveHeader("timeout=" + std::to_string(settings.KeepAliveTimeout) + ", max=" + std::to_string(settings.KeepAliveMax)) {
    }

    std::shared_ptr<NetworkConnection> HttpServer::CreateNetworkConnection(boost::asio::io_context& context) {
        return std::make_shared<HttpConnection>(context, this);
    }

    void HttpServer::ReadData(std::shared_ptr<NetworkConnection> connection) {
        auto* httpConnection = static_cast<HttpConnection*>(connection.get()); // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
        httpConnection->ReadData(std::move(connection));
    }

    const HttpServerSettings& HttpServer::GetHttpSettings() const {
        return m_settings;
    }

    void HttpServer::HandleRequestInternal(std::shared_ptr<HttpConnection> connection, http::request<http::string_body>& request, http::response<http::string_body>& response) {
        HandleRequest(std::move(connection), request, response);

        // basic headers
        response.version(request.version());
        response.set(http::field::content_length, response.body().size());

        // keep alive
        response.keep_alive(m_settings.AllowKeepAlive && request.keep_alive());
        if (response.keep_alive()) {
            response.set(http::field::connection, "keep-alive");
            response.set(http::field::keep_alive, m_keepAliveHeader);
        }
    }

    HttpConnection::HttpConnection(boost::asio::io_context& ioContext, HttpServer* server) : NetworkConnection(ioContext), m_server(server) {
    }

    void HttpConnection::ReadData(std::shared_ptr<NetworkConnection> connectionOwnership) {
        http::async_read(GetSocket(), m_buffer, m_request, [this, connectionOwnership = std::move(connectionOwnership)](boost::beast::error_code ec, std::size_t) mutable {
            if (ec) {
                // todo: handle error
                return;
            }

            m_server->HandleRequestInternal(shared_from_this(), m_request, m_response);

            // todo: deadline?
            http::async_write(GetSocket(), m_response, [this, connectionOwnership = std::move(connectionOwnership)](boost::beast::error_code error, std::size_t) mutable {
                if (!m_response.keep_alive() && GetSocket().is_open()) {
                    GetSocket().shutdown(tcp::socket::shutdown_send, error);
                }

                ReadData(std::move(connectionOwnership));
            });
        });
    }
}