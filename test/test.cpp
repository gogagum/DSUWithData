//
// Created by gogagum on 29.05.22.
//

#include <gtest/gtest.h>
#include "../src/DSUWithData.hpp"
#include "../src/DefaultDSUData.hpp"

TEST(HelloTest, BasicAssertions) {
    auto dsu =
    gdsu::DSUWithData<int, gdsu::BaseRootDSUData<int>, gdsu::BaseSimpleDSUData<int>>(
            std::initializer_list<int>{0, 1, 3});

    EXPECT_TRUE(dsu.getNumberOfComponents() == 3);
}