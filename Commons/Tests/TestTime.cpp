#include <gtest/gtest.h>
#include <Commons/Time.hpp>

#include <thread>

TEST(TestTime, TestTime) {
    const auto point = Merrie::PointInFuture<std::chrono::seconds>(1);
    ASSERT_FALSE(Merrie::IsPast(point)) << "point in future is in past";
    std::this_thread::sleep_for(std::chrono::seconds(1));
    ASSERT_TRUE(Merrie::IsPast(point)) << "point in future is not in past after wait";
}