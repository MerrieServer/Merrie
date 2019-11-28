#include <Commons/ApplicationMain.hpp>
#include <Commons/Logging.hpp>
#include <Commons/Ticker.hpp>
#include <GameServer/GameServer.hpp>

namespace Merrie {
    int ApplicationMain(int, char* []) {
        LoggingSystem loggingSystem;

        GameServer gameServer;
        gameServer.Start();

        while (gameServer.IsRunning()) {
            gameServer.GetTicker()->DoTick();
        }

        return 0;
    }
}