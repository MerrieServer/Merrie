#ifndef MERRIE_COMMONS_HEADERS_INCLUDES_COMMONS_CRYPTO_DIGEST_HPP
#define MERRIE_COMMONS_HEADERS_INCLUDES_COMMONS_CRYPTO_DIGEST_HPP
#include "../Commons.hpp"
#include "../CWrapper.hpp"

#ifdef M_HAS_OPENSSL_CRYPTO
#   include <openssl/sha.h>
#   include <openssl/md4.h>
#   include <openssl/md5.h>
#endif

namespace Merrie {

    // ================================================================================
    // =  Functions for converting digest bytes to a hash strings                     =
    // ================================================================================

    /**
     * Coverts the given digest to a hex string.
     *
     * @param digest digest bytes to convert.
     *
     * @return the hash string, it is guaranteed that is has the same length as the provided digest * 2, is be padded with zeros if required and is lowercase.
     */
    std::string DigestToHex(const std::vector<uint8_t>& digest);

    /**
     * Coverts the given digest to a hex string as if the digest was a two's complement signed integer.
     *
     * @param digest digest bytes to convert.
     *
     * @return the bigint string, it may be shorter than the digest (leading zeros may be omitted), is lowercase and may be prefixed with "-" if the number was negative.
     */
    std::string DigestToBigInt(const std::vector<unsigned char>& digest);

    #ifdef M_HAS_OPENSSL_CRYPTO

    // ================================================================================
    // =  Raw OpenSSL digest function types.                                          =
    // ================================================================================
    template<typename CTX>
    using InitFunction = std::add_pointer_t<int(CTX* c)>;

    template<typename CTX>
    using UpdateFunction = std::add_pointer_t<int(CTX* c, const void* data, size_t length)>;

    template<typename CTX>
    using FinalFunction = std::add_pointer_t<int(unsigned char* md, CTX* c)>;


    // ================================================================================
    // =  Wrapper for an OpenSSL digest                                               =
    // ================================================================================

    /**
     * Wrapper for an OpenSSL digest.
     *
     * @tparam CTX type of the CTX struct for the given digest.
     * @tparam DataType type of the data that the digest Update function takes
     * @tparam OutputSize size of the output buffer
     * @tparam InitF function for initializing the context
     * @tparam UpdateF function for updating the context with data
     * @tparam FinalF function for finalizing the context
     */
    template<
            typename CTX,
            size_t OutputSize,
            InitFunction<CTX> InitF,
            UpdateFunction<CTX> UpdateF,
            FinalFunction<CTX> FinalF
    >
    class Digest {
        public: // Constructors & Destructors

            /*
             * Constructs a new Digest and initializes it.
             */
            Digest() {
                const int success = InitF(&m_ctx);
                M_ASSERT(success, "Failed to init Digest");
            }

            ~Digest() noexcept = default;

            TRIVIALLY_COPYABLE(Digest);
            TRIVIALLY_MOVEABLE(Digest);

        public: // Public methods

            /**
             * Updates the digest with data from the given vector.
             *
             * @param type vector with data
             */
            void Update(const std::vector<uint8_t>& type) {
                Update(static_cast<void*>(type.data()), type.size());
            }

            /**
             * Updates the digest with the given data.
             *
             * @param data data to update the digest with
             * @param length length of that data, in bytes
             */
            void Update(const void* data, size_t length) {
                M_ASSERT(!m_finalized, "Digest already finalized");
                const int success = UpdateF(&m_ctx, data, length);
                M_ASSERT(success, "Failed to update Digest");
            }

            /**
             * Finalizes the digest and returns a vector with the output.
             *
             * @return vector with the digest bytes
             */
            std::vector<uint8_t> Finalize() {
                M_ASSERT(!m_finalized, "Digest already finalized");

                std::vector<uint8_t> bytes(OutputSize);
                const int success = FinalF(bytes.data(), &m_ctx);
                M_ASSERT(success, "Failed to finalize Digest");
                m_finalized = true;

                return bytes;
            }

            /**
             * Finalizes the digest and uses DigestToHex to convert it into a string.
             *
             * @return string digest representation.
             */
            std::string FinalizeAsHex() {
                return DigestToHex(Finalize());
            }

            /**
             * Finalizes the digest and uses DigestToHex to convert it into a string.
             *
             * @return bigint string digest representation.
             */
            std::string FinalizeAsBigInt() {
                std::vector<uint8_t> digest = Finalize();
                return DigestToBigInt(std::move(digest));
            }

        private: // Private fields
            CTX m_ctx{};
            bool m_finalized = false;
    };


    // ================================================================================
    // =  Ready-to-use OpenSSL digest types                                           =
    // ================================================================================
    // @formatter:off
    using MD4_Digest    = Digest<MD4_CTX,     MD4_DIGEST_LENGTH,       &MD4_Init,      &MD4_Update,       &MD4_Final>;
    using MD5_Digest    = Digest<MD5_CTX,     MD5_DIGEST_LENGTH,       &MD5_Init,      &MD5_Update,       &MD5_Final>;
    using SHA1_Digest   = Digest<SHA_CTX,     SHA_DIGEST_LENGTH,       &SHA1_Init,     &SHA1_Update,      &SHA1_Final>;
    using SHA224_Digest = Digest<SHA256_CTX,  SHA224_DIGEST_LENGTH,    &SHA224_Init,   &SHA224_Update,    &SHA224_Final>;
    using SHA256_Digest = Digest<SHA256_CTX,  SHA256_DIGEST_LENGTH,    &SHA256_Init,   &SHA256_Update,    &SHA256_Final>;
    using SHA384_Digest = Digest<SHA512_CTX,  SHA384_DIGEST_LENGTH,    &SHA384_Init,   &SHA384_Update,    &SHA384_Final>;
    using SHA512_Digest = Digest<SHA512_CTX,  SHA512_DIGEST_LENGTH,    &SHA512_Init,   &SHA512_Update,    &SHA512_Final>;
    // @formatter:on

    #endif // M_HAS_OPENSSL_CRYPTO

} // namespace Merrie

#endif //MERRIE_COMMONS_HEADERS_INCLUDES_COMMONS_CRYPTO_DIGEST_HPP
