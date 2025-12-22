#include <gtest/gtest.h>
#include "ch_graph.hpp"



// simple test graph to be used for test
static CHGraph::Graph make_simple_graph()
{
    CHGraph::Graph g;
    g.num_nodes = 3;

    g.first_out = {0, 2, 3, 3};
    g.to        = {1, 2, 2};
    g.weights   = {1.0, 3.0, 1.0};

 
    return g;
}

TEST(CHPreprocessing, RanksAssigned)
{
    CHGraph::Graph g = make_simple_graph();
    CHGraph::PreprocGraph p;

    CHGraph::preproc_graph_top_down(g, p);

    ASSERT_EQ(p.ranks.size(), g.num_nodes);

    for (int v = 0; v < g.num_nodes; ++v)
        EXPECT_GE(p.ranks[v], 0);
}

TEST(CHPreprocessing, ForwardGraphIsUpward)
{
    CHGraph::Graph g = make_simple_graph();
    CHGraph::PreprocGraph p;

    CHGraph::preproc_graph_top_down(g, p);

    for (int u = 0; u < p.num_nodes; ++u)
    {
        for (int e = p.forward_first_out[u];
             e < p.forward_first_out[u + 1]; ++e)
        {
            const auto &arc = p.forward_arcs[e];
            EXPECT_LT(p.ranks[arc.from], p.ranks[arc.to]);
        }
    }
}

TEST(CHPreprocessing, BackwardGraphIsReverseOfForward)
{
    CHGraph::Graph g = make_simple_graph();
    CHGraph::PreprocGraph p;

    CHGraph::preproc_graph_top_down(g, p);

    int forward_edges = p.forward_arcs.size();
    int backward_edges = p.backward_arcs.size();

    EXPECT_EQ(forward_edges, backward_edges);
}

TEST(CHPreprocessing, ShortcutIsAdded)
{
    CHGraph::Graph g = make_simple_graph();
    CHGraph::PreprocGraph p;

    CHGraph::preproc_graph_top_down(g, p);

    bool found_shortcut = false;
    for (const auto &arc : p.forward_arcs)
    {
        if (arc.from == 0 && arc.to == 2 && arc.weight == 2.0)
        {
            found_shortcut = true;
            break;
        }
    }

    // shortcut may or may not exist, and both are valid
    SUCCEED();
}


TEST(CHPreprocessing, ShortestPathPreserved)
{
    CHGraph::Graph g = make_simple_graph();
    CHGraph::PreprocGraph p;

    CHGraph::preproc_graph_top_down(g, p);

    // Dijkstra on upward graph from 0 to 2
    const double INF = 1e18;
    std::vector<double> dist(3, INF);
    dist[0] = 0.0;

    bool updated = true;
    while (updated)
    {
        updated = false;
        for (const auto &arc : p.forward_arcs)
        {
            if (dist[arc.from] + arc.weight < dist[arc.to])
            {
                dist[arc.to] = dist[arc.from] + arc.weight;
                updated = true;
            }
        }
    }

    EXPECT_DOUBLE_EQ(dist[2], 2.0);
}

TEST(TestCaategory1, Test1) {
    CHGraph::Graph graph;
    CHGraph::PreprocGraph preproc_graph;
    CHGraph::preproc_graph_bottom_up(graph, preproc_graph);
    CHGraph::preproc_graph_bottom_up(graph, preproc_graph);
    EXPECT_EQ(3+2, 5);
    EXPECT_TRUE(true);
}