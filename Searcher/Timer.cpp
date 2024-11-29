#include "Timer.h"

using namespace std;
using namespace chrono;

void Profiler::RestartTimer() {
    start_time_point = steady_clock::now();
}

nanoseconds Profiler::StopTimer() {
    duration = steady_clock::now() - start_time_point;
    return duration;
}

Timer::Timer(long long sec) : seconds_duration(sec), is_expired(false) {
    SetTimerDuration(seconds_duration);
    start_time_point = steady_clock::now();
}

void Timer::SetTimerDuration(long long sec) {
    timer_duration = duration_cast<nanoseconds>(seconds(sec));
}

void Timer::UpdateTimer() {
    timer_duration -= (steady_clock::now() - start_time_point);
    start_time_point = steady_clock::now();

    if (timer_duration.count() <= 0) {
        is_expired = true;
    }
}

const bool* Timer::IsExpired() {
    return &is_expired;
}

void Timer::RestartTimer() {
    is_expired = false;
    start_time_point = steady_clock::now();
    SetTimerDuration(seconds_duration);
}
