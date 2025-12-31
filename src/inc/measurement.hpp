#ifndef __MEASUREMENT_HPP__
#define __MEASUREMENT_HPP__

#include <unordered_map>
#include <vector>
#include <string>
#include "timer.hpp"

struct Measurement
{
    std::unordered_map<std::string, std::vector<TimerTime>> data;
};

#endif