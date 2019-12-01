#include <Commons/Crypto/Digest.hpp>

#include <sstream>
#include <iomanip>

namespace Merrie {

#ifdef M_HAS_OPENSSL_CRYPTO

    std::string DigestToHex(const std::vector<uint8_t>& digest) {
        std::ostringstream stream;
        for (const auto& byte : digest) {
            stream << std::setfill('0') << std::setw(2) << std::hex << static_cast<unsigned int>(byte);
        }
        return stream.str();
    }

    std::string DigestToBigInt(const std::vector<unsigned char>& digest) {
        const size_t size = digest.size();
        const unsigned char highestByte = digest[0];
        const bool negative = static_cast<bool>(highestByte & 0b1000'0000U);

        std::string string;
        if (negative) {
            std::vector<uint8_t> fixedDigest(size);

            for (size_t i = 0; i < size; i++) {
                fixedDigest[i] = std::numeric_limits<uint8_t>::max() - digest[i];
            }

            for (auto i = static_cast<int32_t>(size - 1); i >= 0; i--) {
                fixedDigest[i]++;
                if (fixedDigest[i] != 0) {
                    break;
                }
            }

            string = DigestToHex(fixedDigest);
        } else {
            string = DigestToHex(digest);
        }

        string.erase(0, std::min(string.find_first_not_of('0'), string.size() - 1));

        if (negative) {
            string = "-" + string;
        }

        return string;
    }

#endif // M_HAS_OPENSSL_CRYPTO

}  // namespace Merrie::Crypto