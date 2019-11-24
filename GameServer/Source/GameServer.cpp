#include <GameServer/GameServer.hpp>

#include <GameServer/Network/GameHttpServer.hpp>


namespace Merrie {
    GameServer::GameServer() : m_running(false) {
        HttpServerSettings settings = {
                .NetworkServerSettings = {
                        .BindIp = "127.0.0.1",
                        .BindPort = 81,
                        .WorkerThreadCount = 8
                },
                .AllowKeepAlive = true,
                .KeepAliveTimeout = 15,
                .KeepAliveMax = 5000,
        }; // todo

        m_gameHttpServer = std::make_unique<GameHttpServer>(this, std::move(settings));
    }

    GameServer::~GameServer() = default;

    void GameServer::Start() {
        M_LOG_INFO_THIS << "Starting the game server";

        m_gameHttpServer->StartServer();
        m_gameHttpServer->Join();
    }
}
