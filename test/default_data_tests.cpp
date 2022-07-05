//
// Created by gogagum on 29.05.22.
//

#include <vector>
#include <list>

#include <gtest/gtest.h>
#include "../src/DSUWithData.hpp"

//----------------------------------------------------------------------------//
TEST(DefaultData, KeysConstructor) {
    auto dsu =
        gdsu::DSUWithData<int>(std::initializer_list<int>{0, 1, 3});

    EXPECT_EQ(dsu.getNumberOfComponents(), 3);
}

//----------------------------------------------------------------------------//
TEST(DefaultData, RootDataVectorConstructor) {
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

//----------------------------------------------------------------------------//
TEST(DefaultData, RootDataConstructor) {
    const auto dsu =
            gdsu::DSUWithData<int>(std::initializer_list<gdsu::BaseRootDSUData<int>>{
                gdsu::BaseRootDSUData<int>(1),
                gdsu::BaseRootDSUData<int>(2),
                gdsu::BaseRootDSUData<int>(3)
            });

    EXPECT_EQ(dsu.getNumberOfComponents(), 3);
}

//----------------------------------------------------------------------------//
TEST(DefaultData, RootDataILConstructorRepeatingKeys) {
    const auto action = []{
        const auto dsu =
                gdsu::DSUWithData<int>(
                    std::initializer_list<gdsu::BaseRootDSUData<int>>{
                        gdsu::BaseRootDSUData<int>(1),
                        gdsu::BaseRootDSUData<int>(2),
                        gdsu::BaseRootDSUData<int>(1)
                    }
                );
    };

    EXPECT_THROW(action(), std::invalid_argument);
}

//----------------------------------------------------------------------------//
TEST(DefaultData, ConstructFromVectorIterators) {
    auto keysVec = std::vector{1, 2, 3};
    auto dsu = gdsu::DSUWithData<int>{keysVec.begin(), keysVec.end()};

    EXPECT_EQ(dsu.getNumberOfComponents(), 3);
    dsu.joinByKeys(2, 3);
    EXPECT_EQ(dsu.getNumberOfComponents(), 2);
}

//----------------------------------------------------------------------------//
TEST(DefaultData, ConstructFromVectorIteratorsRepeatingKeys) {
    auto keysVec = std::vector{1, 2, 2};
    auto dsu = gdsu::DSUWithData<int>{keysVec.begin(), keysVec.end()};

    EXPECT_EQ(dsu.getNumberOfComponents(), 2);
    EXPECT_EQ(dsu.getComponentSize(2), 1);
}

//----------------------------------------------------------------------------//
TEST(DefaultData, RootDataConstructorRepeatingKeys) {
    const auto action = []{
        auto rootData = std::vector{
                gdsu::BaseRootDSUData<int>(1),
                gdsu::BaseRootDSUData<int>(2),
                gdsu::BaseRootDSUData<int>(1)
        };
        const auto dsu =
                gdsu::DSUWithData<int>(rootData.begin(), rootData.end());
    };

    EXPECT_THROW(action(), std::invalid_argument);
}

//----------------------------------------------------------------------------//
TEST(DefaultData, ConstructFromListIterators) {
    auto keysList = std::list{1, 2, 3};
    auto dsu = gdsu::DSUWithData<int>{keysList.begin(), keysList.end()};

    EXPECT_EQ(dsu.getNumberOfComponents(), 3);
    dsu.joinByKeys(2, 3);
    EXPECT_EQ(dsu.getNumberOfComponents(), 2);
}

//----------------------------------------------------------------------------//
TEST(DefaultData, ConstructFromListIteratorsRepeatingKeys) {
    auto keysVec = std::list{1, 2, 2};
    auto dsu = gdsu::DSUWithData<int>{keysVec.begin(), keysVec.end()};

    EXPECT_EQ(dsu.getNumberOfComponents(), 2);
    EXPECT_EQ(dsu.getComponentSize(2), 1);
}

//----------------------------------------------------------------------------//
TEST(DefaultData, ConstructFromSetIterators) {
    auto keysSet = std::set{1, 2, 3, 3};
    auto dsu = gdsu::DSUWithData<int>{keysSet.begin(), keysSet.end()};

    EXPECT_EQ(dsu.getNumberOfComponents(), 3);
    dsu.joinByKeys(2, 3);
    EXPECT_EQ(dsu.getNumberOfComponents(), 2);
}

//----------------------------------------------------------------------------//
TEST(DefaultData, ConstructFromUnorderedSetIterators) {
    auto keysSet = std::unordered_set{1, 2, 3, 3};
    auto dsu = gdsu::DSUWithData<int>{keysSet.begin(), keysSet.end()};

    EXPECT_EQ(dsu.getNumberOfComponents(), 3);
    dsu.joinByKeys(2, 3);
    EXPECT_EQ(dsu.getNumberOfComponents(), 2);
}

//----------------------------------------------------------------------------//
TEST(DefaultData, JoinByKey) {
    auto dsu = gdsu::DSUWithData<int>({0, 1});
    EXPECT_EQ(dsu.getNumberOfComponents(), 2);
    dsu.joinByKeys(0, 1);

    EXPECT_EQ(dsu.getNumberOfComponents(), 1);
    EXPECT_EQ(dsu.getComponentSize(0), 2);
    EXPECT_EQ(dsu.getComponentSize(1), 2);
    EXPECT_TRUE(dsu.inSameComponent(0, 1));
}

//----------------------------------------------------------------------------//
TEST(DefaultData, TestInSameComponent) {
    auto dsu = gdsu::DSUWithData<int>{1, 2, 3, 4, 5};

    ASSERT_FALSE(dsu.inSameComponent(1, 2));

    dsu.joinByKeys(1, 2);

    ASSERT_TRUE(dsu.inSameComponent(1, 2));
}

//----------------------------------------------------------------------------//
TEST(DefaultData, JoinFive) {
    auto dsu =
        gdsu::DSUWithData<std::string>({"one", "two", "three", "four", "five"});

    dsu.joinByKeys("one", "two");
    dsu.joinByKeys("two", "three");

    dsu.joinByKeys("four", "five");

    EXPECT_EQ(dsu.getNumberOfComponents(), 2);
    EXPECT_TRUE(dsu.inSameComponent("one", "three"));
    EXPECT_TRUE(dsu.inSameComponent("four", "five"));
    EXPECT_FALSE(dsu.inSameComponent("one", "five"));
}

//----------------------------------------------------------------------------//
TEST(DefaultData, SelfJoin) {
    auto dsu = gdsu::DSUWithData<int>{2, 3, 4, 5};

    dsu.joinByKeys(2, 2);

    EXPECT_EQ(dsu.getComponentSize(2), 1);
}

//----------------------------------------------------------------------------//
TEST(DefaultData, GetComponent) {
    auto dsu = gdsu::DSUWithData<int>{3, 4, 5, 6};

    ASSERT_EQ(dsu.getComponentSize(4), 1);
}

//----------------------------------------------------------------------------//
TEST(DefaultData, JoinAllByKeysAndWatchComponent) {
    auto dsu = gdsu::DSUWithData<int>{3, 4, 5, 6};

    dsu.joinByKeys(3, 4);
    dsu.joinByKeys(5, 6);

    ASSERT_EQ(dsu.getComponentSize(4), 2);
    ASSERT_EQ(dsu.getComponentSize(3), 2);
    ASSERT_TRUE(dsu.inSameComponent(3, 4));

    ASSERT_EQ(dsu.getComponentSize(5), 2);
    ASSERT_EQ(dsu.getComponentSize(6), 2);
    ASSERT_TRUE(dsu.inSameComponent(5, 6));

    ASSERT_FALSE(dsu.inSameComponent(3, 6));

    dsu.joinByKeys(3, 6);

    ASSERT_EQ(dsu.getNumberOfComponents(), 1);
    ASSERT_EQ(dsu.getComponentSize(5), 4);
}

//----------------------------------------------------------------------------//
TEST(DefaultData, GetRootData) {
    auto dsu = gdsu::DSUWithData<int>{2, 3, 4, 5};

    auto rootData = dsu.getRootData(3);
    EXPECT_EQ(rootData.getKey(), 3);
}

//----------------------------------------------------------------------------//
TEST(DefaultData, NoSuchKeyJoin0) {
    auto dsu = gdsu::DSUWithData<int>{1, 2, 3};

    EXPECT_THROW(dsu.joinByKeys(3, 5), std::invalid_argument);
}

//----------------------------------------------------------------------------//
TEST(DefaultData, NoSuchKeyJoin1) {
    auto dsu = gdsu::DSUWithData<int>{1, 2, 3};

    EXPECT_THROW(dsu.joinByKeys(4, 5), std::invalid_argument);
}

//----------------------------------------------------------------------------//
TEST(DefaultData, NoSuchKeyRootData) {
    auto dsu = gdsu::DSUWithData<int>{1, 2, 3};
    dsu.joinByKeys(2, 3);

    EXPECT_THROW(dsu.getRootData(5), std::invalid_argument);
}

//----------------------------------------------------------------------------//
TEST(DefaultData, NoSuchKeySameComponentCheck) {
    auto dsu = gdsu::DSUWithData<int>{1, 2, 3};
    EXPECT_THROW(dsu.inSameComponent(3, 5), std::invalid_argument);
}

//----------------------------------------------------------------------------//
TEST(DefaultData, NoSuchKeySameComponentCheck2) {
    auto dsu = gdsu::DSUWithData<int>{1, 2, 3};
    EXPECT_THROW(dsu.inSameComponent(4, 5), std::invalid_argument);
}

//----------------------------------------------------------------------------//
TEST(DefaultData, NoSuchKeyComponentSize) {
    auto dsu = gdsu::DSUWithData<int>{1, 2, 3};
    dsu.joinByKeys(2, 3);
    EXPECT_THROW(dsu.getComponentSize(42), std::invalid_argument);
}
