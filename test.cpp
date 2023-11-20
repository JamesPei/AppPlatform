#include "Timer.hpp"
#include "Shortnames.hpp"
#include <iostream>
#include <iomanip>

int main(int argc, char const *argv[])
{
    std::cout << std::left << std::setw(10) << "Start:" << std::chrono::time_point_cast<nanoseconds>(system_clock::now()).time_since_epoch().count() - TIME_BASE << "\n";

    if(std::string(argv[1])=="wait"){
        std::cout << "=====sync wait test=====" << "\n";
        std::cout << "====oneshot====" << "\n";
        for(int i=1; i<11; i++){
            Timer syn_timer = Timer::CreateOneshotTimer(1000000000);
            uint64_t timer_id = syn_timer.GetId();
            std::cout << "timer id: " << timer_id << "\n";
            int res = syn_timer.Wait();
            std::cout << "res:" << res << "\n"; 
        }
    
        std::cout << "====periodic====" << "\n";
        Timer asyn_timer = Timer::CreatePeriodicTimer(1*1000000000);
        uint64_t asyntimer_id = asyn_timer.GetId();
        std::cout << "asyntimer id: " << asyntimer_id << "\n";
        for(int i=1; i<11; i++){
            int res = asyn_timer.Wait();
            std::cout << "res:" << res << "\n"; 
        }
    }else if (std::string(argv[1])=="waitasync"){
        std::cout << "=====async wait test=====" << "\n";
        std::cout << "====oneshot====" << "\n";
        std::function<void()> test_function = [](){ std::cout << "test" << std::endl;};
        for(int i=1; i<11; i++){
            Timer syn_timer1 = Timer::CreateOneshotTimer(1000000000);
            uint64_t timer_id = syn_timer1.GetId();
            // std::cout << "timer id:" << timer_id << "\n";
            syn_timer1.WaitAsync(test_function, i);
        }
        
        std::cout << "====periodic====" << "\n";
        for(int i=1; i<11; i++){
            Timer syn_timer1 = Timer::CreatePeriodicTimer(1000000000);
            uint64_t timer_id = syn_timer1.GetId();
            // std::cout << "timer id:" << timer_id << "\n";
            syn_timer1.WaitAsync(test_function, i);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(5*1000));
    }
    


    std::cout << std::endl;
    return 0;
}
