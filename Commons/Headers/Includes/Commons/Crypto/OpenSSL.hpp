#ifndef MERRIE_COMMONS_HEADERS_INCLUDES_COMMONS_CRYPTO_OPENSSL_HPP
#define MERRIE_COMMONS_HEADERS_INCLUDES_COMMONS_CRYPTO_OPENSSL_HPP

#include "../Commons.hpp"

namespace Merrie {

    #if defined(M_HAS_OPENSSL_CRYPTO) || defined(M_HAS_OPENSSL_SSL)

    /**
      * RAII class for initializing and destroying the OpenSSL context
      */
    class OpenSslContext {
        public: // Constructors & destructors

            /**
             * Initialized the OpenSSL context
             */
            OpenSslContext();

            /**
             * Destroys the OpenSSL context
             */
            ~OpenSslContext();

            NON_COPYABLE(OpenSslContext);
            NON_MOVEABLE(OpenSslContext);
    };

    #endif
} // namespace Merrie

#endif //MERRIE_COMMONS_HEADERS_INCLUDES_COMMONS_CRYPTO_OPENSSL_HPP
