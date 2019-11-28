#include <gtest/gtest.h>
#include <Commons/Containers.hpp>

using namespace Merrie;

TEST(TestContainers, TestFindInMap) {
    std::map<std::string, std::string> map = {
            {"key1", "value1"},
            {"key2", "value2"}
    };

    const std::optional<std::string> value1 = FindInMap(map, "key1"s);
    const std::optional<std::string> value2 = FindInMap(map, "key2"s);
    const std::optional<std::string> valueNonExisting1 = FindInMap(map, "key3"s);
    const std::optional<std::string> valueNonExisting2 = FindInMap(map, "kEy3"s);

    EXPECT_EQ(value1, "value1");
    EXPECT_EQ(value2, "value2");
    EXPECT_FALSE(valueNonExisting1);
    EXPECT_FALSE(valueNonExisting2);
}

TEST(TestContainers, TestContains) {
    std::vector<std::string> vector = {
            "value1",
            "value2"
    };

    EXPECT_TRUE(Contains(vector, "value1"));
    EXPECT_TRUE(Contains(vector, "value2"));
    EXPECT_FALSE(Contains(vector, "value3"));
    EXPECT_FALSE(Contains(vector, "valuE1"));
}

TEST(TestContainers, TestRemoveIf) {
    std::vector<uint8_t> vector = {
            3, 4, 7, 2, 5, 1, 6
    };

    RemoveIf(vector, [](uint8_t value) { return value < 5; });

    EXPECT_EQ(vector.size(), 3);
    EXPECT_TRUE(Contains(vector, 5));
    EXPECT_TRUE(Contains(vector, 6));
    EXPECT_TRUE(Contains(vector, 7));
}