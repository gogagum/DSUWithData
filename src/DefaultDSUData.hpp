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
        BaseRootDSUData() { assert(false); };
        explicit BaseRootDSUData(const KeyT& key);
        explicit BaseRootDSUData(KeyT&& key);

        const KeyT& getKey() const { return _key; };
    public:
        // Join with other root.
        // @param other - other data to join.
        void joinWith(BaseRootDSUData<KeyT>&& other);
    private:
        KeyT _key;
    };
}

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template<class KeyT>
gdsu::BaseRootDSUData<KeyT>::BaseRootDSUData(const KeyT& key) : _key(key) {}

//----------------------------------------------------------------------------//
template<class KeyT>
gdsu::BaseRootDSUData<KeyT>::BaseRootDSUData(KeyT&& key) : _key(std::move(key)) {}

//----------------------------------------------------------------------------//
template<class KeyT>
void
gdsu::BaseRootDSUData<KeyT>::joinWith(BaseRootDSUData<KeyT> &&other) {}

#endif //DSU_WITH_DATA_DEFAULTDSUDATA_HPP


