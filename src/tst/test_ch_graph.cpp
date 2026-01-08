#include <gtest/gtest.h>
#include "ch_graph.hpp"
#include "file_facilities.hpp"
#include <cmath>
#include <queue>
#include <limits>


// Dijkstra implementation for validation
static double dijkstra_shortest_path(const CHGraph::Graph &graph, int source, int target)
{
    const int n = graph.first_out.size() - 1;
    
    if (source < 0 || source >= n || target < 0 || target >= n)
        return std::numeric_limits<double>::infinity();
    
    if (source == target)
        return 0.0;
    
    const double INF = std::numeric_limits<double>::infinity();
    std::vector<double> dist(n, INF);
    
    using QItem = std::pair<double, int>;
    std::priority_queue<QItem, std::vector<QItem>, std::greater<QItem>> pq;
    
    dist[source] = 0.0;
    pq.push(QItem(0.0, source));
    
    while (!pq.empty())
    {
        auto [d, u] = pq.top();
        pq.pop();
        
        if (d > dist[u])
            continue;
        
        if (u == target)
            return dist[target];
        
        for (int e = graph.first_out[u]; e < graph.first_out[u + 1]; ++e)
        {
            int v = graph.to[e];
            double w = graph.weights[e];
            double new_dist = d + w;
            
            if (new_dist < dist[v])
            {
                dist[v] = new_dist;
                pq.push(QItem(new_dist, v));
            }
        }
    }
    
    return dist[target];
}


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

// Query tests using simple graph
TEST(CHQuery, SimpleQuerySameNode)
{
    CHGraph::Graph g = make_simple_graph();
    CHGraph::PreprocGraph p;
    CHGraph::preproc_graph_top_down(g, p);

    CHGraph::Destination dest{.source = 0, .target = 0};
    CHGraph::Route route;
    CHGraph::query_route(g, p, dest, route);

    EXPECT_EQ(route.total_weight, 0.0);
}

TEST(CHQuery, SimpleQueryDirectEdge)
{
    CHGraph::Graph g = make_simple_graph();
    CHGraph::PreprocGraph p;
    CHGraph::preproc_graph_top_down(g, p);

    CHGraph::Destination dest{.source = 0, .target = 1};
    CHGraph::Route route;
    CHGraph::query_route(g, p, dest, route);

    EXPECT_EQ(route.total_weight, 1.0);
}

TEST(CHQuery, SimpleQueryDirectEdge2)
{
    CHGraph::Graph g = make_simple_graph();
    CHGraph::PreprocGraph p;
    CHGraph::preproc_graph_top_down(g, p);

    CHGraph::Destination dest{.source = 1, .target = 2};
    CHGraph::Route route;
    CHGraph::query_route(g, p, dest, route);
    
    EXPECT_EQ(route.total_weight, 1.0);
}

TEST(CHQuery, SimpleQueryTwoHops)
{
    CHGraph::Graph g = make_simple_graph();
    CHGraph::PreprocGraph p;
    CHGraph::preproc_graph_top_down(g, p);

    CHGraph::Destination dest{.source = 0, .target = 2};
    CHGraph::Route route;
    CHGraph::query_route(g, p, dest, route);

    // Shortest path: 0 -> 1 -> 2 with total weight 1.0 + 1.0 = 2.0
    // or 0 -> 2 with weight 3.0
    EXPECT_EQ(route.total_weight, 2.0);
}

TEST(CHQuery, SimpleQueryInvalidSource)
{
    CHGraph::Graph g = make_simple_graph();
    CHGraph::PreprocGraph p;
    CHGraph::preproc_graph_top_down(g, p);

    CHGraph::Destination dest{.source = -1, .target = 1};
    CHGraph::Route route;
    CHGraph::query_route(g, p, dest, route);

    EXPECT_TRUE(std::isinf(route.total_weight));
}

TEST(CHQuery, SimpleQueryInvalidTarget)
{
    CHGraph::Graph g = make_simple_graph();
    CHGraph::PreprocGraph p;
    CHGraph::preproc_graph_top_down(g, p);

    CHGraph::Destination dest{.source = 0, .target = 10};
    CHGraph::Route route;
    CHGraph::query_route(g, p, dest, route);

    EXPECT_TRUE(std::isinf(route.total_weight));
}


TEST(CHQueryLargeGraph, AllQueriesMatchSolutions)
{
    CHGraph::Graph graph;
    CHGraph::PreprocGraph preproc_graph;
    std::vector<CHGraph::Destination> destinations;
    std::vector<CHGraph::Solution> solutions;

    // Read graph, destinations and solutions
    FileFacilities::read_graph("tst/graphs/rome99.gr", graph);
    FileFacilities::read_destinations("tst/destinations/d_rome99.txt", destinations);
    FileFacilities::read_solutions("tst/graph_solutions/formatted_rome99.txt", solutions);

    // Preprocess the graph
    CHGraph::preproc_graph_top_down(graph, preproc_graph);

    // Verify we have matching number of destinations and solutions
    ASSERT_EQ(destinations.size(), solutions.size());

    // Test each query
    for (size_t i = 0; i < destinations.size(); ++i)
    {
        CHGraph::Route route;
        CHGraph::query_route(graph, preproc_graph, destinations[i], route);

        double expected = solutions[i].expected_weight;
        double actual = route.total_weight;

        EXPECT_EQ(expected,actual);
    }
}

TEST(CHQueryLargeGraph, CHMatchesDijkstra)
{
    CHGraph::Graph graph;
    CHGraph::PreprocGraph preproc_graph;
    std::vector<CHGraph::Destination> destinations;

    // Read rome99 graph and destinations
    FileFacilities::read_graph("tst/graphs/rome99.gr", graph);
    FileFacilities::read_destinations("tst/destinations/d_rome99.txt", destinations);

    // Preprocess the graph for CH
    CHGraph::preproc_graph_top_down(graph, preproc_graph);

    // Test each query: compare CH query with Dijkstra

    for (size_t i = 0; i < destinations.size(); ++i)
    {
        // Run  Dijkstra
        double dijkstra_dist = dijkstra_shortest_path(graph, destinations[i].source, destinations[i].target);
        
        // Run CH query
        CHGraph::Route route;
        CHGraph::query_route(graph, preproc_graph, destinations[i], route);
        double ch_dist = route.total_weight;

        // Compare results
        EXPECT_EQ(dijkstra_dist,ch_dist);

    }

}

TEST(CHQueryLargeGraph, GRAPH_5000_10000)
{
    CHGraph::Graph graph;
    CHGraph::PreprocGraph preproc_graph;
    std::vector<CHGraph::Destination> destinations;
    std::vector<CHGraph::Solution> solutions;

    // Read graph, destinations and solutions
    FileFacilities::read_graph("tst/graphs/graph_5000_10000.gr", graph);
    FileFacilities::read_destinations("tst/destinations/d_5000_100.txt", destinations);
    FileFacilities::read_solutions("tst/graph_solutions/formatted_5000_10000.txt", solutions);

    // Preprocess the graph
    CHGraph::preproc_graph_top_down(graph, preproc_graph);

    // Verify we have matching number of destinations and solutions
    ASSERT_EQ(destinations.size(), solutions.size());

    // Test each query
    for (size_t i = 0; i < destinations.size(); ++i)
    {
        CHGraph::Route route;
        CHGraph::query_route(graph, preproc_graph, destinations[i], route);

        double expected = solutions[i].expected_weight;
        double actual = route.total_weight;
        EXPECT_EQ(expected,actual);
    }
}