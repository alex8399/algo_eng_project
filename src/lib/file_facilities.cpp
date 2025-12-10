#include <stdexcept>
#include "ch_graph.hpp"
#include "file_facilities.hpp"
#include <format>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_set>


constexpr char DESTINATION_SYMBOL = 'd';
constexpr char COMMENT_SYMBOL = 'c';


void FileFacilities::read_graph(const std::string &graph_file, CHGraph::Graph &graph)
{
}

void FileFacilities::read_destinations(const std::string &destinations_file, std::vector<CHGraph::Destination> &destinations)
{
    std::ifstream file(destinations_file);
    std::string line;

    if (!file.is_open())
    {
        throw std::runtime_error("Cannot open destinations file " + destinations_file);
    }
    
    while (std::getline(file, line)) {
        std::istringstream string_stream(line);
        char symbol;
        string_stream >> symbol;

        switch (symbol)
        {
            case COMMENT_SYMBOL:
            {
                break;
            }
            case DESTINATION_SYMBOL:
            {
                int node_from, node_to;
                string_stream >> node_from >> node_to;
                
                if (node_from < 0 || node_to < 0)
                {
                    throw std::runtime_error("Negative node number in destinations file " + destinations_file);
                }
                
                if (string_stream.fail())
                {
                    throw std::runtime_error("Incorrect line in destinations file " + destinations_file);
                }

                CHGraph::Destination destination{.source = node_from, .target = node_to};
                destinations.push_back(destination);
                break;
            }
            default:
            {
                throw std::runtime_error("Incorrect line in destinations file " + destinations_file);
            }
        }
    }

    file.close();
}

void FileFacilities::dump_measurement(const Measurement &measurement, const std::string &output_file)
{
}

void FileFacilities::write_graph(const CHGraph::Graph &graph, const std::string &graph_file)
{
    std::ofstream file(graph_file);

    if (!file.is_open())
    {
        throw std::runtime_error("Cannot open graph file " + graph_file);
    }

    std::unordered_set<int> nodes = {};
    nodes.insert(graph.from.begin(), graph.from.end());
    nodes.insert(graph.to.begin(), graph.to.end());

    file << std::format("p sp {} {}", nodes.size(), graph.to.size()) << std::endl;

    for (int ind = 0; ind < graph.from.size(); ++ind)
    {
        file << std::format("a {} {} {}", graph.from[ind], graph.to[ind], graph.weights[ind]);
    }
    
    file.close();
}