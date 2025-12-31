#ifndef __TIMER_HPP__
#define __TIMER_HPP__

#include <chrono>

using TimerTime = long long;
using ChronoTime = std::chrono::milliseconds;

class Timer
{
private:
    enum class State
    {
        INITIAL,
        RUNNING,
        FINISHED
    };

    Timer::State m_state = Timer::State::INITIAL;
    
    std::chrono::time_point<std::chrono::high_resolution_clock> m_start;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_end;
public:
    Timer() {};
    void start();
    void stop();
    TimerTime get_result() const;
};

#endif