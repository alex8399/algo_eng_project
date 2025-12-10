#ifndef __EXPERIMENT_HPP__
#define __EXPERIMENT_HPP__

#include <string>

namespace Experiment
{
    void run(const std::string &graph_file, const std::string &destinations_file,
             const std::string &output_file, const int run_number);
}

#endif