//
// Created by gogagum on 30.05.22.
//

#include <gtest/gtest.h>
#include "../src/DSUWithData.hpp"
#include "../samples/test_dsu_data.hpp"
#include "../samples/greatest_element_dsu_data.hpp"

TEST(CustomData, FloatAvgData) {
    auto dsu =
        gdsu::DSUWithData<float, FloatAvgRootData>(std::initializer_list<float>{2, 3, 5});

    dsu.joinByKeys(2, 3);

    EXPECT_FLOAT_EQ(dsu.getRootData(2).getAvg(), 2.5);
}

TEST(CustomData, GreatestData) {
    auto dsu =
            gdsu::DSUWithData<int,
                              GreatestElementRootDsuData<int>>(std::initializer_list<int>{1, 3, 6, 3, 5});

    dsu.joinByKeys(6, 3);
    dsu.joinByKeys(6, 1);

    ASSERT_EQ(dsu.getRootData(3).getGreatest(), 6);
}

TEST(CustomData, GreatestString) {
    using TestDsuType = gdsu::DSUWithData<std::string,
                                          GreatestElementRootDsuData<std::string>>;

    auto dsu = TestDsuType(std::initializer_list<std::string>{"aba", "caba", "ebgdae", "crmn", "ebfgd"});

    EXPECT_EQ(dsu.getRootData("ebgdae").getGreatest(), "ebgdae");

    dsu.joinByKeys("caba", "crmn");

    EXPECT_EQ(dsu.getRootData("caba").getGreatest(), "crmn");
    EXPECT_EQ(dsu.getComponentSize("caba"), 2);
}

TEST(CustomData, SmallestString) {
    using TestDsuType2 = gdsu::DSUWithData<std::string,
            GreatestElementRootDsuData<std::string, std::greater<>>,
            std::greater<std::string>>;

    auto dsu = TestDsuType2(std::initializer_list<std::string>{"aba", "caba", "ebgdae", "crmn", "ebfgd"});

    EXPECT_EQ(dsu.getRootData("ebgdae").getGreatest(), "ebgdae");

    dsu.joinByKeys("caba", "crmn");

    EXPECT_EQ(dsu.getRootData("crmn").getGreatest(), "caba");
    EXPECT_EQ(dsu.getComponentSize("caba"), 2);
}