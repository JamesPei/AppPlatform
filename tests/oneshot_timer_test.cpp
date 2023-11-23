#include <gtest/gtest.h>
#include "Timer.hpp"
#include <numeric>
#include <vector>

void test_function(int a){
    std::cout << "test_function:" << a << std::endl;
}

void sleep_n_seconds(uint32_t s){
    std::this_thread::sleep_for(seconds(s));
    std::cout << "sleep " << s << " seconds" << std::endl;
}

TEST(OneShotTimer, GetId)
{
    Timer timer = Timer::CreateOneshotTimer(1000000000); // 1s
    ASSERT_EQ(timer.GetId(), 1);
    Timer timer2 = Timer::CreateOneshotTimer(1000000000); //1s
    ASSERT_EQ(timer2.GetId(), 2);
}

TEST(OneShotTimer, Wait)
{
// normal
    uint64_t start = system_clock::now().time_since_epoch().count();
    Timer timer = Timer::CreateOneshotTimer(1000000000); // 1s
    ASSERT_EQ(timer.GetId(), 1);
    int res = timer.Wait();
    uint64_t end = system_clock::now().time_since_epoch().count();
    ASSERT_EQ(res, 1);
    EXPECT_LT(end-start, 1001000000);  // 1.001s

    // expired 
    Timer timer2 = Timer::CreateOneshotTimer(500000000); // 0.5s
    ASSERT_EQ(timer2.GetId(), 2);
    std::this_thread::sleep_for(seconds(1));
    res = timer2.Wait();
    ASSERT_EQ(res, 2);
}

TEST(OneShotTimer, WaitAsync)
{
    // normal
    uint64_t start = system_clock::now().time_since_epoch().count();
    Timer timer = Timer::CreateOneshotTimer(1000000000); // 1s
    ASSERT_EQ(timer.GetId(), 1);
    timer.WaitAsync(test_function, 1);
    uint64_t end = system_clock::now().time_since_epoch().count();
    EXPECT_LT(end-start, 1000000);  // 1ms
}

TEST(OneShotTimer, IsExpired)
{
    Timer timer = Timer::CreateOneshotTimer(1000000000);  // 1s
    EXPECT_FALSE(timer.IsExpired());

    Timer timer2 = Timer::CreateOneshotTimer(1000000000);  // 1s
    std::this_thread::sleep_for(seconds(1));
    EXPECT_TRUE(timer2.IsExpired());
}

TEST(OneShotTimer, GetDeadline)
{
    uint64_t start = system_clock::now().time_since_epoch().count();
    Timer timer = Timer::CreateOneshotTimer(1000000000);  // 1s
    EXPECT_LT(timer.GetDeadline(), start+1001000000);     // 1.001s
}

TEST(OneShotTimer, IsRunning)
{
    Timer timer = Timer::CreateOneshotTimer(1000000); // 1ms
    EXPECT_FALSE(timer.IsExpired());
    timer.WaitAsync(sleep_n_seconds, 1);
    std::this_thread::sleep_for(milliseconds(500));   // 500ms
    EXPECT_TRUE(timer.IsRunning());

    Timer timer2 = Timer::CreateOneshotTimer(1000000); // 1ms
    EXPECT_FALSE(timer2.IsExpired());
    timer.WaitAsync(test_function, 1);
    std::this_thread::sleep_for(milliseconds(100));   // 100ms
    EXPECT_FALSE(timer2.IsRunning());
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}