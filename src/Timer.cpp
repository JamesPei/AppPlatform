#include "Timer.hpp"
#include "TimerScheduler.hpp"
#include <thread>

#ifdef DEBUG_MODE
#include <iostream>
#include <iomanip> 
#endif

uint64_t Timer::last_id = 0;

Timer::Timer(Timer::Type type, uint64_t duration, uint64_t start_time): type_(type), duration_(duration), 
    start_timepoint_(start_time), 
    scheduler(TimerScheduler::Instance()) 
{
    id_ = generateTimerId();
    if(start_timepoint_==0)
    {
        const time_point now = system_clock::now();
        start_timepoint_ = std::chrono::time_point_cast<nanoseconds>(now).time_since_epoch().count();
    }
    deadline_timepoint_ = start_timepoint_ + duration_;

    if(type==Type::PERIODIC){
        round_ = 1;
    }
}

uint64_t Timer::GetId(){
    return id_;
};

int Timer::Wait(){
    system_clock::time_point end_timepoint = system_clock::time_point(nanoseconds(deadline_timepoint_));
    if(type_==Type::ONESHOT)
    {
        if(system_clock::now() > end_timepoint){
            return 2;
        }
        std::this_thread::sleep_until<system_clock, nanoseconds>(end_timepoint);
    }else{
        if(system_clock::now() > end_timepoint){
            return 2;
        }
        std::this_thread::sleep_until<system_clock>(end_timepoint);
        start_timepoint_ = deadline_timepoint_;
        round_++;
        deadline_timepoint_ = start_timepoint_ + duration_;
#ifdef DEBUG_MODE
        std::cout << std::left << std::setw(16) << "next timepoint:" << start_timepoint_ << "\n";
        std::cout << "round:" << round_ << "\n";
#endif
    }
    return 1;
};

bool Timer::IsExpired(uint64_t round){
    time_point timeout = system_clock::time_point(nanoseconds(start_timepoint_));
    if(type_==Type::ONESHOT)
    {
        nanoseconds duration(duration_);
        timeout += duration;
        if(system_clock::now() > timeout){
            return true;
        }
    }else{
        nanoseconds duration(duration_*round);
        if(system_clock::now() > (timeout+=duration)){
            return true;
        }
    }
    return false;
};

bool Timer::IsRunning(){
    return true;
};


uint64_t Timer::GetTimerRound(){
    if(type_ == Type::ONESHOT){
        return 0;
    }
    return round_;
};
    
void Timer::Cancel(){
    scheduler.CancelEvent(id_);
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

uint64_t Timer::GetDeadline() const {
    return deadline_timepoint_;
};
    
bool Timer::operator<=(const Timer& timer){
    if(timer.GetDeadline() <= deadline_timepoint_){
        return true;
    }
};
