#include "Timer.hpp"
#include "TimerScheduler.hpp"
#include <thread>
#include <iostream>
#include <iomanip> 

uint64_t Timer::last_id = 0;

Timer::Timer(Timer::Type type, uint64_t duration, uint64_t start_time): type_(type), duration_(duration), start_timepoint_(start_time){
    id_ = generateTimerId();
    if(start_timepoint_==0)
    {
        const time_point now = system_clock::now();
        start_timepoint_ = std::chrono::time_point_cast<nanoseconds>(now).time_since_epoch().count();
    }
}

uint64_t Timer::GetId(){
    return id_;
};

int Timer::Wait(){
    time_point timeout = system_clock::time_point(nanoseconds(start_timepoint_));;
    if(type_==Type::ONESHOT)
    {
        nanoseconds duration(duration_);
        timeout += duration;
#ifdef DEBUG_MODE
        std::cout << std::setw(9) << "start:" << start_timepoint_ << "\n";
        std::cout << std::setw(9) << "now:" << std::chrono::time_point_cast<nanoseconds>(system_clock::now()).time_since_epoch().count() << "\n";
        std::cout << std::setw(9) << "timeout:" << std::chrono::time_point_cast<nanoseconds>(timeout).time_since_epoch().count() << "\n";
#endif
        if(system_clock::now() > timeout){
            return 2;
        }
        std::this_thread::sleep_until<system_clock, nanoseconds>(timeout);
#ifdef DEBUG_MODE
        std::cout << std::setw(9) << "End:" << std::chrono::time_point_cast<nanoseconds>(timeout).time_since_epoch().count() << "\n";
#endif
    }else{
        timeout = system_clock::from_time_t(start_timepoint_);
        nanoseconds duration(duration_);
        timeout += duration;
        if(system_clock::now() > timeout){
            return 2;
        }
        std::this_thread::sleep_until<system_clock>(timeout);
        start_timepoint_ = std::chrono::time_point_cast<seconds>(system_clock::now()).time_since_epoch().count();
        round_++;
    }
    return 1;
};

bool Timer::IsExpired(uint64_t round){
    if(type_==Type::ONESHOT)
    {

    }else{

    }
    return true;
};

bool Timer::IsRunning(){
    return true;
};


uint64_t Timer::GetTimerRound(){
    return round_;
};
    
void Timer::Cancel(){

};
    
Timer Timer::CreatePeriodicTimer(uint64_t duration, uint64_t start_time){
    return Timer(Type::PERIODIC, duration, start_time);
};


Timer Timer::CreateOneshotTimer(uint64_t duration, uint64_t start_time){
    return Timer(Type::ONESHOT, duration, start_time);
};
 
uint64_t Timer::generateTimerId(){
    return ++last_id;
};