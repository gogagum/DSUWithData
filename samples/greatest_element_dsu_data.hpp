//
// Created by gogagum on 30.05.22.
//

#ifndef DSU_TEST_GREATEST_ELEMENT_DSU_DATA_HPP
#define DSU_TEST_GREATEST_ELEMENT_DSU_DATA_HPP

#include <functional>
#include "../src/DefaultDSUData.hpp"

template<class KeyT, class Comp = std::less<KeyT>>
class GreatestElementRootDsuData : public gdsu::BaseRootDSUData<KeyT> {
public:
    explicit GreatestElementRootDsuData(const KeyT& key)
            : gdsu::BaseRootDSUData<KeyT>(key), _greatest(key) {};

    void joinWith(GreatestElementRootDsuData<KeyT, Comp>&& other) {
        if (Comp()(_greatest, other._greatest)) {
            _greatest = other._greatest;
        }
    }

    [[nodiscard]] const KeyT& getGreatest() const { return _greatest; };

private:
    KeyT _greatest;
};

#endif //DSU_TEST_GREATEST_ELEMENT_DSU_DATA_HPP
