#include <gtest/gtest.h>
#include "file_facilities.hpp"
#include <string>
#include <vector>

TEST(ReadDestinationsTests, SuccessfulRetrieving01) {
    const std::vector<CHGraph::Destination> expeted_destinations = {
        CHGraph::Destination{.source = 1, .target = 10},
        CHGraph::Destination{.source = 2, .target = 20},
        CHGraph::Destination{.source = 89, .target = 56},
    };

    std::vector<CHGraph::Destination> destinations;
    const std::string destinations_file_path = "tst/data/test_facilities/destinations_01.txt";

    FileFacilities::read_destinations(destinations_file_path, destinations);

    EXPECT_EQ(expeted_destinations.size(), destinations.size());
    for(int ind = 0; ind < destinations.size(); ++ind)
    {
        EXPECT_EQ(expeted_destinations[ind].source, destinations[ind].source);
        EXPECT_EQ(expeted_destinations[ind].target, destinations[ind].target);
    }
}