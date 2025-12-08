#include "file_facilities.hpp"

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
            
            // Convert from 1-based to 0-based indexing
            graph.from.push_back(from - 1);
            graph.to.push_back(to - 1);
            graph.weights.push_back(weight);
        }
        else if (!header_read) {
            // format: first line is "num_nodes num_edges"
            std::istringstream first_line(line);
            first_line >> graph.num_nodes >> expected_edges;
            
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