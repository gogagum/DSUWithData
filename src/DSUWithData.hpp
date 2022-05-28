//
// Created by gogagum on 18.04.22.
//

#ifndef DSU_WITH_DATA_DSU_WITH_DATA_HPP
#define DSU_WITH_DATA_DSU_WITH_DATA_HPP

#include <cstdint>
#include <vector>
#include <variant>
#include <map>
#include <unordered_set>
#include <cassert>

namespace gdsu {

    ////////////////////////////////////////////////////////////////////////////
    // class DSUWithData<RootDataT, SimpleDataT>
    template<class KeyT,
             template<class KeyT_> class RootDataT,
             template<class KeyT_> class SimpleDataT>
    class DSUWithData {
    public:

        ////////////////////////////////////////////////////////////////////////
        // class DSUWithData<RootDataT, SimpleDataT>::Component
        class Component{
        public:
            struct Hash {
            public:
                std::size_t operator() (const Component& comp) const {
                    return comp._someIndex;
                }
            };
            using OwnerT = DSUWithData<KeyT, RootDataT, SimpleDataT>;
        private:
            // Constructor
            // @param ownerPtr - DSU, in which component lays.
            // @param idx - implementation index of the component.
            // @param size - number of elements in the component.
            Component(OwnerT* ownerPtr, std::size_t idx, std::size_t size);
        public:
            // Get root data from component.
            // @return root data
            RootDataT<KeyT>& getRootData();

            // Get root data from component.
            // @return root data.
            const RootDataT<KeyT>& getRootData() const;

            bool operator==(const Component& other) const;

        private:
            OwnerT* _owner;
            mutable std::size_t _someIndex;
            std::size_t _size;

            friend class DSUWithData<KeyT, RootDataT, SimpleDataT>;
        };

        ////////////////////////////////////////////////////////////////////////
        // class DSUWithData<RootDataT, SimpleDataT>::ElementPresenter
        class ElementPresenter {
        public:
            using OwnerT = DSUWithData<KeyT, RootDataT, SimpleDataT>;

        private:
            ElementPresenter(OwnerT* owner, std::size_t index);

        private:
            OwnerT* _owner;
            std::size_t _index;
        };

        using ComponentRef = std::reference_wrapper<Component>;
        using ComponentConstRef = std::reference_wrapper<const Component>;

    public:
        // Constructor from data
        DSUWithData(std::initializer_list<RootDataT<KeyT>> data);

        // Join two nodes
        Component& join(Component&& comp1, Component&& comp2);

        std::vector<ElementPresenter> getPresenters() const;

    private:

        std::size_t _getRootIdxByIndex(std::size_t idx);

        RootDataT<KeyT>& _getRootDataByIndex(std::size_t idx);

        const RootDataT<KeyT>& _getRootDataByIndex(std::size_t idx) const;

    private:
        using DataVar = std::variant<RootDataT<KeyT>, SimpleDataT<KeyT>>;

    private:
        std::vector<std::size_t> _parents;
        std::vector<DataVar> _data;
        std::unordered_set<Component, typename Component::Hash> _components;
        std::map<KeyT, std::size_t> _keyToIndex;
    };
}

#endif //DSU_WITH_DATA_DSU_WITH_DATA_HPP

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template<class KeyT,
         template<class> class RootDataT,
         template<class> class SimpleDataT>
gdsu::DSUWithData<KeyT, RootDataT, SimpleDataT>::DSUWithData(
    std::initializer_list<RootDataT<KeyT>> data)
    : _parents(data.size()),
      _data(data.begin(), data.end()) {
    for (std::size_t i = 0; i < _parents.size(); ++i) {
        _parents[i] = i;
    }
    for (std::size_t i = 0; i < _data.size(); ++i) {
        auto getKey = [](const auto& data) {
            return data.getKey();
        };

        _keyToIndex[std::visit(getKey, _data[i])] = i;
        _components.insert(Component(this, i, 1));
    }
}

