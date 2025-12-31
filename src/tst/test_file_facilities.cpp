#include <gtest/gtest.h>
#include "file_facilities.hpp"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include "measurement.hpp"
#include <filesystem>


constexpr double EPS = 1e-9;


static void expect_equal_destinations(const std::vector<CHGraph::Destination> &destination1,
                                      const std::vector<CHGraph::Destination> &destination2);

static void expect_equal_double_vectors(const std::vector<double> &vector1, const std::vector<double> &vector2, const double eps);

static void expect_equal_graphs(const CHGraph::Graph &graph1, const CHGraph::Graph &graph2);

static void expect_equal_text_files(const std::string &file1, const std::string &file2);


static void expect_equal_destinations(const std::vector<CHGraph::Destination> &destinations1,
                                 const std::vector<CHGraph::Destination> &destinations2)
{
    EXPECT_EQ(destinations1.size(), destinations2.size());
    for (int ind = 0; ind < destinations1.size(); ++ind)
    {
        EXPECT_EQ(destinations1[ind].source, destinations2[ind].source);
        EXPECT_EQ(destinations1[ind].target, destinations2[ind].target);
    }
}

TEST(ReadDestinationsTests, SuccessfulRetrieving01)
{
    const std::vector<CHGraph::Destination> expected_destinations = {
        CHGraph::Destination{.source = 1, .target = 10},
        CHGraph::Destination{.source = 2, .target = 20},
        CHGraph::Destination{.source = 89, .target = 56},
    };

    std::vector<CHGraph::Destination> destinations;
    const std::string destinations_file_path = "tst/data/test_facilities/destinations_01.txt";

    EXPECT_NO_THROW(FileFacilities::read_destinations(destinations_file_path, destinations));

    expect_equal_destinations(expected_destinations, destinations);
}

TEST(ReadDestinationsTests, SuccessfulRetrieving02)
{
    const std::vector<CHGraph::Destination> expected_destinations = {
        CHGraph::Destination{.source = 454, .target = 65},
        CHGraph::Destination{.source = 223, .target = 24},
        CHGraph::Destination{.source = 1, .target = 10},
        CHGraph::Destination{.source = 3, .target = 56},
        CHGraph::Destination{.source = 34, .target = 99},
        CHGraph::Destination{.source = 5, .target = 5}};

    std::vector<CHGraph::Destination> destinations;
    const std::string destinations_file_path = "tst/data/test_facilities/destinations_02.txt";

    EXPECT_NO_THROW(FileFacilities::read_destinations(destinations_file_path, destinations));

    expect_equal_destinations(expected_destinations, destinations);
}

TEST(ReadDestinationsTests, SuccessfulRetrievingEmptyList)
{
    const std::vector<CHGraph::Destination> expected_destinations = {};

    std::vector<CHGraph::Destination> destinations;
    const std::string destinations_file_path = "tst/data/test_facilities/destinations_03.txt";

    EXPECT_NO_THROW(FileFacilities::read_destinations(destinations_file_path, destinations));

    expect_equal_destinations(expected_destinations, destinations);
}

TEST(ReadDestinationsTests, FileNotExist)
{
    std::vector<CHGraph::Destination> destinations;
    const std::string destinations_file_path = "tst/data/test_facilities/non_existing_destinations_file.txt";

    EXPECT_THROW(FileFacilities::read_destinations(destinations_file_path, destinations), std::runtime_error);
}

TEST(ReadDestinationsTests, NegativeNodeNumberInFile)
{
    std::vector<CHGraph::Destination> destinations;
    const std::string destinations_file_path = "tst/data/test_facilities/destinations_04.txt";

    EXPECT_THROW(FileFacilities::read_destinations(destinations_file_path, destinations), std::runtime_error);
}

TEST(ReadDestinationsTests, IncorrectFormattedLineInFile01)
{
    std::vector<CHGraph::Destination> destinations;
    const std::string destinations_file_path = "tst/data/test_facilities/destinations_05.txt";

    EXPECT_THROW(FileFacilities::read_destinations(destinations_file_path, destinations), std::runtime_error);
}

TEST(ReadDestinationsTests, IncorrectFormattedLineInFile02)
{
    std::vector<CHGraph::Destination> destinations;
    const std::string destinations_file_path = "tst/data/test_facilities/destinations_06.txt";

    EXPECT_THROW(FileFacilities::read_destinations(destinations_file_path, destinations), std::runtime_error);
}

static void expect_equal_double_vectors(const std::vector<double> &vector1, const std::vector<double> &vector2, const double eps)
{
    EXPECT_EQ(vector1.size(), vector2.size());
    for (int ind = 0; ind < vector1.size(); ++ind)
    {
        EXPECT_NEAR(vector1[ind], vector2[ind], eps);
    }
}

static void expect_equal_graphs(const CHGraph::Graph &graph1, const CHGraph::Graph &graph2)
{
    EXPECT_EQ(graph1.first_out, graph2.first_out);
    EXPECT_EQ(graph1.from, graph2.from);
    EXPECT_EQ(graph1.to, graph2.to);
    expect_equal_double_vectors(graph1.weights, graph2.weights, EPS);
}

