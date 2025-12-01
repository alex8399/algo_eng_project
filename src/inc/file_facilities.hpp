#ifndef __FILE_FACILITIES_HPP__
#define __FILE_FACILITIES_HPP__

#include "measurement.hpp"
#include "ch_graph.hpp"
#include <string>
#include <vector>

namespace FileFacilities
{
    void read_graph(const std::string &graph_file, CHGraph::Graph &graph);

    void read_destinations(const std::string &destinations_file, std::vector<CHGraph::Destination> &destinations);

    void dump_measurement(const Measurement &measurement, const std::string &output_file);
}

#endif