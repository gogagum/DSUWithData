//
// Created by gogagum on 30.05.22.
//

#include <gtest/gtest.h>
#include "../src/DSUWithData.hpp"
#include "../samples/test_dsu_data.hpp"

TEST(CustomData, FloatAvgData) {
    auto dsu =
        gdsu::DSUWithData<float, FloatAvgRootData, FloatAvgSimpleData>(std::initializer_list<float>{2, 3, 5});

    dsu.joinByKeys(2, 3);

    EXPECT_FLOAT_EQ(dsu.getRootData(2).getAvg(), 2.5);
}