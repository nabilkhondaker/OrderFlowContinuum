#include <gtest/gtest.h>
#include "ofcl/continuum/field.hpp"

using namespace ofcl;

TEST(ContinuumField, ResizeAndProject) {
    ContinuumField field(32, 90.0, 110.0);
    EXPECT_EQ(field.size(), 32u);

    std::vector<std::pair<Price, Quantity>> bids = {{99.0, 10.0}, {98.5, 5.0}};
    std::vector<std::pair<Price, Quantity>> asks = {{100.0, 8.0}};
    field.project_from_book(bids, asks, 1.0);
    field.update_stress();

    double total_depth = 0.0;
    for (const auto& p : field.points()) total_depth += p.depth;
    EXPECT_GT(total_depth, 0.0);
}
