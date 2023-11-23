#pragma once
#ifndef __TIMERSCHEDULER_HPP__
#define __TIMERSCHEDULER_HPP__

#include <cstdint>
#include <functional>
#include <memory>
#include <queue>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <map>
#include "Shortnames.hpp"

class TimerScheduler
{
public:
    static TimerScheduler& Instance(){
        static TimerScheduler timerscheduler;
        return timerscheduler;
    };

    struct Event{
        uint64_t id;
        uint64_t deadline; 
        std::function<void()> callback_;
        uint64_t period;
    };

    struct EventState{
        uint64_t id;
        uint64_t round;
        int state;
    };

    void RunTask(time_point timepoint, TimerScheduler::Event event);
    void Start();
    void RegisterEvent(uint64_t id, uint64_t deadline, std::function<void()> callback_, uint64_t period = 0);
    void CancelEvent(uint64_t id);
    bool IsRunning();

    /**
     * state:
     *  1: waiting
     *  2: running
     *  3: ending
    */
    int SetEventState(uint64_t id, int state);
    int GetEventState(uint64_t id);
    int GetEventRound(uint64_t id);

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
    std::atomic_bool stop_flag;
    std::mutex mtx;
    std::map<uint64_t, EventState> events_state;
};

#endif