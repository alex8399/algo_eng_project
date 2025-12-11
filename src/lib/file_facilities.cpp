#include "file_facilities.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>


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
}

void FileFacilities::dump_measurement(const Measurement &measurement, const std::string &output_file)
{
}

void FileFacilities::write_graph(const CHGraph::Graph &graph, const std::string &graph_file)
{
}