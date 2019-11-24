#include <gtest/gtest.h>


#ifdef M_HAS_OPENSSL_CRYPTO
#include <Commons/Crypto/OpenSSL.hpp>
#include <Commons/Crypto/Digest.hpp>

class TestDigest : public ::testing::Test {
    private:
        Merrie::OpenSslContext m_context;
};

namespace {
    template<typename Digest>
    inline void _DoTestDigest(std::string_view raw, std::string_view expectedHash, std::string_view expectedBigInt) {
        Digest digest;
        digest.Update(raw.data(), raw.size());

        const std::vector<uint8_t> data = digest.Finalize();
        const std::string hash = Merrie::DigestToHex(data);
        const std::string bigint = Merrie::DigestToBigInt(data);

        EXPECT_EQ(hash, expectedHash) << "Invalid hash";
        EXPECT_EQ(bigint, expectedBigInt) << "Invalid bigint";
    }
}  // namespace

TEST_F(TestDigest, TestDigests) {
    using namespace Merrie;
    _DoTestDigest<MD4_Digest>(
            "Hello!",
            "f7d1caa57c3dff988c6f881036c4c273",
            "-82e355a83c20067739077efc93b3d8d"
    );

    _DoTestDigest<MD5_Digest>(
            "Hello!",
            "952d2c56d0485958336747bcdd98590d",
            "-6ad2d3a92fb7a6a7cc98b8432267a6f3"
    );

    _DoTestDigest<SHA1_Digest>(
            "Hello!",
            "69342c5c39e5ae5f0077aecc32c0f81811fb8193",
            "69342c5c39e5ae5f0077aecc32c0f81811fb8193"
    );

    _DoTestDigest<SHA224_Digest>(
            "Hello!",
            "e250e4339955f02294ceb623860ae5f9374b93f4335bd605ada5113a",
            "-1daf1bcc66aa0fdd6b3149dc79f51a06c8b46c0bcca429fa525aeec6"
    );

    _DoTestDigest<SHA256_Digest>(
            "Hello!",
            "334d016f755cd6dc58c53a86e183882f8ec14f52fb05345887c8a5edd42c87b7",
            "334d016f755cd6dc58c53a86e183882f8ec14f52fb05345887c8a5edd42c87b7"
    );

    _DoTestDigest<SHA384_Digest>(
            "Hello!!!!!!!!!!!!!!!!!!!!!!!",
            "042ae6c1e2e8ad763603f8b8117bfddd986ffecafe9ae3af825811db939a6f38e4520fe54360b17f340f4fc59d3947fd",
            "42ae6c1e2e8ad763603f8b8117bfddd986ffecafe9ae3af825811db939a6f38e4520fe54360b17f340f4fc59d3947fd"
    );

    _DoTestDigest<SHA512_Digest>(
            "Hello!",
            "3a928aa2cc3bf291a4657d1b51e0e087dfb1dea060c89d20776b8943d24e712ea65778fe608ddaee0a191bc6680483ad12be1f357389a2380f660db246be5844",
            "3a928aa2cc3bf291a4657d1b51e0e087dfb1dea060c89d20776b8943d24e712ea65778fe608ddaee0a191bc6680483ad12be1f357389a2380f660db246be5844"
    );
}

#endif // M_HAS_OPENSSL_CRYPTO