//
// Created by gogagum on 20.04.22.
//

#include "../src/DSUWithData.hpp"
#include "../src/DefaultDSUData.hpp"

void default_dsu_test() {
    auto simpleData1 = gdsu::BaseRootDSUData<int>(0);
    auto simpleData2 = gdsu::BaseRootDSUData<int>(0);

    auto dsu =
        gdsu::DSUWithData<int, gdsu::BaseRootDSUData, gdsu::BaseSimpleDSUData>({simpleData1, simpleData2});



}


int main() {
    default_dsu_test();
    return 0;
}