#include <gtest/gtest.h>
#include "file_facilities.hpp"
#include <string>
#include <vector>

static void compare_destinations(const std::vector<CHGraph::Destination> &destination1,
                                 const std::vector<CHGraph::Destination> &destination2);

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

    compare_destinations(expected_destinations, destinations);
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

    compare_destinations(expected_destinations, destinations);
}

TEST(ReadDestinationsTests, SuccessfulRetrievingEmptyList)
{
    const std::vector<CHGraph::Destination> expected_destinations = {};

    std::vector<CHGraph::Destination> destinations;
    const std::string destinations_file_path = "tst/data/test_facilities/destinations_03.txt";

    EXPECT_NO_THROW(FileFacilities::read_destinations(destinations_file_path, destinations));

    compare_destinations(expected_destinations, destinations);
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

static void compare_destinations(const std::vector<CHGraph::Destination> &destinations1,
                                 const std::vector<CHGraph::Destination> &destinations2)
{
    EXPECT_EQ(destinations1.size(), destinations2.size());
    for (int ind = 0; ind < destinations1.size(); ++ind)
    {
        EXPECT_EQ(destinations1[ind].source, destinations2[ind].source);
        EXPECT_EQ(destinations1[ind].target, destinations2[ind].target);
    }
}