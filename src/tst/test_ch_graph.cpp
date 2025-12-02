#include <gtest/gtest.h>
#include "ch_graph.hpp"

TEST(TestCaategory1, Test1) {
    CHGraph::Graph graph;
    CHGraph::PreprocGraph preproc_graph;
    CHGraph::preproc_graph_bottom_up(graph, preproc_graph);
    CHGraph::preproc_graph_bottom_up(graph, preproc_graph);
    EXPECT_EQ(3+2, 5);
    EXPECT_TRUE(true);
}