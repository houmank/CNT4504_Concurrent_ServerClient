#include "timer.hpp"

void Timer::StartTimer(){
    // record start time
    m_StartTime = std::chrono::high_resolution_clock::now();
}

void Timer::StopTimer(){
    // take the time we stopped
    auto endTime = std::chrono::high_resolution_clock::now();

    // calculate the time since epoch in ms
    auto start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTime).time_since_epoch();
    auto end = std::chrono::time_point_cast<std::chrono::microseconds>(endTime).time_since_epoch();

    // difference is the duration of the timer
    m_DurationMicro = end - start;
    m_DurationMilli = std::chrono::duration_cast<std::chrono::milliseconds>(m_DurationMicro);

}

std::chrono::milliseconds Timer::GetDurationMilli(){ return m_DurationMilli; }
std::chrono::microseconds Timer::GetDurationMicro(){ return m_DurationMicro; }