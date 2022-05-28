//
// Created by gogagum on 20.04.22.
//

#include "../src/DSUWithData.hpp"
#include "../src/DefaultDSUData.hpp"

void default_dsu_test() {
    auto dsu =
        gdsu::DSUWithData<int, gdsu::BaseRootDSUData, gdsu::BaseSimpleDSUData>({0, 1});

    assert(dsu.getNumberOfComponents() == 2);

    ////////////////////////////////////////////////////////////////////////////
    {
        assert(dsu.getComponent(0).getSize() == 1);
        assert(dsu.getComponent(1).getSize() == 1);
    }

    ////////////////////////////////////////////////////////////////////////////
    {
        auto comp1 = dsu.getComponent(0);
        auto comp2 = dsu.getComponent(1);

        dsu.join(std::move(comp1), std::move(comp2));
        assert(dsu.getNumberOfComponents() == 1);
    }

    ////////////////////////////////////////////////////////////////////////////
    {
        assert(dsu.getComponent(0).getSize() == 2);
        assert(dsu.getComponent(1).getSize() == 2);
    }

    ////////////////////////////////////////////////////////////////////////////
    {
        const auto& rootData1 = dsu.getRootData(0);
        const auto& rootData2 = dsu.getRootData(1);

        assert(&rootData1 == &rootData2);
    }

    ////////////////////////////////////////////////////////////////////////////
    {
        const auto& rootData1 = dsu.getComponent(0).getRootData();
        const auto& rootData2 = dsu.getComponent(1).getRootData();

        assert(&rootData1 == &rootData2);
    }
}


int main() {
    default_dsu_test();
    return 0;
}
