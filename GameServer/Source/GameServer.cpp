#include <GameServer/GameServer.hpp>

#include <Commons/Containers.hpp>
#include <Commons/Ticker.hpp>
#include <GameServer/Player.hpp>
#include <GameServer/Network/GameHttpServer.hpp>

namespace Merrie {
    GameServer::GameServer(GameServerSettings settings) : m_settings(std::move(settings)) {
        m_gameHttpServer = std::make_unique<GameHttpServer>(this, m_settings.HttpServerSettingsValue);
        m_ticker = std::make_unique<Ticker>();
    }

    GameServer::~GameServer() {
        if (IsRunning()) {
            M_LOG_WARNING_THIS << "Destroying a running server (probably a bug, please report it)";
            Stop();
        }
    }

    void GameServer::Start() {
        M_ASSERT(!m_running, "Already running");
        M_LOG_INFO_THIS << "Starting the game server";

        m_running = true;
        m_gameHttpServer->Start();
        m_ticker->ResetAll();
        m_ticker->DoInMainThread(std::bind(&GameServer::Tick, this), true);
    }

    void GameServer::Stop() {
        m_gameHttpServer->Stop();
        m_gameHttpServer->Join();
        m_running = false;
    }

    bool GameServer::IsRunning() const noexcept {
        return m_running;
    }

    const std::unique_ptr<Ticker>& GameServer::GetTicker() const noexcept {
        return m_ticker;
    }

    std::shared_ptr<Player> GameServer::GetPlayer(uint64_t aid) {
        {
            std::shared_lock lock(m_playersMutex);
            auto player = FindInMap(m_players, aid);
            if (player)
                return player.value();
        }

        // create new one if it does not exist
        std::unique_lock lockUnique(m_playersMutex);
        auto[iterator, _] = m_players.try_emplace(aid, std::make_shared<Player>(aid));
        M_LOG_INFO(iterator->second->GetLogger()) << "Joined the game";
        return iterator->second;
    }

    void GameServer::Tick() {
        if (IsPast(m_oneSecondTasks)) {
            {
                std::unique_lock lock(m_playersMutex);

                // remove inactive players
                for (auto iterator = m_players.cbegin(); iterator != m_players.cend();) {
                    const std::shared_ptr<Player>& player = iterator->second;
                    std::unique_lock playerLock(player->GetDataMutex());

                    if (IsPast(player->GetTimeout())) {
                        player->SetInitLevel(InitLevel::None);
                        M_LOG_INFO(player->GetLogger()) << "Left the game";
                        playerLock.unlock();

                        m_players.erase(iterator++);
                    } else {
                        ++iterator;
                    }
                }
            }

            m_oneSecondTasks = PointInFuture<std::chrono::seconds>(1);
        }
    }
}
