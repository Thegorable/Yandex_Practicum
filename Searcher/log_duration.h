#pragma once
#include <iostream>
#include <chrono>
#include <string>

class LogDuration;

#define NAME_CONCAT_INTERNAL(X, Y) X ## Y
#define NAME_CONCAT(X, Y) NAME_CONCAT_INTERNAL(X, Y)
#define PROFINER_UNIQUE_NAME NAME_CONCAT(profiler_, __LINE__)
#define LOG_DURATION(x) LogDuration PROFINER_UNIQUE_NAME(x);

class LogDuration {
public:
    LogDuration(const std::string& str) : msg(str) {
        start_time_point = std::chrono::steady_clock::now();
    }

    ~LogDuration() {
        auto duration = std::chrono::steady_clock::now() - start_time_point;
        std::cerr << msg << ": " << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << " ms" << '\n';
    }
private:
    std::chrono::time_point<std::chrono::steady_clock, std::chrono::nanoseconds> start_time_point;
    std::string msg;
};