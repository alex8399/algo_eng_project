#include <gtest/gtest.h>
#include "file_facilities.hpp"
#include <string>


TEST(FileFacilitiesTest, ReadGraph_DIMACS) {
    CHGraph::Graph graph;
    std::string file_path = "tst/data/test_facilities/test_graph_dimacs.gr";
    FileFacilities::read_graph(file_path, graph);
    
    EXPECT_EQ(graph.num_nodes, 3);
    EXPECT_EQ(graph.from.size(), 2);
    EXPECT_EQ(graph.to.size(), 2);
    EXPECT_EQ(graph.weights.size(), 2);
    
    // Check edges (0-based indexing)
    EXPECT_EQ(graph.from[0], 0);
    EXPECT_EQ(graph.to[0], 1);
    EXPECT_DOUBLE_EQ(graph.weights[0], 10.0);
    
    EXPECT_EQ(graph.from[1], 1);
    EXPECT_EQ(graph.to[1], 2);
    EXPECT_DOUBLE_EQ(graph.weights[1], 20.0);
}

TEST(FileFacilitiesTest, ReadGraph_Alternative) {
    CHGraph::Graph graph;
    std::string file_path = "tst/data/test_facilities/test_graph_alt_format.gr";
    FileFacilities::read_graph(file_path, graph);
    
    EXPECT_EQ(graph.num_nodes, 3);
    EXPECT_EQ(graph.from.size(), 2);
    EXPECT_EQ(graph.to.size(), 2);
    EXPECT_EQ(graph.weights.size(), 2);
    
    // Check edges (already 0-based)
    EXPECT_EQ(graph.from[0], 0);
    EXPECT_EQ(graph.to[0], 1);
    EXPECT_DOUBLE_EQ(graph.weights[0], 10.0);
    
    EXPECT_EQ(graph.from[1], 1);
    EXPECT_EQ(graph.to[1], 2);
    EXPECT_DOUBLE_EQ(graph.weights[1], 20.0);
}


TEST(FileFacilitiesTest, ReadGraph_HeaderOnly) {
    CHGraph::Graph graph;
    std::string file_path = "tst/data/test_facilities/test_graph_header_only.gr";
    FileFacilities::read_graph(file_path, graph);
    
    EXPECT_EQ(graph.num_nodes, 10);
    EXPECT_EQ(graph.from.size(), 0);
    EXPECT_EQ(graph.to.size(), 0);
    EXPECT_EQ(graph.weights.size(), 0);
}

TEST(FileFacilitiesTest, ReadGraph_LargeDIMACS) {
    CHGraph::Graph graph;
    std::string file_path = "tst/data/test_facilities/NY-road-time.gr";
    FileFacilities::read_graph(file_path, graph);
    
    EXPECT_EQ(graph.num_nodes, 264346);
    EXPECT_EQ(graph.from.size(), 733846);
    EXPECT_EQ(graph.to.size(), 733846);
    EXPECT_EQ(graph.weights.size(), 733846);
    
    // Check the first arc: a 1 2 2008 -> from 0, to 1, weight 2008
    EXPECT_EQ(graph.from[0], 0);
    EXPECT_EQ(graph.to[0], 1);
    EXPECT_DOUBLE_EQ(graph.weights[0], 2008.0);
}

TEST(FileFacilitiesTest, ReadGraph_FileNotFound) {
    CHGraph::Graph graph;
    std::string file_path = "tst/data/test_facilities/nonexistent.gr";
    EXPECT_THROW(FileFacilities::read_graph(file_path, graph), std::runtime_error);
}

TEST(FileFacilitiesTest, ReadGraph_InvalidNegativeNodes) {
    CHGraph::Graph graph;
    std::string file_path = "tst/data/test_facilities/test_graph_invalid_negative.gr";
    EXPECT_THROW(FileFacilities::read_graph(file_path, graph), std::runtime_error);
}

TEST(FileFacilitiesTest, ReadGraph_InvalidRangeNodes) {
    CHGraph::Graph graph;
    std::string file_path = "tst/data/test_facilities/test_graph_invalid_range.gr";
    EXPECT_THROW(FileFacilities::read_graph(file_path, graph), std::runtime_error);
}

TEST(FileFacilitiesTest, ReadGraph_MalformedArc) {
    CHGraph::Graph graph;
    std::string file_path = "tst/data/test_facilities/test_graph_malformed_arc.gr";
    EXPECT_THROW(FileFacilities::read_graph(file_path, graph), std::runtime_error);
}

TEST(FileFacilitiesTest, ReadGraph_InvalidHeader) {
    CHGraph::Graph graph;
    std::string file_path = "tst/data/test_facilities/test_graph_invalid_header.gr";
    EXPECT_THROW(FileFacilities::read_graph(file_path, graph), std::runtime_error);
}

TEST(FileFacilitiesTest, ReadGraph_AltInvalid) {
    CHGraph::Graph graph;
    std::string file_path = "tst/data/test_facilities/test_graph_alt_invalid.gr";
    EXPECT_THROW(FileFacilities::read_graph(file_path, graph), std::runtime_error);
}