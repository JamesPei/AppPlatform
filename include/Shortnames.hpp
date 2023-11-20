#pragma once
#ifndef __SHORTNAMES_HPP__
#define __SHORTNAMES_HPP__

#define TIME_BASE 1700461337000000000

#include <chrono>
using seconds = std::chrono::seconds;
using milliseconds = std::chrono::milliseconds;
using microseconds = std::chrono::microseconds;
using nanoseconds = std::chrono::nanoseconds;
using system_clock = std::chrono::system_clock;
using time_point = std::chrono::time_point<system_clock, nanoseconds>;

#endif