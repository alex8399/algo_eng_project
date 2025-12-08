#include <stdexcept>
#include "ch_graph.hpp"
#include "file_facilities.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>


constexpr char DESTINATION_SYMBOL = 'd';
constexpr char COMMENT_SYMBOL = 'c';


void FileFacilities::read_graph(const std::string &graph_file, CHGraph::Graph &graph)
{
}

void FileFacilities::read_destinations(const std::string &destinations_file, std::vector<CHGraph::Destination> &destinations)
{
    std::ifstream file(destinations_file);

    if (!file.is_open())
    {
        throw std::runtime_error("Cannot open destinations file " + destinations_file);
    }

    std::string line;
    
    while (std::getline(file, line)) {
        std::istringstream string_stream(line);
        char first_char;
        string_stream >> first_char;

        switch (first_char)
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
                    throw std::runtime_error("Incorrect node number in the destinations file " + destinations_file);
                }

                CHGraph::Destination destination;
                destination.source = node_from;
                destination.target = node_to;
                destinations.push_back(destination);

                break;
            }
            default:
            {
                throw std::runtime_error("Incorrect format of the line in the destinations file " + destinations_file);
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
}