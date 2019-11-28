#ifndef MERRIE_GAMESERVER_HEADERS_GAMESERVER_NETWORK_GAMEHTTPSERVER_HPP
#define MERRIE_GAMESERVER_HEADERS_GAMESERVER_NETWORK_GAMEHTTPSERVER_HPP

#include "../GameServer.hpp"

#include <Commons/Network/Http.hpp>

namespace Merrie {

    /*
     * TODO Docs
     */

    class GameHttpServer : public HttpServer {
        public:
            NON_COPYABLE(GameHttpServer);
            NON_MOVEABLE(GameHttpServer);

            GameHttpServer(GameServer* gameServer, HttpServerSettings settings);

        protected:
            void HandleRequest(std::shared_ptr<HttpConnection> connection) override;

        private: // Private methods
            void HandleEnginePacket(std::shared_ptr<HttpConnection> connection, const DecodedUrl& url);

        private: // Private fields
            GameServer* m_gameServer;
    };


}

#endif //MERRIE_GAMESERVER_HEADERS_GAMESERVER_NETWORK_PACKET_HANDLERS_HPP
