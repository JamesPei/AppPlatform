#include <gtest/gtest.h>
#include "Timer.hpp"
#include <numeric>
#include <vector>

void test_function(int a, std::string b="test"){
    std::cout << "int:" << a << " string:" << b << std::endl;
}

void sleep_n_seconds(uint32_t s){
    std::this_thread::sleep_for(seconds(s));
    std::cout << "sleep " << s << " seconds" << std::endl;
}

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

TEST(PeriodTimer, WaitAsync){
    uint64_t start = system_clock::now().time_since_epoch().count();
    Timer timer = Timer::CreatePeriodicTimer(1000000000);
    timer.WaitAsync(test_function, 1, std::string("4 test"));
    uint64_t end = system_clock::now().time_since_epoch().count();
    EXPECT_LT(end-start, 1000000);  // 1ms
}

TEST(PeriodTimer, IsExpired){
    Timer timer = Timer::CreatePeriodicTimer(10000000); // 10ms
    EXPECT_FALSE(timer.IsExpired());
    int res = timer.Wait();
    ASSERT_EQ(res, 1);
    std::this_thread::sleep_for(milliseconds(20));
    EXPECT_TRUE(timer.IsExpired());
}

TEST(PeriodTimer, GetTimerRound){
    Timer timer = Timer::CreatePeriodicTimer(10000000); // 10ms
    int res;
    uint64_t round;
    for(int i=1; i<10; i++){
        round = timer.GetTimerRound();
        ASSERT_EQ(round, i);
        res = timer.Wait();
        ASSERT_EQ(res, 1);
    }
}

TEST(PeriodTimer, IsRunning){
    Timer timer = Timer::CreatePeriodicTimer(1000000000); // 1s
    timer.WaitAsync(sleep_n_seconds, 1);
    std::this_thread::sleep_for(milliseconds(1500));
    EXPECT_TRUE(timer.IsRunning());
}

TEST(PeriodTimer, Cancel){
    Timer timer = Timer::CreatePeriodicTimer(1000000000); // 1s
    timer.WaitAsync(sleep_n_seconds, 1);
    std::this_thread::sleep_for(milliseconds(1100));
    EXPECT_TRUE(timer.IsRunning());
    timer.Cancel();
    std::this_thread::sleep_for(milliseconds(2000));
    EXPECT_FALSE(timer.IsRunning());
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