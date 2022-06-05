//
// Created by gogagum on 28.05.22.
//

#ifndef SIMPLE_DSU_SAMPLE_TEST_DSU_DATA_HPP
#define SIMPLE_DSU_SAMPLE_TEST_DSU_DATA_HPP

#include "../src/DefaultDSUData.hpp"

////////////////////////////////////////////////////////////////////////////////
// class FloatAvgRootData
class FloatAvgRootData : public gdsu::BaseRootDSUData<float> {
public:
    explicit FloatAvgRootData(float key)
        : gdsu::BaseRootDSUData<float>(key), _size(1), _avg(key) {};

    void joinWith(FloatAvgRootData&& other);

    [[nodiscard]] float getAvg() const;

private:
    float _avg;
    std::size_t _size;
};

#endif //SIMPLE_DSU_SAMPLE_TEST_DSU_DATA_HPP

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
void FloatAvgRootData::joinWith(FloatAvgRootData &&other) {
    _size += other._size;
    _avg = (this->_avg + other._avg) / static_cast<float>(_size);
}

//----------------------------------------------------------------------------//
float FloatAvgRootData::getAvg() const {
    return _avg;
}
