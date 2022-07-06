//
// Created by gogagum on 18.04.22.
//

#ifndef DSU_WITH_DATA_DEFAULTDSUDATA_HPP
#define DSU_WITH_DATA_DEFAULTDSUDATA_HPP

#include <cstdint>
#include <utility>

namespace gdsu {

    ////////////////////////////////////////////////////////////////////////////
    // class BaseRootDSUData<KeyT>
    template<class KeyT>
    class BaseRootDSUData {
    public:
        explicit BaseRootDSUData(const KeyT& key);
        explicit BaseRootDSUData(KeyT&& key);

        const KeyT& getKey() const;
        [[nodiscard]] std::size_t getSize() const;
    public:
        // Join with other root.
        // @param other - other data to join.
        void joinWith(const BaseRootDSUData<KeyT>& other);
    protected:
        std::size_t _size;
    private:
        KeyT _key;
    };
}

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template<class KeyT>
gdsu::BaseRootDSUData<KeyT>::BaseRootDSUData(const KeyT& key)
        : _key(key), _size(1) {}

//----------------------------------------------------------------------------//
template<class KeyT>
gdsu::BaseRootDSUData<KeyT>::BaseRootDSUData(KeyT&& key)
        : _key(std::move(key)), _size(1) {}

//----------------------------------------------------------------------------//
template<class KeyT>
void
gdsu::BaseRootDSUData<KeyT>::joinWith(const BaseRootDSUData<KeyT>& other) {
    this->_size += other._size;
}

//----------------------------------------------------------------------------//
template<class KeyT>
std::size_t gdsu::BaseRootDSUData<KeyT>::getSize() const {
    return _size;
}

//----------------------------------------------------------------------------//
template<class KeyT>
const KeyT &gdsu::BaseRootDSUData<KeyT>::getKey() const { return _key; }

#endif //DSU_WITH_DATA_DEFAULTDSUDATA_HPP
