#include "TimerScheduler.hpp"
#include "Shortnames.hpp"
#include <thread>
#include <chrono>
#include <algorithm>

TimerScheduler::TimerScheduler():events([](const Event& a, const Event& b){ return a.deadline < b.deadline; }){
    std::thread(&TimerScheduler::RunTask, this);
}

void TimerScheduler::RegisterEvent(uint64_t id, uint64_t deadline, std::function<void()> callback_, uint64_t period){
    Event event{id, deadline, callback_, period};    
    events.push(event);
};

void TimerScheduler::CancelEvent(uint64_t id){
    depreacted_events.push_back(id);
};

void TimerScheduler::RunTask(){
    while (1)
    {
        if(events.empty()){
            continue;
        }else{
            Event nearest_event = events.top();
            time_point nearest_timepoint = time_point(nanoseconds(nearest_event.deadline));
            time_point now = system_clock::now();

            if(now > nearest_timepoint){
                // expired

                uint64_t id = nearest_event.id;
                uint64_t deadline = nearest_event.deadline;
                // uint64_t callback = nearest_event.callback_;
                // uint64_t id = nearest_event.id;
                events.pop();
                if(nearest_event.period!=0){
                    Event next_event{id, deadline};
                    events.push(next_event)
                }

                continue;
            }else if (now  < nearest_timepoint ){
                std::this_thread::sleep_until(nearest_timepoint);
            }

            auto p_id = std::find(depreacted_events.cbegin(), depreacted_events.cend(), nearest_event.id);
            if(p_id!=depreacted_events.cend()){
                nearest_event.callback_();
            }
            events.pop();
        }
    }
    
}