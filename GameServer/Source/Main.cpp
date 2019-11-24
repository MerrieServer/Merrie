#include <Commons/ApplicationMain.hpp>
#include <Commons/Logging.hpp>
#include <GameServer/GameServer.hpp>

namespace Merrie {
    int ApplicationMain(int, char* []) {
        LoggingSystem loggingSystem;
        GameServer gameServer;
        gameServer.Start();

        return 3;
    }
}