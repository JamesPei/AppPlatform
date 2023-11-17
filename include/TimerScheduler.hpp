#pragma once
#ifndef __TIMERSCHEDULER_HPP__
#define __TIMERSCHEDULER_HPP__

#include <cstdint>
#include <functional>
#include <memory>
#include <queue>
#include <vector>

class TimerScheduler
{
public:
    static TimerScheduler& Instance(){
        static TimerScheduler timerscheduler;
        static TimerScheduler& instance = timerscheduler;
        return instance;
    };

    struct Event{
        uint64_t id;
        uint64_t deadline; 
        std::function<void()> callback_;
        uint64_t period;
    };

    void RegisterEvent(uint64_t id, uint64_t deadline, std::function<void()> callback_, uint64_t period = 0);
    void CancelEvent(uint64_t id);

private:
    TimerScheduler();
    TimerScheduler(const TimerScheduler& ts) = delete;
    TimerScheduler& operator=(const TimerScheduler& ts) = delete;

    void RunTask();

private:
    std::priority_queue<Event, std::vector<Event>, std::function<bool(Event, Event)>> events;
    std::vector<uint64_t> depreacted_events;
    
};

#endif