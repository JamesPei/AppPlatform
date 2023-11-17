#include "Timer.hpp"
#include <iostream>

int main(int argc, char const *argv[])
{
    std::cout << "Start:" << std::chrono::time_point_cast<milliseconds>(system_clock::now()).time_since_epoch().count() << "\n";
    for(int i=1; i<11; i++){
        Timer syn_timer = Timer::CreateOneshotTimer(500000000);
        uint64_t timer_id = syn_timer.GetId();
        std::cout << "timer id: " << timer_id << "\n";
        int res = syn_timer.Wait();
        std::cout << "res:" << res << "\n"; 
    }
    // Timer asyn_timer = Timer::CreatePeriodicTimer(2*1000000000);
    // uint64_t asyntimer_id = asyn_timer.GetId();
    // std::cout << "asyntimer id: " << asyntimer_id << "\n";

    std::cout << std::endl;
    return 0;
}