//----------------------------------------------------------------------------//
template<class KeyT,
         template<class> class RootDataT,
         template<class> class SimpleDataT>
auto
gdsu::DSUWithData<KeyT, RootDataT, SimpleDataT>::join(
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
template<class KeyT,
         template<class> class RootDataT,
         template<class> class SimpleDataT>
auto
gdsu::DSUWithData<KeyT, RootDataT, SimpleDataT>::getPresenters() const
-> std::vector<ElementPresenter> {
    std::vector<ElementPresenter> ret;
    for (std::size_t i = 0; i < _data.size(); ++i) {
        ret.push_back(ElementPresenter(this, i));
    }
}

//----------------------------------------------------------------------------//
template<class KeyT,
         template<class> class RootDataT,
         template<class> class SimpleDataT>
std::size_t
gdsu::DSUWithData<KeyT, RootDataT, SimpleDataT>::_getRootIdxByIndex(std::size_t idx) {
    if(std::size_t parentIdx = _parents[idx]; parentIdx != idx) {
        // Rejoin
        _parents[idx] = _parents[parentIdx];
        return _getRootIdxByIndex(_parents[parentIdx]);
    }
    return idx;
}

//----------------------------------------------------------------------------//
template<class KeyT,
        template<class> class RootDataT,
        template<class> class SimpleDataT>
RootDataT<KeyT>&
gdsu::DSUWithData<KeyT, RootDataT, SimpleDataT>::_getRootDataByIndex(
        std::size_t idx) {
    return std::get<RootDataT>(_data[idx]);
}

//----------------------------------------------------------------------------//
template<class KeyT,
         template<class> class RootDataT,
         template<class> class SimpleDataT>
const RootDataT<KeyT>&
gdsu::DSUWithData<KeyT, RootDataT, SimpleDataT>::_getRootDataByIndex(
        std::size_t idx) const {
    return std::get<RootDataT>(_data[idx]);
}

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template<class KeyT,
         template<class> class RootDataT,
         template<class> class SimpleDataT>
gdsu::DSUWithData<KeyT, RootDataT, SimpleDataT>::Component::Component(
        OwnerT *ownerPtr, std::size_t idx, std::size_t size)
        : _owner(ownerPtr), _someIndex(idx), _size(size) {}

//----------------------------------------------------------------------------//
template<class KeyT,
         template<class> class RootDataT,
         template<class> class SimpleDataT>
RootDataT<KeyT>&
gdsu::DSUWithData<KeyT, RootDataT, SimpleDataT>::Component::getRootData() {
    _someIndex = _owner->_getRootIdxByIndex(_someIndex);
    return _owner->_getRootDataByIndex(_someIndex);
}

//----------------------------------------------------------------------------//
template<class KeyT,
        template<class> class RootDataT,
        template<class> class SimpleDataT>
const RootDataT<KeyT>&
gdsu::DSUWithData<KeyT, RootDataT, SimpleDataT>::Component::getRootData() const {
    _someIndex = _owner->_getRootIdxByIndex(_someIndex);
    return _owner->_getRootDataByIndex(_someIndex);
}

//----------------------------------------------------------------------------//
template<class KeyT,
         template<class> class RootDataT,
         template<class> class SimpleDataT>
bool
gdsu::DSUWithData<KeyT, RootDataT, SimpleDataT>::Component::operator==(
        const Component &other) const {
    const auto rootIdxThis = _owner->_getRootIdxByIndex(_someIndex);
    const auto rootIdxOther = _owner->_getRootIdxByIndex(other._someIndex);

    return rootIdxOther == rootIdxThis;
}

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template<class KeyT,
         template<class> class RootDataT,
         template<class> class SimpleDataT>
gdsu::DSUWithData<KeyT, RootDataT, SimpleDataT>::ElementPresenter::ElementPresenter(
        OwnerT* owner, std::size_t index) : _owner(owner), _index(index) {}