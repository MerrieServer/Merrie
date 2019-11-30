#include <Commons/Random.hpp>

namespace Merrie {

    std::random_device& GetCommonRandomDevice() {
        static std::random_device c_randomDevice;
        return c_randomDevice;
    }

    std::mt19937_64& GetCommonMt19937() {
        static std::mt19937_64 c_mt19937(GetCommonRandomDevice()());
        return c_mt19937;
    }
}