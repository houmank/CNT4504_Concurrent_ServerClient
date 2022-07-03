#ifndef TIMER_HPP
#define TIMER_HPP

#include <chrono>

/**
 * The Timer class is a simple header only class used to measure the performance and turn-around time
 * of the socket code in the server. There are two important functions and a public getter.
 * StartTimer and StopTimer respectively start and stop the timer.
 * GetTime is the getter to access the clocked in duration of the test.
 */
class Timer 
{
public:

    // Default constructor and destructor
    Timer() = default;
    ~Timer() = default;

    /**
     * StartTimer grabs a high resolution time point when called, it does not
     * accept any arguments and returns nothing.
     * 
     * @param void
     * @return void
     */
    void StartTimer();

    /**
     * StopTimer grabs a second HRTP and calculates the duration of the timer and then
     * proceeds to store it in the duration member variable. The function accepts no 
     * arguments and returns nothing.
     * 
     * @param void
     * @return void
     */
    void StopTimer();

    /**
     * GetDurationMilli is a function that returns the calculated the duration of the timer.
     * The function will return std::chrono::milliseconds duration object and will accept no arguments.
     * 
     * @param void
     * @return std::chrono::milliseconds
     */
    std::chrono::milliseconds GetDurationMilli();

    /**
     * GetDurationMicro is a function that returns the calculated the duration of the timer.
     * The function will return std::chrono::microseconds duration object and will accept no arguments.
     * 
     * @param void
     * @return std::chrono::microseconds
     */
    std::chrono::microseconds GetDurationMicro();
    

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTime;
    std::chrono::microseconds m_DurationMicro;
    std::chrono::milliseconds m_DurationMilli;
};

#endif