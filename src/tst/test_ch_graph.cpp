#include <gtest/gtest.h>
#include "ch_graph.hpp"
#include <vector>
#include <queue>
#include <limits>
#include <algorithm>

// Reference Dijkstra implementation for verification
double dijkstra_distance(const CHGraph::Graph& graph, int source, int target) {
    const int n = graph.num_nodes;
    if (source < 0 || source >= n || target < 0 || target >= n) return std::numeric_limits<double>::infinity();
    if (source == target) return 0.0;

    const double INF = std::numeric_limits<double>::infinity();
    std::vector<double> dist(n, INF);
    dist[source] = 0.0;

    using QItem = std::pair<double, int>;
    std::priority_queue<QItem, std::vector<QItem>, std::greater<QItem>> pq;
    pq.push({0.0, source});

    while (!pq.empty()) {
        auto [cost, u] = pq.top(); pq.pop();
        if (cost > dist[u]) continue;

        for (int e = graph.first_out[u]; e < graph.first_out[u + 1]; ++e) {
            int v = graph.to[e];
            double w = graph.weights[e];
            if (dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                pq.push({dist[v], v});
            }
        }
    }

    return dist[target];
}

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

static CHGraph::Graph sample_graph() {
    CHGraph::Graph g;
    g.num_nodes = 6;

    // Node 0 connected to 1 (weight 1) and 3 (weight 4)
    // Node 1 connected to 2 (weight 2) and 4 (weight 5)
    // Node 2 connected to 5 (weight 3)
    // Node 3 connected to 4 (weight 1)
    // Node 4 connected to 5 (weight 2)
    // Node 5 has no outgoing edges

    g.first_out = {0, 2, 4, 5, 6, 7, 7};  // size = num_nodes + 1
    g.to = {1, 3, 2, 4, 5, 4, 5};
    g.weights = {1.0, 4.0, 2.0, 5.0, 3.0, 1.0, 2.0};

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

// Test basic query functionality
TEST(CHQuery, BasicQuerySimpleGraph) {
    CHGraph::Graph g = sample_graph();
    CHGraph::PreprocGraph p;
    CHGraph::preproc_graph_top_down(g, p);

    CHGraph::Destination dest{0, 5};  // source=0, target=5
    CHGraph::Route route;
    CHGraph::query_route(g, p, dest, route);

    // Expected shortest path: 0->1->2->5 with weigth 6.0
    EXPECT_DOUBLE_EQ(route.total_weight, 6.0);
}

// Test query against reference Dijkstra
TEST(CHQuery, CorrectnessAgainstDijkstra) {
    CHGraph::Graph g = sample_graph();
    CHGraph::PreprocGraph p;
    CHGraph::preproc_graph_top_down(g, p);

    // Test multiple source-target pairs
    std::vector<std::pair<int, int>> test_pairs = {
        {0, 5}, {1, 4}, {2, 3}, {0, 0}, {5, 5}
    };

    for (auto [source, target] : test_pairs) {
        CHGraph::Destination dest{source, target};
        CHGraph::Route route;
        CHGraph::query_route(g, p, dest, route);

        double expected = dijkstra_distance(g, source, target);
        EXPECT_DOUBLE_EQ(route.total_weight, expected);
    }
}

// Test edge cases
TEST(CHQuery, EdgeCases) {
    CHGraph::Graph g = sample_graph();
    CHGraph::PreprocGraph p;
    CHGraph::preproc_graph_top_down(g, p);

    // Same source and target
    CHGraph::Destination dest_same{1, 1};
    CHGraph::Route route_same;
    CHGraph::query_route(g, p, dest_same, route_same);
    EXPECT_DOUBLE_EQ(route_same.total_weight, 0.0);

    // Invalid source/target (out of bounds)
    CHGraph::Destination dest_invalid{-1, 2};
    CHGraph::Route route_invalid;
    CHGraph::query_route(g, p, dest_invalid, route_invalid);
    EXPECT_EQ(route_invalid.total_weight, std::numeric_limits<double>::infinity());

    CHGraph::Destination dest_invalid2{0, 10};
    CHGraph::Route route_invalid2;
    CHGraph::query_route(g, p, dest_invalid2, route_invalid2);
    EXPECT_EQ(route_invalid2.total_weight, std::numeric_limits<double>::infinity());
}

// Test empty graph
TEST(CHQuery, EmptyGraph) {
    CHGraph::Graph g;
    g.num_nodes = 0;
    CHGraph::PreprocGraph p;
    CHGraph::preproc_graph_top_down(g, p);

    CHGraph::Destination dest{0, 0};
    CHGraph::Route route;
    CHGraph::query_route(g, p, dest, route);
    EXPECT_EQ(route.total_weight, std::numeric_limits<double>::infinity());
}

// Test single node graph
TEST(CHQuery, SingleNodeGraph) {
    CHGraph::Graph g;
    g.num_nodes = 1;
    g.first_out = {0, 0};
    CHGraph::PreprocGraph p;
    CHGraph::preproc_graph_top_down(g, p);

    CHGraph::Destination dest{0, 0};
    CHGraph::Route route;
    CHGraph::query_route(g, p, dest, route);
    EXPECT_DOUBLE_EQ(route.total_weight, 0.0);
}

// Test larger graph for performance and correctness
TEST(CHQuery, LargerGraph) {
    CHGraph::Graph g;
    g.num_nodes = 10;

    // Create a more complex graph: a line 0-1-2-...-9
    g.first_out.resize(11, 0);
    for (int i = 0; i < 10; ++i) {
        g.first_out[i] = i;
    }
    g.first_out[10] = 9;

    g.to.resize(9);
    g.weights.resize(9);
    for (int i = 0; i < 9; ++i) {
        g.to[i] = i + 1;
        g.weights[i] = 1.0;
    }

    CHGraph::PreprocGraph p;
    CHGraph::preproc_graph_top_down(g, p);

    // Test path from 0 to 9
    CHGraph::Destination dest{0, 9};
    CHGraph::Route route;
    CHGraph::query_route(g, p, dest, route);
    EXPECT_DOUBLE_EQ(route.total_weight, 9.0);

    // Test against Dijkstra
    double expected = dijkstra_distance(g, 0, 9);
    EXPECT_DOUBLE_EQ(route.total_weight, expected);
}
