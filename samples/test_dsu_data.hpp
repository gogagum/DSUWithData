//
// Created by gogagum on 28.05.22.
//

#ifndef SIMPLE_DSU_SAMPLE_TEST_DSU_DATA_HPP
#define SIMPLE_DSU_SAMPLE_TEST_DSU_DATA_HPP

#include "../src/DefaultDSUData.hpp"

////////////////////////////////////////////////////////////////////////////////
// class FloatAvgSimpleData
class FloatAvgSimpleData : public gdsu::BaseSimpleDSUData<float> {
public:
    FloatAvgSimpleData(float key) : gdsu::BaseSimpleDSUData<float>(key) {};
};

////////////////////////////////////////////////////////////////////////////////
// class FloatAvgRootData
class FloatAvgRootData : public gdsu::BaseRootDSUData<float> {
public:
    FloatAvgRootData(float key) : gdsu::BaseRootDSUData<float>(key), _size(1), _avg(key) {};

    FloatAvgSimpleData joinWith(FloatAvgRootData&& other);

    float getAvg() const;

private:
    float _avg;
    std::size_t _size;
};

#endif //SIMPLE_DSU_SAMPLE_TEST_DSU_DATA_HPP

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
FloatAvgSimpleData FloatAvgRootData::joinWith(FloatAvgRootData &&other) {
    _size += other._size;
    _avg = (this->_key + other._key) / static_cast<float>(_size);
    return {other._key};
}

//----------------------------------------------------------------------------//
float FloatAvgRootData::getAvg() const {
    return _avg;
}
