#pragma once
#ifndef TIMERSCHEDULER_HPP
#define TIMERSCHEDULER_HPP

#include <cstdint>
#include <functional>

class TimerScheduler
{
public:
    void RegisterEvent(uint64_t id, uint64_t deadline, std::function<void()> callback_, uint64_t period = 0);
    void CancelEvent(uint64_t id);
};

#endif