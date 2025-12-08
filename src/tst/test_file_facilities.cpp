#include <gtest/gtest.h>
#include "file_facilities.hpp"
#include <string>

TEST(TestCaategory2, Test2) {
    CHGraph::Graph graph;
    std::string file_path = "file.txt";
    FileFacilities::read_graph(file_path, graph);
    EXPECT_EQ(3+2, 5);
    EXPECT_TRUE(true);
}