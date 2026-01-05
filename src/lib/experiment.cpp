#include "experiment.hpp"
#include "file_facilities.hpp"
#include "measurement.hpp"
#include "ch_graph.hpp"
#include "timer.hpp"
#include <vector>
#include <string>
#include <iostream>


#define MEASURE_TIME(func, stopwatch) \
    {                                 \
        stopwatch.start();            \
        func;                         \
        stopwatch.stop();             \
    };


constexpr int DEFAULT_FORMAT_STRING_SIZE = 2;


static std::string format_numb(const int numb, const int string_size);
static void log(const std::string &message);


static std::string format_numb(const int numb, const int string_size)
{
    const std::string str_numb = std::to_string(numb);
    const int zero_fill_size = (string_size > str_numb.size()) ? (string_size - str_numb.size()) : 0;
    return std::string(zero_fill_size, '0')  + str_numb;
}

static void log(const std::string &message)
{
    std::cout << "LOG: " << message << std::endl;
}

void Experiment::run(const std::string &graph_file, const std::string &destinations_file,
                     const std::string &output_file, const int run_number)
{
    CHGraph::Graph graph;
    std::vector<CHGraph::Destination> destinations;

    log("Experiment started.");

    log("Graph file reading started.");
    FileFacilities::read_graph(graph_file, graph);
    log("Graph file reading finished.");

    log("Destinations file reading started.");
    FileFacilities::read_destinations(destinations_file, destinations);
    log("Destinations file reading finished.");

    CHGraph::PreprocGraph bottom_up_graph, top_down_graph;
    Measurement measurement;
    Timer timer;

    log("Preproccessing graph by bottom up approach started.");
    for (int ind = 0; ind < run_number; ++ind)
    {
        CHGraph::PreprocGraph preproc_graph;
        MEASURE_TIME(CHGraph::preproc_graph_bottom_up(graph, preproc_graph), timer);
        measurement.data["preproc_graph_bottom_up"].push_back(timer.get_result());

        if (ind == run_number - 1)
        {
            bottom_up_graph = preproc_graph;
            log("Bottom up preprocced graph saved.");
        }
    }
    log("Preproccessing graph by bottom up approach finished.");

    for (int dest_ind = 0; dest_ind < destinations.size(); ++dest_ind)
    {
        log("Quering route " + std::to_string(dest_ind) + " in bottom up preprocced graph started.");
        const std::string dest_str_numb = format_numb(dest_ind, DEFAULT_FORMAT_STRING_SIZE);

        for (int ind = 0; ind < run_number; ++ind)
        {
            CHGraph::Route route;
            MEASURE_TIME(CHGraph::query_route(graph, bottom_up_graph, destinations[dest_ind], route), timer);
            measurement.data["query_route_bottom_up_" + dest_str_numb].push_back(timer.get_result());
        }
        log("Quering route " + std::to_string(dest_ind) + " in bottom up preprocced graph finished.");
    }

    log("Preproccessing graph by top down approach started.");
    for (int ind = 0; ind < run_number; ++ind)
    {
        CHGraph::PreprocGraph preproc_graph;
        MEASURE_TIME(CHGraph::preproc_graph_top_down(graph, preproc_graph), timer);
        measurement.data["preproc_graph_top_down"].push_back(timer.get_result());

        if (ind == run_number - 1)
        {
            top_down_graph = preproc_graph;
            log("Top down preprocced graph saved.");
        }
    }
    log("Preproccessing graph by top down approach finished.");

    for (int dest_ind = 0; dest_ind < destinations.size(); ++dest_ind)
    {
        log("Quering route " + std::to_string(dest_ind) + " in top down preprocced graph started.");
        const std::string dest_str_numb = format_numb(dest_ind, DEFAULT_FORMAT_STRING_SIZE);

        for (int ind = 0; ind < run_number; ++ind)
        {
            CHGraph::Route route;
            MEASURE_TIME(CHGraph::query_route(graph, top_down_graph, destinations[dest_ind], route), timer);
            measurement.data["query_route_top_down_" + dest_str_numb].push_back(timer.get_result());
        }
        log("Quering route " + std::to_string(dest_ind) + " in top down preprocced graph finished.");
    }

    log("Saving measurements started.");
    FileFacilities::dump_measurement(measurement, output_file);
    log("Saving measurements finished.");
    
    log("Experiment finished.");
}