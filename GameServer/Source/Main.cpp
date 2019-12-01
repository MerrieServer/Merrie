#include <Commons/ApplicationMain.hpp>
#include <Commons/Logging.hpp>
#include <Commons/Ticker.hpp>
#include <GameServer/GameServer.hpp>
#include <boost/filesystem.hpp>
#include <yaml-cpp/yaml.h>

namespace Merrie {
    GameServerSettings _ReadSettings() {

        const std::string configFile = "config.yml";

        if (!boost::filesystem::exists(configFile)) {
            YAML::Node config;
            config["tps"] = 100;
            config["log_filters"] = std::vector<std::string>{};

            config["http"] = YAML::Node();
            config["http"]["bind_ip"] = "127.0.0.1";
            config["http"]["bind_port"] = 80;
            config["http"]["worker_threads"] = 256;
            config["http"]["request_timeout"] = 15;
            config["http"]["keepalive"] = YAML::Node();
            config["http"]["keepalive"]["enabled"] = true;
            config["http"]["keepalive"]["timeout"] = 15;
            config["http"]["keepalive"]["max"] = 5000;

            std::ofstream file("config.yml");
            file << config;
        }

        const YAML::Node config = YAML::LoadFile(configFile);

        return GameServerSettings{
                {
                        {
                                config["http"]["bind_ip"].as<std::string>(),
                                config["http"]["bind_port"].as<NetworkPort>(),
                                config["http"]["worker_threads"].as<size_t>(),
                        },
                        config["http"]["keepalive"]["enabled"].as<bool>(),
                        config["http"]["request_timeout"].as<uint16_t>(),
                        config["http"]["keepalive"]["timeout"].as<uint16_t>(),
                        config["http"]["keepalive"]["max"].as<uint16_t>(),
                },
                config["tps"].as<unsigned int>(),
                config["log_filters"].as<std::vector<std::string>>()
        };
    }

    int ApplicationMain(int, char* []) {
        LoggingSystem loggingSystem;
        Logger initLogger = LoggingSystem::InitLogger("INIT");

        M_LOG_INFO(initLogger) << "Reading the config.yml file";
        GameServerSettings settings = _ReadSettings();
        LoggingSystem::UpdateFilters(settings.LogFilters);

        M_LOG_INFO(initLogger) << "Initializing the server";
        std::unique_ptr<GameServer> gameServer = std::make_unique<GameServer>(std::move(settings));
        gameServer->Start();

        while (gameServer->IsRunning()) {
            gameServer->GetTicker()->DoTick();
        }

        return 0;
    }
}