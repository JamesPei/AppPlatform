#include "TimerScheduler.hpp"
#include <thread>
#include <algorithm>

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
        throw std::runtime_error(e.what());
    }
}

bool TimerScheduler::IsRunning(){
    return scheduler_thread.joinable();
}

void TimerScheduler::RegisterEvent(uint64_t id, uint64_t deadline, std::function<void()> callback_, uint64_t period){
    if(!IsRunning()){
        Start();
    };
    Event event{id, deadline, callback_, period};    
    std::lock_guard<std::mutex> lock(mtx);
    events.push(event);
};

void TimerScheduler::CancelEvent(uint64_t id){
    depreacted_events.push_back(id);
};

int TimerScheduler::SetEventState(uint64_t id, int state){
    std::lock_guard<std::mutex> lock(mtx);
#ifdef DEBUG_MODE
        std::cerr << "SetEventState:" << id << "state:" << state << "\n";
#endif
    if(events_state.count(id)==1){
        events_state[id].round++;
        events_state[id].state = state;
    }else{
        EventState event_state{id,1,state};
        events_state.insert({id, event_state});
    }
    return 1;
};

void TimerScheduler::RunTask(time_point timepoint, TimerScheduler::Event event){
    // waiting
    SetEventState(event.id, 1);
    std::this_thread::sleep_until(timepoint);
    // running
    SetEventState(event.id, 2);
    event.callback_();
    // ending
    SetEventState(event.id, 3);
};

int TimerScheduler::GetEventState(uint64_t id){
    if(events_state.count(id)==1){
        return events_state[id].state;
    }
    return -1;
};

int TimerScheduler::GetEventRound(uint64_t id){
    if(events_state.count(id)==1){
        return events_state[id].round;
    }
    return -1;
}

void TimerScheduler::Schedule(){
    while (!stop_flag)
    {
        if(events.empty()){
            continue;
        }else{
            Event nearest_event(events.top());
            {
                std::lock_guard<std::mutex> lock(mtx);
                events.pop();
            }
            uint64_t event_id = nearest_event.id;
            time_point nearest_timepoint = time_point(nanoseconds(nearest_event.deadline));
            time_point now = system_clock::now();

            if(!depreacted_events.empty()){
                auto p_id = std::find(depreacted_events.cbegin(), depreacted_events.cend(), nearest_event.id);
                if(p_id!=depreacted_events.cend()){
                    // cancaled task
                    continue;
                }
            }

            if(now > nearest_timepoint){
                // expired
                if(nearest_event.period!=0){
                    Event next_event(nearest_event);
                    // periodic task
                    next_event.deadline += next_event.period;
                    std::lock_guard<std::mutex> lock(mtx);
                    events.push(next_event);
                }

                continue;
            }

            try{
                std::thread t(&TimerScheduler::RunTask, this, nearest_timepoint, nearest_event);
                t.detach();
            }catch(const std::exception& e){
                throw std::runtime_error(e.what());
            }
        }
    }
    
}