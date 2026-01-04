#include "ch_graph.hpp"
#include <vector>
#include <limits>
#include <algorithm>

namespace CHGraph {

// Helper function for stall-on-demand on forward graph
bool stall_forward(int v, const std::vector<double>& dist_f, const CHGraph::PreprocGraph& preproc_graph) {
    // iterate incoming upward edges u -> v stored as backward_arcs at index v: v -> u
    for (int e = preproc_graph.backward_first_out[v]; e < preproc_graph.backward_first_out[v + 1]; ++e) {
        const CHArc &arc = preproc_graph.backward_arcs[e];
        int u = arc.to;              //lower-ranked neighbor u
        double w = arc.weight;       // weight(u,v)
        if (dist_f[u] < std::numeric_limits<double>::infinity() && dist_f[u] + w < dist_f[v]) {
            return true; // stall as better path to v exists via u
        }
    }
    return false;
}

// Helper function for stall-on-demand on backward graph
bool stall_backward(int v, const std::vector<double>& dist_b, const CHGraph::PreprocGraph& preproc_graph) {
    for (int e = preproc_graph.forward_first_out[v]; e < preproc_graph.forward_first_out[v + 1]; ++e) {
        const CHArc &arc = preproc_graph.forward_arcs[e];
        int u = arc.to;              // higher-ranked neighbor u
        double w = arc.weight;       // weight(v,u)
        if (dist_b[u] < std::numeric_limits<double>::infinity() && dist_b[u] + w < dist_b[v]) {
            return true; // stall on backward side
        }
    }
    return false;
}

} // namespace CHGraph