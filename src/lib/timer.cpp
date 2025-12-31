#include "timer.hpp"

#include <chrono>
#include <stdexcept>

void Timer::start()
{
    if (!(m_state == Timer::State::FINISHED || m_state == Timer::State::INITIAL))
    {
        throw std::runtime_error("Can start timer only in initial or finished state");
    }

    m_state = Timer::State::RUNNING;
    m_start = std::chrono::high_resolution_clock::now();
}

void Timer::stop()
{
    // Save time before checking status in order to collect more accurate measurement
    m_end = std::chrono::high_resolution_clock::now();

    if (!(m_state == Timer::State::RUNNING))
    {
        throw std::runtime_error("Can stop timer only in running state");
    }

    m_state = Timer::State::FINISHED;
}

TimerTime Timer::get_result() const
{
    if (!(m_state == Timer::State::FINISHED))
    {
        throw std::runtime_error("Can collect timer result only in finished state");
    }

    return std::chrono::duration_cast<ChronoTime>(m_end - m_start).count();
}