TEST(ReadGraphFileTests, SuccessfulRetrieving01)
{
    const CHGraph::Graph expected_graph = {
        .first_out =    {0, 3, 6, 6},
        .from =         {0,     0,      0,      1,      1},
        .to =           {1,     2,      3,      2,      3},
        .weights =      {1.5,   2.5,    3.5,    4.5,    5.5},
    };

    CHGraph::Graph graph;
    const std::string graph_file_path = "tst/data/test_facilities/graph_01.txt";

    EXPECT_NO_THROW(FileFacilities::read_graph(graph_file_path, graph));

    expect_equal_graphs(expected_graph, graph);
}

TEST(ReadGraphFileTests, SuccessfulRetrieving02)
{
    const CHGraph::Graph expected_graph = {
        .first_out =    {0, 4, 9, 5, 7},
        .from =         {0,     0,      0,      0,      1,      3,      3,      4},
        .to =           {2,     4,      1,      3,      4,      4,      0,      1},
        .weights =      {1.23,  45.0,   0.3,    0.0,    0.5,    45.8,   0.0,    23.5},
    };

    CHGraph::Graph graph;
    const std::string graph_file_path = "tst/data/test_facilities/graph_02.txt";

    EXPECT_NO_THROW(FileFacilities::read_graph(graph_file_path, graph));

    expect_equal_graphs(expected_graph, graph);
}

TEST(ReadGraphFileTests, SuccessfulRetrieving03)
{
    const CHGraph::Graph expected_graph = {
        .first_out =    {0, 1, 6, 7, 8, 11, 11, 9},
        .from =         {0,     1,      1,      1,      1,      1,      2,          3,      4,      7},
        .to =           {1,     0,      7,      4,      3,      2,      1,          1,      1,      1},
        .weights =      {1.23,  0.5,    45.0,   0.3,    0.0,    45.5,   767.52,     23.5,   0.0,    45.8},
    };

    CHGraph::Graph graph;
    const std::string graph_file_path = "tst/data/test_facilities/graph_03.txt";

    EXPECT_NO_THROW(FileFacilities::read_graph(graph_file_path, graph));

    expect_equal_graphs(expected_graph, graph);
}

TEST(ReadGraphFileTests, FileNotExist)
{
    CHGraph::Graph graph;
    const std::string graph_file_path = "tst/data/test_facilities/non_existing_graph_file.txt";

    EXPECT_THROW(FileFacilities::read_graph(graph_file_path, graph), std::runtime_error);
}

TEST(ReadGraphFileTests, IncorrectEdgeNumberInFile)
{
    CHGraph::Graph graph;
    const std::string graph_file_path = "tst/data/test_facilities/graph_04.txt";

    EXPECT_THROW(FileFacilities::read_graph(graph_file_path, graph), std::runtime_error);
}

TEST(ReadGraphFileTests, NegativeNodeNumberInFile)
{
    CHGraph::Graph graph;
    const std::string graph_file_path = "tst/data/test_facilities/graph_05.txt";

    EXPECT_THROW(FileFacilities::read_graph(graph_file_path, graph), std::runtime_error);
}

TEST(ReadGraphFileTests, NegativeEdgeWeightInFile)
{
    CHGraph::Graph graph;
    const std::string graph_file_path = "tst/data/test_facilities/graph_06.txt";

    EXPECT_THROW(FileFacilities::read_graph(graph_file_path, graph), std::runtime_error);
}

TEST(ReadGraphFileTests, IncorrectNodeNumberInFile)
{
    CHGraph::Graph graph;
    const std::string graph_file_path = "tst/data/test_facilities/graph_07.txt";

    EXPECT_THROW(FileFacilities::read_graph(graph_file_path, graph), std::runtime_error);
}

static void expect_equal_text_files(const std::string &file1_path, const std::string &file2_path)
{
    std::ifstream file1(file1_path);
    std::ifstream file2(file2_path);

    std::string line1, line2;
    bool run = true;

    while (run)
    {
        bool eof1 = !std::getline(file1, line1);
        bool eof2 = !std::getline(file2, line2);

        if (eof1 || eof2)
        {
            EXPECT_TRUE(eof1 && eof2);
            run = false;
        }
        else
        {
            EXPECT_EQ(line1, line2);
        }
    }

    file1.close();
    file2.close();
}

TEST(DumpMeasurementTests, SuccessfulWriting01)
{
    const std::string expected_dump_measurement ="tst/data/test_facilities/measurement_01.csv";

    const Measurement measurement = {
        .data = {
            {   "column1",     {34, 45}},
            {   "column2",     {43, 12}},
            {   "column3",     {23, 43}}
        }
    };
    const std::string dump_measurement_path = "tst/tmp/measurement_01.tmp";

    EXPECT_NO_THROW(FileFacilities::dump_measurement(measurement, dump_measurement_path));

    expect_equal_text_files(dump_measurement_path, expected_dump_measurement);
}

TEST(DumpMeasurementTests, SuccessfulWriting02)
{
    const std::string expected_dump_measurement ="tst/data/test_facilities/measurement_02.csv";

    const Measurement measurement = {
        .data = {
            {   "asd",          {23, 766, 2367, 45}},
            {   "cxcv",         {434556, 12}},
            {   "aaaaab",       {235656, 4365, 23}}
        }
    };
    const std::string dump_measurement_path = "tst/tmp/measurement_02.tmp";

    EXPECT_NO_THROW(FileFacilities::dump_measurement(measurement, dump_measurement_path));

    expect_equal_text_files(dump_measurement_path, expected_dump_measurement);
}