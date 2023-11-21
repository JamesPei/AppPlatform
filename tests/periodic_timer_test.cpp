#include <gtest/gtest.h>
#include "Timer.hpp"
#include <numeric>
#include <vector>

TEST(PeriodTimer, GetId)
{
    Timer timer = Timer::CreatePeriodicTimer(1000000000);
    EXPECT_EQ(timer.GetId(), 1);
}

TEST(PeriodTimer, Wait)
{
    uint64_t start = system_clock::now().time_since_epoch().count();
    Timer timer = Timer::CreatePeriodicTimer(1000000000);
    ASSERT_EQ(timer.GetId(), 1);
    for(int i=0; i<3; i++){
        int res = timer.Wait();
        uint64_t end = system_clock::now().time_since_epoch().count();
        ASSERT_EQ(res, 1);
        EXPECT_LT(end-start, 1001000000);
        start = end;
    }
}

TEST(PeriodTimer, GetDeadline)
{
    uint64_t start = system_clock::now().time_since_epoch().count();
    Timer timer = Timer::CreatePeriodicTimer(1000000000);
    EXPECT_LT(timer.GetDeadline(), start+1001000000);
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}