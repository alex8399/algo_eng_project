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
    std::ifstream file(graph_file);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open graph file: " + graph_file);
    }

    std::string line;
    bool header_read = false;
    int expected_edges = 0;
    
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
        std::istringstream iss(line);
        char first_char;
        iss >> first_char;
        
        if (first_char == 'c') {
            // Comment line, skip
            continue;
        }
        else if (first_char == 'p') {
            //format: p sp num_nodes num_edges
            std::string sp;
            iss >> sp >> graph.num_nodes >> expected_edges;
            if (iss.fail() || graph.num_nodes <= 0 || expected_edges < 0) {
                throw std::runtime_error("Malformed problem line in graph file");
            }
            
            // Pre-allocate memory for efficiency
            graph.from.reserve(expected_edges);
            graph.to.reserve(expected_edges);
            graph.weights.reserve(expected_edges);
            
            header_read = true;
        }
        else if (first_char == 'a') {
            //arc: from to weight
            int from, to;
            double weight;
            iss >> from >> to >> weight;
            if (iss.fail()) {
                throw std::runtime_error("Malformed arc line in graph file");
            }
            
            // Convert from 1-based to 0-based indexing
            int from_0 = from - 1;
            int to_0 = to - 1;
            if (from_0 < 0 || from_0 >= graph.num_nodes || to_0 < 0 || to_0 >= graph.num_nodes) {
                throw std::runtime_error("Invalid node indices in arc");
            }
            
            graph.from.push_back(from_0);
            graph.to.push_back(to_0);
            graph.weights.push_back(weight);
        }
        else if (!header_read) {
            // format: first line is "num_nodes num_edges"
            std::istringstream first_line(line);
            first_line >> graph.num_nodes >> expected_edges;
            if (first_line.fail() || graph.num_nodes <= 0 || expected_edges < 0) {
                throw std::runtime_error("Malformed header line in graph file");
            }
            
            // Pre-allocate memory
            graph.from.reserve(expected_edges);
            graph.to.reserve(expected_edges);
            graph.weights.reserve(expected_edges);
            
            header_read = true;
            
            // Read edges (0-based indexing)
            while (std::getline(file, line)) {
                if (line.empty()) continue;
                
                std::istringstream edge_iss(line);
                int from, to;
                double weight;
                edge_iss >> from >> to >> weight;
                if (edge_iss.fail()) {
                    throw std::runtime_error("Malformed edge line in graph file");
                }
                
                if (from < 0 || from >= graph.num_nodes || to < 0 || to >= graph.num_nodes) {
                    throw std::runtime_error("Invalid node indices in edge");
                }
                
                graph.from.push_back(from);
                graph.to.push_back(to);
                graph.weights.push_back(weight);
            }
            break;
        }
    }
    
    file.close();

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