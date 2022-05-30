//
// Created by gogagum on 29.05.22.
//

#include <gtest/gtest.h>
#include "../src/DSUWithData.hpp"
#include "../samples/test_dsu_data.hpp"

TEST(DefaultData, KeysConstructor) {
    auto dsu =
    gdsu::DSUWithData<int>({0, 1, 3});

    EXPECT_EQ(dsu.getNumberOfComponents(), 3);
}

TEST(DefaultData, RootDataConstructor) {
    const auto rootDataVec =
            std::vector<gdsu::BaseRootDSUData<int>>{
                    gdsu::BaseRootDSUData<int>(4),
                    gdsu::BaseRootDSUData<int>(5)
            };

    auto dsu = gdsu::DSUWithData<int>(rootDataVec.begin(), rootDataVec.end());
    EXPECT_EQ(dsu.getNumberOfComponents(), 2);
    dsu.joinByKeys(4, 5);
    EXPECT_EQ(dsu.getNumberOfComponents(), 1);
}

TEST(DefaultData, ConstructFromVector) {
    auto keysVec = std::vector{1, 2, 3};
    auto dsu = gdsu::DSUWithData<int>{keysVec.begin(), keysVec.end()};

    EXPECT_EQ(dsu.getNumberOfComponents(), 3);
    dsu.joinByKeys(2, 3);
    EXPECT_EQ(dsu.getNumberOfComponents(), 2);
}

TEST(DefaultData, ConstructFromVectorRepeatingKeys) {
    auto keysVec = std::vector{1, 2, 2};
    auto dsu = gdsu::DSUWithData<int>{keysVec.begin(), keysVec.end()};

    EXPECT_EQ(dsu.getNumberOfComponents(), 2);
    EXPECT_EQ(dsu.getComponent(2).getSize(), 1);
}

TEST(DefaultData, Join) {
    auto dsu = gdsu::DSUWithData<int>(std::initializer_list<int>{0, 1});

    EXPECT_EQ(dsu.getNumberOfComponents(), 2);

    auto component0 = dsu.getComponent(0);
    auto component1 = dsu.getComponent(1);

    dsu.join(std::move(component0), std::move(component1));

    EXPECT_EQ(dsu.getNumberOfComponents(), 1);
    EXPECT_EQ(dsu.getComponent(0).getSize(), 2);
    EXPECT_EQ(dsu.getComponent(1).getSize(), 2);
    EXPECT_EQ(&dsu.getComponent(0), &dsu.getComponent(1));
}

TEST(DefaultData, JoinByKey) {
    auto dsu = gdsu::DSUWithData<int>({0, 1});


    EXPECT_EQ(dsu.getNumberOfComponents(), 2);

    dsu.joinByKeys(0, 1);

    EXPECT_EQ(dsu.getNumberOfComponents(), 1);
    EXPECT_EQ(dsu.getComponent(0).getSize(), 2);
    EXPECT_EQ(dsu.getComponent(1).getSize(), 2);
    EXPECT_EQ(&dsu.getComponent(0), &dsu.getComponent(1));
}

TEST(DefaultData, TestInSameComponent) {
    auto dsu = gdsu::DSUWithData<int>{1, 2, 3, 4, 5};

    ASSERT_FALSE(dsu.inSameComponent(1, 2));

    dsu.joinByKeys(1, 2);

    ASSERT_TRUE(dsu.inSameComponent(1, 2));
}

TEST(DefaultData, JoinFive) {
    auto dsu =
        gdsu::DSUWithData<std::string>({"one", "two", "three", "four", "five"});

    dsu.joinByKeys("one", "two");
    dsu.joinByKeys("two", "three");

    dsu.joinByKeys("four", "five");

    EXPECT_EQ(dsu.getNumberOfComponents(), 2);
    EXPECT_EQ(&dsu.getComponent("one"), &dsu.getComponent("three"));
    EXPECT_EQ(&dsu.getComponent("four"), &dsu.getComponent("five"));
    EXPECT_FALSE(&dsu.getComponent("one") == &dsu.getComponent("five"));
}

TEST(CustomData, FloatAvgData) {
    auto dsu =
            gdsu::DSUWithData<float, FloatAvgRootData, FloatAvgSimpleData>(std::initializer_list<float>{2, 3, 5});

    dsu.joinByKeys(2, 3);

    EXPECT_FLOAT_EQ(dsu.getRootData(2).getAvg(), 2.5);
}
