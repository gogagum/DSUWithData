//
// Created by gogagum on 28.05.22.
//

#include "test_dsu_data.hpp"
#include "../src/DSUWithData.hpp"

void test_float_dsu() {
    auto dsu =
            gdsu::DSUWithData<float, FloatAvgRootData, FloatAvgSimpleData>(std::initializer_list<float>{2, 3, 5});

    dsu.joinByKeys(2, 3);

    assert(dsu.getRootData(2).getAvg() == 2.5);
}

int main() {
    test_float_dsu();

    return 0;
}