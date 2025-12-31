#include "experiment.hpp"
#include "file_facilities.hpp"
#include "measurement.hpp"
#include "ch_graph.hpp"
#include "timer.hpp"
#include <format>
#include <vector>

#define MEASURE_TIME(func, stopwatch) \
    {                                 \
        stopwatch.start();            \
        func;                         \
        stopwatch.stop();             \
    };

void Experiment::run(const std::string &graph_file, const std::string &destinations_file,
                     const std::string &output_file, const int run_number)
{
    CHGraph::Graph graph;
    std::vector<CHGraph::Destination> destinations;

    FileFacilities::read_graph(graph_file, graph);
    FileFacilities::read_destinations(destinations_file, destinations);

    CHGraph::PreprocGraph bottom_up_graph, top_down_graph;
    Measurement measurement;
    Timer timer;

    for (int ind = 0; ind < run_number; ++ind)
    {
        CHGraph::PreprocGraph preproc_graph;
        MEASURE_TIME(CHGraph::preproc_graph_bottom_up(graph, preproc_graph), timer);
        measurement.data["preproc_graph_bottom_up"].push_back(timer.get_result());

        if (ind + 1 == run_number)
        {
            bottom_up_graph = preproc_graph;
        }
    }

    for (int dest_ind = 0; dest_ind < destinations.size(); ++dest_ind)
    {
        for (int ind = 0; ind < run_number; ++ind)
        {
            CHGraph::Route route;
            MEASURE_TIME(CHGraph::query_route(graph, bottom_up_graph, destinations[dest_ind], route), timer);
            measurement.data["query_route_bottom_up_" + std::format("{:03}", dest_ind)].push_back(timer.get_result());
        }
    }

    for (int ind = 0; ind < run_number; ++ind)
    {
        CHGraph::PreprocGraph preproc_graph;
        MEASURE_TIME(CHGraph::preproc_graph_top_down(graph, preproc_graph), timer);
        measurement.data["preproc_graph_top_down"].push_back(timer.get_result());

        if (ind + 1 == run_number)
        {
            top_down_graph = preproc_graph;
        }
    }

    for (int dest_ind = 0; dest_ind < destinations.size(); ++dest_ind)
    {
        for (int ind = 0; ind < run_number; ++ind)
        {
            CHGraph::Route route;
            MEASURE_TIME(CHGraph::query_route(graph, top_down_graph, destinations[dest_ind], route), timer);
            measurement.data["query_route_top_down_" + std::format("{:03}", dest_ind)].push_back(timer.get_result());
        }
    }

    FileFacilities::dump_measurement(measurement, output_file);
}