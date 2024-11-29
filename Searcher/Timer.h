#pragma once
#include <chrono>
#include <iostream>

class Profiler {
public:
    Profiler() = default;

    virtual void RestartTimer();
    std::chrono::nanoseconds StopTimer();
    template <class TimeType>
    void PrintResults();

protected:
    std::chrono::time_point<std::chrono::steady_clock, std::chrono::nanoseconds> start_time_point;
    std::chrono::nanoseconds duration;
};

template<class TimeType>
inline void Profiler::PrintResults() {
    std::cerr << duration.count() << " ns" << '\n';
}

template <>
inline void Profiler::PrintResults<std::chrono::milliseconds>() {
    using namespace std::chrono; {
        std::cerr << duration_cast<milliseconds>(nanoseconds(duration)).count() << " ms" << '\n';
    }
}

class Timer : public Profiler {
public:
    Timer() = default;
    Timer(long long sec);

    void SetTimerDuration(long long sec);
    void UpdateTimer();
    const bool* IsExpired();
    void RestartTimer() override;

private:
    long long seconds_duration = 0;
    std::chrono::nanoseconds timer_duration;
    bool is_expired = false;
};
