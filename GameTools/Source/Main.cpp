#include <Commons/ApplicationMain.hpp>
#include <Commons/Logging.hpp>

namespace Merrie {
    int ApplicationMain(int, char*[]) {
        LoggingSystem loggingSystem;

        Logger logger = LoggingSystem::InitLogger("Test123");
        M_LOG_INFO(logger) << "Test123";

        return 3;
    }
}