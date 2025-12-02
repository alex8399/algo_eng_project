#ifndef __GRAPH_HPP__
#define __GRAPH_HPP__

#include <vector>

namespace CHGraph
{

    struct Graph
    {
        std::vector<int> from;
        std::vector<int> to;
        std::vector<double> weights;

        int num_nodes = 0;
    };

    struct PreprocGraph
    {
        std::vector<int> ranks;
        
        struct Arc {
            int from;
            int to;
            double weight;
            int mid_node;
        };
        std::vector<Arc> arcs;
    };

    struct Route
    {
        double total_weight = 0;
        std::vector<int> nodes;
    };

    struct Destination
    {
        int source = -1;
        int target = -1;
    };

    void preproc_graph_bottom_up(const Graph &graph, PreprocGraph &preproc_graph);

    void preproc_graph_top_down(const Graph &graph, PreprocGraph &preproc_graph);

    void query_route(const Graph &graph, const PreprocGraph &preproc_graph,
                     const Destination &destination, Route &route);
}

#endif 