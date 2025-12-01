#include "experiment.hpp"
#include "file_facilities.hpp"
#include "measurement.hpp"
#include "ch_graph.hpp"
#include "timer.hpp"
#include <vector>

#define MEASURE_TIME(func, measurements, stopwatch, times)             \
    {                                                                  \
        for (int counter = 0; counter < times; ++counter)              \
        {                                                              \
            stopwatch.start();                                         \
            func;                                                      \
            stopwatch.stop();                                          \
            measurements.push_back(stopwatch.get_result());            \
        }                                                              \
    }

void Experiment::run(const std::string &graph_file, const std::string &destinations_file,
                     const std::string &output_file, const int &run_number)
{
    CHGraph::Graph graph;
    std::vector<CHGraph::Destination> destinations;

    FileFacilities::read_graph(graph_file, graph);
    FileFacilities::read_destinations(destinations_file, destinations);

    CHGraph::PreprocGraph bottom_up_graph, top_down_graph;
    CHGraph::Route route;
    Measurement measurement;
    Timer timer;

    MEASURE_TIME(
        CHGraph::preproc_graph_bottom_up(graph, bottom_up_graph), 
        measurement.data["preproc_graph_bottom_up"],
        timer,
        run_number);
    
    for (int ind = 0; ind < destinations.size(); ++ind)
    {
        MEASURE_TIME(
            CHGraph::query_route(graph, bottom_up_graph, destinations[ind], route),
            measurement.data["query_route_bottom_up_" + std::to_string(ind)],
            timer,
            run_number);
    }

    MEASURE_TIME(
        CHGraph::preproc_graph_top_down(graph, top_down_graph), 
        measurement.data["preproc_graph_top_down"],
        timer,
        run_number);
    
    for (int ind = 0; ind < destinations.size(); ++ind)
    {
        MEASURE_TIME(
            CHGraph::query_route(graph, top_down_graph, destinations[ind], route),
            measurement.data["query_route_top_down_" + std::to_string(ind)],
            timer,
            run_number);
    }
    
    FileFacilities::dump_measurement(measurement, output_file);
}