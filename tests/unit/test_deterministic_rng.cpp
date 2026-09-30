#include <gtest/gtest.h>
#include "ofcl/core/deterministic_rng.hpp"

using namespace ofcl;

TEST(DeterministicRng, ReproducibleSequence) {
    DeterministicRng a(42);
    DeterministicRng b(42);
    for (int i = 0; i < 100; ++i) {
        EXPECT_DOUBLE_EQ(a.uniform01(), b.uniform01());
    }
}

TEST(DeterministicRng, DifferentSeedsDiffer) {
    DeterministicRng a(1);
    DeterministicRng b(2);
    bool differ = false;
    for (int i = 0; i < 20; ++i) {
        if (a.uniform01() != b.uniform01()) {
            differ = true;
            break;
        }
    }
    EXPECT_TRUE(differ);
}
