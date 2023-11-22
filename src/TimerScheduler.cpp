#include "TimerScheduler.hpp"
#include <thread>
#include <algorithm>

#ifdef DEBUG_MODE
    #include <iostream>
    #include <iomanip>
#endif
    

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
        events_state[id] = state;
    }else{
        events_state.insert({id, state});
    }
    return 1;
};

void TimerScheduler::RunTask(time_point timepoint, TimerScheduler::Event event){
#ifdef DEBUG_MODE
        std::cerr << "RunTask:" << event.id << "\n";
#endif
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
#ifdef DEBUG_MODE
        std::cerr << "Event state:" << events_state[id] << "\n";
#endif
    if(events_state.count(id)==1){
        return events_state[id];
    }
    return -1;
};

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
#ifdef DEBUG_MODE
    std::cout << "top timer:" << nearest_event.id << "\n";
#endif
            uint64_t event_id = nearest_event.id;
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

#ifdef DEBUG_MODE
    std::cout << "overtimed task:" << nearest_event.id << "\n";
    std::cout << std::setw(9) << "now:" << std::chrono::time_point_cast<nanoseconds>(now).time_since_epoch().count() - TIME_BASE
        << std::setw(9) << "\novertime:" 
        << std::chrono::time_point_cast<nanoseconds>(nearest_timepoint).time_since_epoch().count() - TIME_BASE << "\n";
#endif
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


#ifdef DEBUG_MODE
    std::cout << std::endl;
#endif
        }
    }
    
}