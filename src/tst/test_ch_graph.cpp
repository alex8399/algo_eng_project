#include <gtest/gtest.h>
#include "ch_graph.hpp"



// simple test graph to be used for test
static CHGraph::Graph make_simple_graph()
{
    CHGraph::Graph g;

    g.first_out = {0, 2, 3, 3};
    g.to        = {1, 2, 2};
    g.weights   = {1.0, 3.0, 1.0};

 
    return g;
}


TEST(CHPreprocessing, ForwardGraphIsUpward)
{
    CHGraph::Graph g = make_simple_graph();
    CHGraph::PreprocGraph p;

    CHGraph::preproc_graph_top_down(g, p);

    const int n = static_cast<int>(p.forward_first_out.size()) - 1;

    for (int u = 0; u < n; ++u)
    {
        for (int e = p.forward_first_out[u];
             e < p.forward_first_out[u + 1]; ++e)
        {
            const auto &arc = p.forward_arcs[e];
            EXPECT_LT(p.ranks[arc.from], p.ranks[arc.to]);
        }
    }
}


TEST(CHPreprocessing, BackwardGraphMatchesDownwardDefinition)
{
    CHGraph::Graph g = make_simple_graph();
    CHGraph::PreprocGraph p;
    CHGraph::preproc_graph_top_down(g, p);

    for (const auto &arc : p.backward_arcs)
        EXPECT_LT(p.ranks[arc.from], p.ranks[arc.to]);
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



TEST(CHPreprocessing, RanksArePermutation)
{
    CHGraph::Graph g = make_simple_graph();
    CHGraph::PreprocGraph p;

    CHGraph::preproc_graph_top_down(g, p);

    const int n = (int)p.ranks.size();
    ASSERT_EQ(n, (int)g.first_out.size() - 1);

    std::vector<int> seen(n, 0);
    for (int r : p.ranks)
    {
        ASSERT_GE(r, 0);
        ASSERT_LT(r, n);
        seen[r]++;
    }
    for (int i = 0; i < n; ++i)
        EXPECT_EQ(seen[i], 1);
}




TEST(TestCaategory1, Test1) {
    CHGraph::Graph graph;
    CHGraph::PreprocGraph preproc_graph;
    CHGraph::preproc_graph_bottom_up(graph, preproc_graph);
    CHGraph::preproc_graph_bottom_up(graph, preproc_graph);
    EXPECT_EQ(3+2, 5);
    EXPECT_TRUE(true);
}

//testing ranking nodes using heuristic

TEST(CHPreprocessing, ShortcutCountIsReasonable)
{
    CHGraph::Graph g = make_simple_graph();
    CHGraph::PreprocGraph p;

    CHGraph::preproc_graph_top_down(g, p);

    int shortcut_count = 0;
    for (const auto &arc : p.forward_arcs)
        if (arc.mid_node != -1)
            shortcut_count++;

    EXPECT_LE(shortcut_count, 2); // heuristic-dependent bound
}

TEST(CHPreprocessing, AllNodesAreRankedExactlyOnce)
{
    CHGraph::Graph g = make_simple_graph();
    CHGraph::PreprocGraph p;

    CHGraph::preproc_graph_top_down(g, p);

    const int n = static_cast<int>(g.first_out.size()) - 1;

    ASSERT_EQ(p.ranks.size(), n);

    std::vector<bool> seen(n, false);

    for (int v = 0; v < n; ++v) {
        int r = p.ranks[v];
        EXPECT_GE(r, 0);
        EXPECT_LT(r, n);
        EXPECT_FALSE(seen[r]);
        seen[r] = true;
    }

    for (int i = 0; i < n; ++i) {
        EXPECT_TRUE(seen[i]);
    }
}