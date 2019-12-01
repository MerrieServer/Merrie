#ifndef MERRIE_GAMESERVER_HEADERS_GAMESERVER_GAMESERVER_HPP
#define MERRIE_GAMESERVER_HEADERS_GAMESERVER_GAMESERVER_HPP

#include <Commons/Commons.hpp>
#include <Commons/Logging.hpp>
#include <Commons/Time.hpp>
#include <Commons/Network/Http.hpp>
#include <shared_mutex>

namespace Merrie {
    class Ticker; // Commons/Ticker.hpp
    class GameHttpServer; // Network/GameHttpServer.hpp
    class Player; // Player.hpp

    struct GameServerSettings {
        HttpServerSettings HttpServerSettingsValue;
        unsigned int Tps;
        std::vector<std::string> LogFilters;
    };

    /*
     * TODO Docs
     */

    class GameServer {
        public:
            NON_COPYABLE(GameServer);
            NON_MOVEABLE(GameServer);

            GameServer(GameServerSettings settings);

            virtual ~GameServer();
        public:
            void Start();

            void Stop();

            [[nodiscard]] const GameServerSettings& GetSettings() const noexcept;

            [[nodiscard]] bool IsRunning() const noexcept;

            [[nodiscard]] const std::unique_ptr<Ticker>& GetTicker() const noexcept;

            std::shared_ptr<Player> GetPlayer(uint64_t aid);

        private:
            void Tick();

        private:
            const GameServerSettings m_settings;
            bool m_running = false;
            std::unique_ptr<GameHttpServer> m_gameHttpServer;
            std::unique_ptr<Ticker> m_ticker;
            std::shared_mutex m_playersMutex{};
            std::map<uint64_t, std::shared_ptr<Player>> m_players{};
            DefaultClock::time_point m_oneSecondTasks{};

            M_DECLARE_LOGGER;
    };

}

#endif //MERRIE_GAMESERVER_HEADERS_GAMESERVER_GAMESERVER_HPP
