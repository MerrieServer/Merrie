#include <gtest/gtest.h>

#include <Commons/Network/Http.hpp>

TEST(TestHttp, TestDecodeUrlQueryString)
{
    const std::string_view query =
            "testing_path"
            "?hello=this"
            "&keyonly1"
            "&polish=za%C5%BC%C3%B3%C5%82%C4%87%20g%C4%99%C5%9Bl%C4%85%20ja%C5%BA%C5%84"
            "&last=wrong"
            "&last=correct"
            "&keyonly2";
    Merrie::DecodedUrl decoded = Merrie::DecodeUrlQueryString(query);

    EXPECT_EQ(decoded.Path, "testing_path");
    EXPECT_EQ(decoded.Parameters["hello"], "this");
    EXPECT_EQ(decoded.Parameters["keyonly1"], "");
    EXPECT_EQ(decoded.Parameters["polish"], "zażółć gęślą jaźń");
    EXPECT_EQ(decoded.Parameters["last"], "correct");
    EXPECT_EQ(decoded.Parameters["keyonly2"], "");
}