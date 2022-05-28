//
// Created by gogagum on 20.04.22.
//

#include "../src/DSUWithData.hpp"
#include "../src/DefaultDSUData.hpp"

void default_dsu_test() {
    auto simpleData1 = gdsu::BaseRootDSUData<int>(0);
    auto simpleData2 = gdsu::BaseRootDSUData<int>(0);

    auto dsu =
        gdsu::DSUWithData<int, gdsu::BaseRootDSUData, gdsu::BaseSimpleDSUData>({0, 1});

    assert(dsu.getNumberOfComponents() == 2);

    {
        auto comp1 = dsu.getComponent(0);
        auto comp2 = dsu.getComponent(1);

        dsu.join(std::move(comp1), std::move(comp2));
    }

    assert(dsu.getNumberOfComponents() == 1);
    auto component = dsu.getComponent(0);

    assert(component.getSize() == 2);

}


int main() {
    default_dsu_test();
    return 0;
}
