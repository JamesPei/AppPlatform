#include "TimerScheduler.hpp"
#include <thread>
#include <algorithm>

#ifdef DEBUG_MODE
    #include <iostream>
    #include <iomanip>
#endif
    
void RunTask(time_point timepoint, TimerScheduler::Event event){
    std::this_thread::sleep_until(timepoint);
    event.callback_();
};

TimerScheduler::TimerScheduler():
    events([](const Event& a, const Event& b){ return a.deadline > b.deadline; }),
    stop_flag(false){};

TimerScheduler::~TimerScheduler(){
    if(scheduler_thread.joinable()){
        stop_flag = true;
        scheduler_thread.join();
    }
};

void TimerScheduler::Start(){
    try{
        scheduler_thread = std::thread(&TimerScheduler::Schedule, this);
    }catch(const std::exception& e){
#ifdef DEBUG_MODE
        std::cerr << "Create Thread Error:" << e.what() << std::endl;
#endif
        throw std::runtime_error(e.what());
    }
}

bool TimerScheduler::IsRunning(){
    return scheduler_thread.joinable();
}

void TimerScheduler::RegisterEvent(uint64_t id, uint64_t deadline, std::function<void()> callback_, uint64_t period){
    Event event{id, deadline, callback_, period};    
    events.push(event);
};

void TimerScheduler::CancelEvent(uint64_t id){
    depreacted_events.push_back(id);
};

void TimerScheduler::Schedule(){
    while (!stop_flag)
    {
        if(events.empty()){
            continue;
        }else{
            Event nearest_event(events.top());
            events.pop();
#ifdef DEBUG_MODE
    std::cout << "top timer:" << nearest_event.id << "\n";
#endif
            time_point nearest_timepoint = time_point(nanoseconds(nearest_event.deadline));
            time_point now = system_clock::now();

            if(!depreacted_events.empty()){
                auto p_id = std::find(depreacted_events.cbegin(), depreacted_events.cend(), nearest_event.id);
                if(p_id!=depreacted_events.cend()){
                    // cancaled task
#ifdef DEBUG_MODE
    std::cout << "cancaled task" << nearest_event.id << "\n";
#endif
                    continue;
                }
            }

            if(now > nearest_timepoint){
                // expired
                Event next_event(nearest_event);

#ifdef DEBUG_MODE
    std::cout << "overtimed task:" << nearest_event.id << "\n";
    std::cout << std::setw(9) << "now:" << std::chrono::time_point_cast<nanoseconds>(now).time_since_epoch().count() - TIME_BASE
        << std::setw(9) << "\novertime:" 
        << std::chrono::time_point_cast<nanoseconds>(nearest_timepoint).time_since_epoch().count() - TIME_BASE << "\n";
#endif
                if(nearest_event.period!=0){
                    // periodic task
                    next_event.deadline += next_event.period;
                    events.push(next_event);
                }

                continue;
            }

            try{
                std::thread t(RunTask, nearest_timepoint, nearest_event);
                t.detach();
            }catch(const std::exception& e){
#ifdef DEBUG_MODE
        std::cerr << "Create Thread Error:" << e.what() << std::endl;
#endif
                throw std::runtime_error(e.what());
            }


#ifdef DEBUG_MODE
    std::cout << std::endl;
#endif
        }
    }
    
}