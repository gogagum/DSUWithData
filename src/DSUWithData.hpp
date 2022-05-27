//const KeyT& key
// Created by gogagum on 18.04.22.
//

#ifndef DSU_WITH_DATA_DSU_WITH_DATA_HPP
#define DSU_WITH_DATA_DSU_WITH_DATA_HPP

#include <cstdint>
#include <vector>
#include <variant>
#include <unordered_set>
#include <cassert>

namespace gdsu {

    ////////////////////////////////////////////////////////////////////////////
    // class DSUWithData<RootDataT, SimpleDataT>
    template<class RootDataT, class SimpleDataT>
    class DSUWithData {
    public:

        ////////////////////////////////////////////////////////////////////////
        // class DSUWithData<RootDataT, SimpleDataT>::Component
        class Component{
        public:
            using OwnerT = DSUWithData<RootDataT, SimpleDataT>;
        private:
            // Constructor
            // @param ownerPtr - DSU, in which component lays.
            // @param idx - implementation index of the component.
            // @param size - number of elements in the component.
            Component(OwnerT* ownerPtr, std::size_t idx, std::size_t size);
        public:
            // Get root data from component.
            // @return root data
            RootDataT& getRootData();

            // Get root data from component.
            // @return root data.
            const RootDataT& getRootData() const;

        private:
            OwnerT* _owner;
            mutable std::size_t _someIndex;
            std::size_t _size;
        };

        using ComponentRef = std::reference_wrapper<Component>;
        using ComponentConstRef = std::reference_wrapper<const Component>;

    public:
        // Constructor
        DSUWithData(std::size_t size, const RootDataT& defaultData);

        // Join two nodes
        Component& join(Component&& comp1, Component&& comp2);

    private:

        std::size_t _getRootIdxByIndex(std::size_t idx);

        RootDataT& _getRootDataByIndex(std::size_t idx);
        const RootDataT& _getRootDataByIndex(std::size_t idx) const;

    private:
        using DataVar = std::variant<RootDataT, SimpleDataT>;


    private:
        std::vector<std::size_t> _parents;
        std::vector<DataVar> _data;
        std::unordered_set<ComponentConstRef> _components;
    };
}

#endif //DSU_WITH_DATA_DSU_WITH_DATA_HPP

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template<class RootDataT, class SimpleDataT>
gdsu::DSUWithData<RootDataT, SimpleDataT>::DSUWithData(std::size_t size,
                                                       const RootDataT& defaultData)
  : _parents(size),
    _data(size, defaultData) {
}

//----------------------------------------------------------------------------//
template<class RootDataT, class SimpleDataT>
auto
gdsu::DSUWithData<RootDataT, SimpleDataT>::join(
        Component&& comp1, Component&& comp2) -> Component& {
    assert(comp1._owner == comp2._owner);

    std::size_t comp1RootIndex = _getRootIdxByIndex(comp1._someIndex);
    std::size_t comp2RootIndex = _getRootIdxByIndex(comp2._someIndex);

    auto& rootData1 = std::get<RootDataT>(_data[comp1RootIndex]);
    auto& rootData2 = std::get<RootDataT>(_data[comp2RootIndex]);

    if (comp1._size < comp2._size) {
        _parents[comp1RootIndex] = comp2RootIndex;
        comp1._someIndex = comp1RootIndex;
        comp1._size += comp2._size;

        SimpleDataT newComp1Data = rootData2.joinWith(std::move(rootData1));
        _data[comp1RootIndex] = std::move(newComp1Data);

        return comp2;
    } else {
        _parents[comp2RootIndex] = comp1RootIndex;
        comp2._someIndex = comp2RootIndex;
        comp2.size += comp2._size;

        SimpleDataT newComp2Data = rootData1.joinWith(std::move(rootData2));
        _data[comp2RootIndex] = std::move(newComp2Data);

        return comp1;
    }
}

//----------------------------------------------------------------------------//
template<class RootDataT, class SimpleDataT>
std::size_t
gdsu::DSUWithData<RootDataT, SimpleDataT>::_getRootIdxByIndex(std::size_t idx) {
    if(std::size_t parentIdx = _parents[idx]; parentIdx != idx) {
        // Rejoin
        _parents[idx] = _parents[parentIdx];
        return _getRootIdxByIndex(_parents[parentIdx]);
    }
    return idx;
}

//----------------------------------------------------------------------------//
template<class RootDataT, class SimpleDataT>
RootDataT&
gdsu::DSUWithData<RootDataT, SimpleDataT>::_getRootDataByIndex(
        std::size_t idx) {
    return std::get<RootDataT>(_data[idx]);
}

//----------------------------------------------------------------------------//
template<class RootDataT, class SimpleDataT>
const RootDataT&
gdsu::DSUWithData<RootDataT, SimpleDataT>::_getRootDataByIndex(
        std::size_t idx) const {
    return std::get<RootDataT>(_data[idx]);
}

//----------------------------------------------------------------------------//
template<class RootDataT, class SimpleDataT>
gdsu::DSUWithData<RootDataT, SimpleDataT>::Component::Component(
        OwnerT *ownerPtr, std::size_t idx, std::size_t size)
        : _owner(ownerPtr), _someIndex(idx), _size(size) {}

//----------------------------------------------------------------------------//
template<class RootDataT, class SimpleDataT>
RootDataT&
gdsu::DSUWithData<RootDataT, SimpleDataT>::Component::getRootData() {
    _someIndex = _owner->_getRootIdxByIndex(_someIndex);
    return _owner->_getRootDataByIndex(_someIndex);
}

//----------------------------------------------------------------------------//
template<class RootDataT, class SimpleDataT>
const RootDataT&
gdsu::DSUWithData<RootDataT, SimpleDataT>::Component::getRootData() const {
    _someIndex = _owner->_getRootIdxByIndex(_someIndex);
    return _owner->_getRootDataByIndex(_someIndex);
}
