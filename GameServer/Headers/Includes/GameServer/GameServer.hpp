#ifndef MERRIE_GAMESERVER_HEADERS_GAMESERVER_GAMESERVER_HPP
#define MERRIE_GAMESERVER_HEADERS_GAMESERVER_GAMESERVER_HPP

#include <Commons/Commons.hpp>
#include <Commons/Logging.hpp>

namespace Merrie {
    class GameHttpServer; // Network/GameHttpServer.hpp

    class GameServer {
        public:
            NON_COPYABLE(GameServer);
            NON_MOVEABLE(GameServer);

            GameServer();

            virtual ~GameServer();
        public:
            void Start();

        private:
            bool m_running;
            std::unique_ptr<GameHttpServer> m_gameHttpServer;

            M_DECLARE_LOGGER;
    };

}

#endif //MERRIE_GAMESERVER_HEADERS_GAMESERVER_GAMESERVER_HPP
