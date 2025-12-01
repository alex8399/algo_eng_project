#include "experiment.hpp"
#include <stdexcept>

int main(int argc, char *argv[])
{
    if (argc != 5)
    {
        throw std::invalid_argument(
            "Program should be invoked in the following way: ./experiment.exe graph_file destinations_file output_file run_number");
    }

    Experiment::run(argv[1], argv[2], argv[3], std::stoi(argv[4]));
    return 0;
}