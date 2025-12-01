#ifndef __TIMER_HPP__
#define __TIMER_HPP__

class Timer
{
public:
    void start();
    void stop();
    float get_result() const;
};

#endif