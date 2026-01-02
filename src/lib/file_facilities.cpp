#include <algorithm>
#include <stdexcept>
#include "ch_graph.hpp"
#include "file_facilities.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <utility>


constexpr char DESTINATION_SYMBOL = 'd';
constexpr char COMMENT_SYMBOL = 'c';
constexpr char GRAPH_SUMMARY_SYMBOL = 'p';
constexpr char EDGE_SYMBOL = 'a';


constexpr std::string CSV_NEW_LINE_SYMBOL = "\n";
constexpr std::string CSV_NEW_COLUMN_SYMBOL = ";";


void FileFacilities::read_graph(const std::string &graph_file, CHGraph::Graph &graph)
{
    std::ifstream file(graph_file);

    if (!file.is_open())
    {
        throw std::runtime_error("Can not open destinations file " + graph_file);
    }

    std::string line;
    int line_number = 1;
    bool summary_read = false;
    int node_number, edge_number;

    while (std::getline(file, line))
    {
        std::istringstream string_stream(line);
        char symbol;
        string_stream >> symbol;

        switch (symbol)
        {
            case COMMENT_SYMBOL:
            {
                break;
            }
            case GRAPH_SUMMARY_SYMBOL:
            {
                std::string sp_symbols;
                string_stream >> sp_symbols >> node_number >> edge_number;
                
                if (string_stream.fail() || sp_symbols != "sp" || node_number < 0 || edge_number < 0)
                {
                    throw std::runtime_error("Incorrect summary line on line " + std::to_string(line_number) + " in graph file " + graph_file);
                }

                summary_read = true;
                break;
            }
            default:
            {
                throw std::runtime_error("Incorrect symbol on line " + std::to_string(line_number) + " in graph file " + graph_file);
            }
        }

        ++line_number;

        if (summary_read)
        {
            break;
        }
    }

    if(!summary_read)
    {
        throw std::runtime_error("No summary line in graph file " + graph_file);
    }

    int actual_edge_number = 0;
    std::unordered_map<int, std::vector<std::pair<int, double>>> edge_map;

    while (std::getline(file, line))
    {
        std::istringstream string_stream(line);
        char symbol;
        string_stream >> symbol;

        switch (symbol)
        {
            case COMMENT_SYMBOL:
            {
                break;
            }
            case EDGE_SYMBOL:
            {
                int node_from, node_to;
                double weight;
                string_stream >> node_from >> node_to >> weight;
                
                if (string_stream.fail() || (node_from <= 0 || node_number < node_from) || (node_to <= 0 || node_number < node_to) || weight < 0)
                {
                    throw std::runtime_error("Incorrect edge format on line " + std::to_string(line_number) + " in graph file " + graph_file);
                }

                node_from -= 1;
                node_to -= 1;

                edge_map[node_from].emplace_back(node_to, weight);
                ++actual_edge_number;
                break;
            }
            default:
            {
                throw std::runtime_error("Incorrect symbol on line " + std::to_string(line_number) + " in graph file " + graph_file);
            }
        }

        ++line_number;
    }
    
    if (actual_edge_number != edge_number)
    {
        throw std::runtime_error("Actual number of edges and number of edges from summary line are not equal in graph file " + graph_file);
    }

    graph.first_out.reserve(node_number);
    graph.from.reserve(edge_number);
    graph.to.reserve(edge_number);
    graph.weights.reserve(edge_number);

    for (int node_from = 0; node_from < node_number; ++node_from)
    {
        if (edge_map.find(node_from) == edge_map.end())
        {
            graph.first_out.push_back(edge_number + 1);
        }
        else
        {
            graph.first_out.push_back(graph.from.size());
            
            for (auto &edge_pair: edge_map[node_from])
            {
                graph.from.push_back(node_from);
                graph.to.push_back(edge_pair.first);
                graph.weights.push_back(edge_pair.second);
            }
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
        throw std::runtime_error("Can not open destinations file " + destinations_file);
    }

    int line_number = 1;
    
    while (std::getline(file, line))
    {
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
                
                if (string_stream.fail() || node_from < 0 || node_to < 0)
                {
                    throw std::runtime_error("Incorrect destination format on line " + std::to_string(line_number) + " in destinations file " + destinations_file);
                }

                CHGraph::Destination destination{.source = node_from, .target = node_to};
                destinations.push_back(destination);
                break;
            }
            default:
            {
                throw std::runtime_error("Incorrect format on line " + std::to_string(line_number) + " in destinations file " + destinations_file);
            }
        }

        ++line_number;
    }

    file.close();
}

void FileFacilities::dump_measurement(const Measurement &measurement, const std::string &output_file)
{
    std::ofstream file(output_file);
    
    if (!file.is_open())
    {
        throw std::runtime_error("Can not create output file " + output_file);
    }

    std::vector<std::string> keys;
    for (const auto& column: measurement.data)
    {
        keys.push_back(column.first);
    }

    std::sort(keys.begin(), keys.end());

    int max_size = 0;

    for (const std::string &key: keys)
    {
        file << key << CSV_NEW_COLUMN_SYMBOL;

        if (measurement.data.at(key).size() > max_size)
        {
            max_size = measurement.data.at(key).size();
        }
    }

    file << CSV_NEW_LINE_SYMBOL;

    for (int ind = 0; ind < max_size; ++ind)
    {
        for (const std::string &key: keys)
        {
            if (ind < measurement.data.at(key).size())
            {
                file << measurement.data.at(key)[ind];
            }

            file << CSV_NEW_COLUMN_SYMBOL;
        }

        file << CSV_NEW_LINE_SYMBOL;
    }

    file.close();
}