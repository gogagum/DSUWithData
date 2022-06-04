//
// Created by gogagum on 30.05.22.
//

#include <gtest/gtest.h>
#include "../src/DSUWithData.hpp"
#include "../samples/test_dsu_data.hpp"
#include "../samples/greatest_element_dsu_data.hpp"

TEST(CustomData, FloatAvgData) {
    auto dsu =
        gdsu::DSUWithData<float, FloatAvgRootData, FloatAvgSimpleData>(std::initializer_list<float>{2, 3, 5});

    dsu.joinByKeys(2, 3);

    EXPECT_FLOAT_EQ(dsu.getRootData(2).getAvg(), 2.5);
}

TEST(CustomData, GreatestData) {
    auto dsu =
            gdsu::DSUWithData<int,
                              GreatestElementRootDsuData<int>,
                              GreatestElementSimpleDsuData<int>>(std::initializer_list<int>{1, 3, 6, 3, 5});

    dsu.joinByKeys(6, 3);
    dsu.joinByKeys(6, 1);

    ASSERT_EQ(dsu.getRootData(3).getGreatest(), 6);
}
