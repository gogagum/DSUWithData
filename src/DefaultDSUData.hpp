//
// Created by gogagum on 18.04.22.
//

#ifndef DSU_WITH_DATA_DEFAULTDSUDATA_HPP
#define DSU_WITH_DATA_DEFAULTDSUDATA_HPP

#include <cstdint>

namespace gdsu {

    enum DSUDataType {
        eRoot = 0,
        eSimple = 1
    };

    ////////////////////////////////////////////////////////////////////////////
    // class BaseDSUData<KeyT>
    template<class KeyT>
    class BaseDSUData {
    public:
        const KeyT& getKey() const;

    protected:
        explicit BaseDSUData(const KeyT& key);

    protected:
        KeyT _key;
    };

    template<class KeyT>
    class BaseSimpleDSUData : public  BaseDSUData<KeyT> {
    public:
        static constexpr DSUDataType dataType = eSimple;

    public:
        explicit BaseSimpleDSUData(const KeyT& key);
    };

    ////////////////////////////////////////////////////////////////////////////
    // class BaseRootDSUData<KeyT>
    template<class KeyT>
    class BaseRootDSUData : public BaseDSUData<KeyT> {
    public:
        static constexpr DSUDataType dataType = eRoot;

    public:
        explicit BaseRootDSUData(const KeyT& key);

    public:
        // Join with other root.
        // @param other - other data to join.
        // @return data to reset after join.
        BaseSimpleDSUData<KeyT> joinWith(BaseRootDSUData<KeyT>&& other);
    };
}

#endif //DSU_WITH_DATA_DEFAULTDSUDATA_HPP

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template<class KeyT>
gdsu::BaseDSUData<KeyT>::BaseDSUData(const KeyT &key) : _key(key) {}

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template<class KeyT>
const KeyT &gdsu::BaseDSUData<KeyT>::getKey() const {
    return _key;
}

//----------------------------------------------------------------------------//
template<class KeyT>
gdsu::BaseSimpleDSUData<KeyT>::BaseSimpleDSUData(const KeyT &key)
  : BaseDSUData<KeyT>(key) { }

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template<class KeyT>
gdsu::BaseRootDSUData<KeyT>::BaseRootDSUData(const KeyT& key)
  : BaseDSUData<KeyT>(key)  {}

//----------------------------------------------------------------------------//
template<class KeyT>
inline gdsu::BaseSimpleDSUData<KeyT>
gdsu::BaseRootDSUData<KeyT>::joinWith(BaseRootDSUData<KeyT> &&other) {
    return BaseSimpleDSUData<KeyT>(other._key);
}