#ifndef __GRAPH_HPP__
#define __GRAPH_HPP__

namespace CHGraph
{

    struct Graph
    {
    };

    struct PreprocGraph
    {
    };

    struct Route
    {
    };

    struct Destination
    {
    };

    void preproc_graph_bottom_up(const Graph &graph, PreprocGraph &preproc_graph);

    void preproc_graph_top_down(const Graph &graph, PreprocGraph &preproc_graph);

    void calculate_route(const Graph &graph, const PreprocGraph &preproc_graph,
                         const Destination &destination, Route &route);
}

#endif 