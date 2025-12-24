#ifndef __GRAPH_HPP__
#define __GRAPH_HPP__

#include <vector>

namespace CHGraph
{

    struct Graph
    {
        std::vector<int> first_out;
        std::vector<int> to;
        std::vector<double> weights;

        int num_nodes = 0;
    };

    struct CHArc {
        int from; //Possibly not needed
        int to; 
        double weight;
        int mid_node;
    };

    struct PreprocGraph
    {   
        int num_nodes = 0;

        std::vector<int> ranks; // ranks[node] = contraction order (0 = lowest)


        // -------- Forward graph (upward edges) --------
        // contains edges u -> v where ranks[u] < ranks[v]
        std::vector<int> forward_first_out;  
        std::vector<CHArc>   forward_arcs;

        // -------- Backward graph (Reverse of Downward Graph) --------
        // contains edges v -> u for each edge (original or shortcut) u -> v with ranks[u] > ranks[v]"
        std::vector<int> backward_first_out;
        std::vector<CHArc>   backward_arcs;
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

    // Helper functions query
    bool stall_forward(int v, const std::vector<double>& dist_f, const PreprocGraph& preproc_graph);
    bool stall_backward(int v, const std::vector<double>& dist_b, const PreprocGraph& preproc_graph);
   
    void query_route(const CHGraph::Graph &graph, const PreprocGraph &preproc_graph, const Destination &destination, Route &route);
}

#endif 
