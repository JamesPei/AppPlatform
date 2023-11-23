#include "Timer.hpp"
#include <iostream>
#include <iomanip>
#include <assert.h>

void test_function(int a){
    std::cout << "test_function:" << a << std::endl;
}

void sleep_n_seconds(uint32_t n){
    std::cout << "sleep " << n << " seconds" <<  std::endl;
    std::this_thread::sleep_for(seconds(n));
}

int main(int argc, char const *argv[])
{
    std::cout << std::left << std::setw(10) << "Start:" << std::chrono::time_point_cast<nanoseconds>(system_clock::now()).time_since_epoch().count() - TIME_BASE << "\n";

    if(std::string(argv[1])=="wait"){
        std::cout << "=====sync wait test=====" << "\n";
        if(std::string(argv[2])=="oneshot"){
            std::cout << "====oneshot====" << "\n";
            for(uint64_t i=1; i<11; i++){
                Timer syn_timer = Timer::CreateOneshotTimer(1000000000);
                uint64_t timer_id = syn_timer.GetId();
                std::cout << "timer id: " << timer_id << " deadline:" << syn_timer.GetDeadline() << "\n";
                int res = syn_timer.Wait();
                std::cout << "res:" << res << "\n"; 
            }
        }else{
            std::cout << "====periodic====" << "\n";
            Timer asyn_timer = Timer::CreatePeriodicTimer(1*1000000000);
            uint64_t asyntimer_id = asyn_timer.GetId();
            std::cout << "asyntimer id: " << asyntimer_id << "\n";
            for(uint64_t i=1; i<11; i++){
                int res = asyn_timer.Wait();
                std::cout << "res:" << res << "\n"; 
            }
        }
    
    }else if (std::string(argv[1])=="waitasync"){
        std::cout << "=====async wait test=====" << "\n";

        if(std::string(argv[2])=="oneshot"){
            std::cout << "====oneshot====" << "\n";
            for(uint64_t i=1; i<11; i++){
                Timer syn_timer1 = Timer::CreateOneshotTimer(i*500000000);
                // uint64_t timer_id = syn_timer1.GetId();
                // std::cout << "timer id: " << timer_id << " deadline:" << syn_timer1.GetDeadline()-TIME_BASE << "\n";
                syn_timer1.WaitAsync(sleep_n_seconds, 1);
                std::this_thread::sleep_for(milliseconds(500));
            }
        }else{
            std::cout << "====periodic====" << "\n";
            for(uint64_t i=1; i<11; i++){
                Timer syn_timer1 = Timer::CreatePeriodicTimer(i*500000000);
                // uint64_t timer_id = syn_timer1.GetId();
                // std::cout << "timer id:" << timer_id << "\n";
                // std::cout << "timer id: " << timer_id << " deadline:" << syn_timer.GetDeadline() << "\n";
                syn_timer1.WaitAsync(test_function, i);
            }
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(30*1000));
    }
    
    std::cout << std::endl;
    return 0;
}
