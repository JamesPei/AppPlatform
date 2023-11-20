#pragma once

#ifndef __TIMER_HPP__
#define __TIMER_HPP__

#include <cstdint>
#include <functional>
#include <thread>
#include "Shortnames.hpp"
#include "TimerScheduler.hpp"

class Timer
{
public:

    enum class Type
    {
        ONESHOT = 1U, // single shot timer
        PERIODIC = 2U, // periodic timer(typically async)
    };
    
    /**
     * Constructor
     * type: timer type
     * duration: timer duration in nanoseconds
     * start_time: start timepoint. 0 means start from current time
     */
    Timer(Type type, uint64_t duration, uint64_t start_time = 0);
    
    uint64_t GetId();
    
    /**
     * Oneshot timer: block and wait for timer to expire.
     * Periodic timer: block and wait for next round time point expire.
     *
     * return: 
     * - SUCCESS: Timer is running and not expired.
     * - EXPIRED: Timer has expired when calling this function
     * - ERROR: Timer is not running
     */
    int Wait();
    
    /**
     * Asynchronous wait
     * Oneshot timer: call F(Args...) when timer expire.
     * Periodic timer: call F(Args...) each round expire.
     */
    template <typename F, typename... Args>
    void WaitAsync(F&& fun, Args&&... args){
        if(!scheduler.IsRunning()){
            scheduler.Start();
        }

        if(type_ == Type::ONESHOT){
            scheduler.RegisterEvent(id_, deadline_timepoint_, fun, 0);
        }else{
            scheduler.RegisterEvent(id_, deadline_timepoint_, fun, duration_);
        }
    };
    
    /**
     * Oneshot timer: Check if timer has expired.
     * Periodic timer: Check if current round has expired(Synchonous mode only).
     */
    bool IsExpired(uint64_t round=0);
    
    /**
     * Check if timer is running
     */
    bool IsRunning();
    
    /**
     * GetCurrentTimerRound(Periodic mode only)
     * For oneshot timer, always return 0
     */
    uint64_t GetTimerRound();
    
    /**
     * Cancel this timer
     * For asynchronoous mode, Remove registered event.
     */
    void Cancel();

    uint64_t GetDeadline() const;

    bool operator<=(const Timer& timer);

public:
    static Timer CreatePeriodicTimer(uint64_t duration, uint64_t start_time = 0);
    static Timer CreateOneshotTimer(uint64_t duration, uint64_t start_time = 0);

private:
    static uint64_t generateTimerId();

private:
    static uint64_t last_id;

private:
    Type type_;
    uint64_t id_;
    uint64_t start_timepoint_;
    uint64_t duration_;
    uint64_t round_;
    uint64_t deadline_timepoint_;
    std::function<void()> callback_;
    TimerScheduler& scheduler;
};

#endif