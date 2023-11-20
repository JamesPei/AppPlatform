#pragma once
#ifndef __TIMERSCHEDULER_HPP__
#define __TIMERSCHEDULER_HPP__

#include <cstdint>
#include <functional>
#include <memory>
#include <queue>
#include <vector>
#include <thread>
#include "Shortnames.hpp"

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


    void Start();
    void RegisterEvent(uint64_t id, uint64_t deadline, std::function<void()> callback_, uint64_t period = 0);
    void CancelEvent(uint64_t id);
    bool IsRunning();

private:
    TimerScheduler();
    ~TimerScheduler();
    TimerScheduler(const TimerScheduler& ts) = delete;
    TimerScheduler& operator=(const TimerScheduler& ts) = delete;

    void Schedule();

private:
    std::priority_queue<Event, std::vector<Event>, std::function<bool(Event, Event)>> events;
    std::vector<uint64_t> depreacted_events;
    std::thread scheduler_thread;
    bool stop_flag;
};

#endif