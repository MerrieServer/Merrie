#ifndef MERRIE_GAMESERVER_HEADERS_GAMESERVER_NETWORK_GAMEHTTPSERVER_HPP
#define MERRIE_GAMESERVER_HEADERS_GAMESERVER_NETWORK_GAMEHTTPSERVER_HPP

#include "../GameServer.hpp"

#include <Commons/Network/Http.hpp>

namespace Merrie {

    class GameHttpServer : public HttpServer
    {
        public:
            NON_COPYABLE(GameHttpServer);
            NON_MOVEABLE(GameHttpServer);

            GameHttpServer(GameServer* gameServer, HttpServerSettings settings);

        protected:
            void HandleRequest(std::shared_ptr<HttpConnection> connection, http::request<http::string_body>& request, http::response<http::string_body>& response) override;

        private: // Private fields
            GameServer* m_gameServer;
    };

}

#endif //MERRIE_GAMESERVER_HEADERS_GAMESERVER_NETWORK_GAMEHTTPSERVER_HPP
