//
// Created by gogagum on 20.04.22.
//

#include <string>

#include "../src/DSUWithData.hpp"
#include "../src/DefaultDSUData.hpp"

//----------------------------------------------------------------------------//
void create_test() {
    auto dsu =
        gdsu::DSUWithData<int, gdsu::BaseRootDSUData, gdsu::BaseSimpleDSUData>(
                {0, 1, 3});

    assert(dsu.getNumberOfComponents() == 3);
}

//----------------------------------------------------------------------------//
void join_test() {
    auto dsu =
            gdsu::DSUWithData<int, gdsu::BaseRootDSUData, gdsu::BaseSimpleDSUData>({0, 1});

    assert(dsu.getNumberOfComponents() == 2);

    auto component0 = dsu.getComponent(0);
    auto component1 = dsu.getComponent(1);

    dsu.join(std::move(component0), std::move(component1));

    assert(dsu.getNumberOfComponents() == 1);
    assert(dsu.getComponent(0).getSize() == 2);
    assert(dsu.getComponent(1).getSize() == 2);
    assert(&dsu.getComponent(0) == &dsu.getComponent(1));
}

//----------------------------------------------------------------------------//
void join_by_key_test() {
    auto dsu =
            gdsu::DSUWithData<int, gdsu::BaseRootDSUData, gdsu::BaseSimpleDSUData>({0, 1});

    assert(dsu.getNumberOfComponents() == 2);

    dsu.joinByKeys(0, 1);

    assert(dsu.getNumberOfComponents() == 1);
    assert(dsu.getComponent(0).getSize() == 2);
    assert(dsu.getComponent(1).getSize() == 2);
    assert(&dsu.getComponent(0) == &dsu.getComponent(1));
}

//----------------------------------------------------------------------------//
void join_five() {
    auto dsu =
            gdsu::DSUWithData<std::string,
                              gdsu::BaseRootDSUData,
                              gdsu::BaseSimpleDSUData>({"one", "two", "three", "four", "five"});

    dsu.joinByKeys("one", "two");
    dsu.joinByKeys("two", "three");

    dsu.joinByKeys("four", "five");

    assert(dsu.getNumberOfComponents() == 2);
    assert(&dsu.getComponent("one") == &dsu.getComponent("three"));
}

int main() {
    create_test();
    join_test();
    join_by_key_test();
    join_five();
    return 0;
}
