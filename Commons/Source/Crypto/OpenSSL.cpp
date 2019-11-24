#include <Commons/Crypto/OpenSSL.hpp>

#include <openssl/evp.h>
#include <openssl/err.h>

namespace Merrie {
    #if defined(M_HAS_OPENSSL_CRYPTO) || defined(M_HAS_OPENSSL_SSL)

    OpenSslContext::OpenSslContext() {
        #ifdef M_HAS_OPENSSL_CRYPTO
        ERR_load_crypto_strings();
        OpenSSL_add_all_algorithms();
        #endif
    }

    OpenSslContext::~OpenSslContext() {
        #ifdef M_HAS_OPENSSL_CRYPTO
        EVP_cleanup();
        CRYPTO_cleanup_all_ex_data();
        ERR_free_strings();
        #endif
    }

    #endif
} // namespace Merrie::Crypto