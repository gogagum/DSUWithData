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

            ////////////////////////////////////////////////////////////////////
            // struct DSUWithData<RootDataT, SimpleDataT>::Component::Hash
            // Component hash.
            struct Hash {
            public:
                std::size_t operator() (const Component& comp) const {
                    return comp._rootIndex;
                }
            };

            ////////////////////////////////////////////////////////////////////
            // struct DSUWithData<RootDataT, SimpleDataT>::Component::Eq
            // Components equality.
            struct Eq {
            public:
                bool operator() (const Component& comp1,
                                 const Component& comp2) const {
                    return comp1._rootIndex == comp2._rootIndex;
                }
            };

            using OwnerT = DSUWithData<KeyT, RootDataT, SimpleDataT>;
        private:
            // Constructor
            // @param ownerPtr - DSU, in which component lays.
            // @param idx - implementation index of the component.
            // @param size - number of elements in the component.
            Component(OwnerT* ownerPtr, std::size_t rootIdx, std::size_t size);


        public:

            // No argument constructor.
            // Must not be called.
            // TODOGG: to be erased somehow.
            Component();

            // Copy constructor
            // @param other - component to copy from.
            Component(const Component& other);

            // Move constructor
            // @param other - component to move from.
            Component(Component&& other) noexcept;

            // Get root data from component.
            // @return root data
            const RootDataT<KeyT>& getRootData();

            // Get root data from component.
            // @return root data.
            const RootDataT<KeyT>& getRootData() const;

            Component& operator=(Component&& other) noexcept;

            std::size_t getSize() const;

        private:
            OwnerT* _owner;
            mutable std::size_t _rootIndex{};
            std::size_t _size{};

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
        DSUWithData(std::initializer_list<KeyT> keys);

        // Join two nodes
        Component& join(Component&& comp1, Component&& comp2);

        std::vector<ElementPresenter> getPresenters() const;

        Component& getComponent(KeyT key);

        [[nodiscard]] std::size_t getNumberOfComponents() const;

        const RootDataT<KeyT>& getRootData(KeyT key) const;

    private:

        std::size_t _getRootIdxByIndex(std::size_t idx);

        RootDataT<KeyT>& _getRootDataByIndex(std::size_t idx);

        const RootDataT<KeyT>& _getRootDataByIndex(std::size_t idx) const;

    private:
        using DataVar = std::variant<RootDataT<KeyT>, SimpleDataT<KeyT>>;

    private:
        std::vector<std::size_t> _parents;
        std::vector<DataVar> _data;
        std::map<std::size_t, Component> _rootIdxToComponent;
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
    std::initializer_list<KeyT> keys)
    : _parents(keys.size()) {
    for (std::size_t i = 0; i < _parents.size(); ++i) {
        _parents[i] = i;
    }

    for (auto&& key : keys) {
        _data.push_back(RootDataT<KeyT>(std::move(key)));
    }

    for (std::size_t i = 0; i < _data.size(); ++i) {
        _keyToIndex[std::get<RootDataT<KeyT>>(_data[i]).getKey()] = i;
        _rootIdxToComponent.template emplace(std::make_pair(i, Component(this, i, 1)));
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

    auto& smallerComponent = (comp1._size < comp2._size) ? comp1 : comp2;
    auto& biggerComponent = (comp1._size < comp2._size) ? comp2 : comp1;

    _parents[smallerComponent._rootIndex] = biggerComponent._rootIndex;
    biggerComponent._size += smallerComponent._size;

    auto biggerComponentData = std::get<RootDataT<KeyT>>(_data[biggerComponent._rootIndex]);
    auto& smallerComponentData = std::get<RootDataT<KeyT>>(_data[smallerComponent._rootIndex]);

    SimpleDataT newSmallerComponentData = biggerComponentData.joinWith(std::move(smallerComponentData));

    _data[smallerComponent._rootIndex] = std::move(newSmallerComponentData);
    _rootIdxToComponent.erase(smallerComponent._rootIndex);
    _rootIdxToComponent.template emplace(
            std::make_pair(biggerComponent._rootIndex, std::move(biggerComponent)));

    return _rootIdxToComponent[biggerComponent._rootIndex];
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

//----------------------------------------------------------------------------//
template<class KeyT,
         template<class> class RootDataT,
         template<class> class SimpleDataT>
auto
gdsu::DSUWithData<KeyT, RootDataT, SimpleDataT>::getComponent(
        KeyT key) -> Component& {

    std::size_t index = _keyToIndex[key];
    std::size_t rootIndex = _getRootIdxByIndex(index);

    return _rootIdxToComponent[rootIndex];
}

//----------------------------------------------------------------------------//
template<class KeyT,
         template<class> class RootDataT,
         template<class> class SimpleDataT>
std::size_t
gdsu::DSUWithData<KeyT, RootDataT, SimpleDataT>::getNumberOfComponents() const {
    return _rootIdxToComponent.size();
}

//----------------------------------------------------------------------------//
template<class KeyT,
         template<class> class RootDataT,
         template<class> class SimpleDataT>
const RootDataT<KeyT>&
gdsu::DSUWithData<KeyT, RootDataT, SimpleDataT>::getRootData(KeyT key) const {
    return std::get<RootDataT<KeyT>>(_data[_keyToIndex[key]]);
}

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template<class KeyT,
         template<class> class RootDataT,
         template<class> class SimpleDataT>
gdsu::DSUWithData<KeyT, RootDataT, SimpleDataT>::Component::Component(
        OwnerT *ownerPtr, std::size_t rootIdx, std::size_t size)
        : _owner(ownerPtr), _rootIndex(rootIdx), _size(size) {}

//----------------------------------------------------------------------------//
template<class KeyT,
        template<class> class RootDataT,
        template<class> class SimpleDataT>
gdsu::DSUWithData<KeyT, RootDataT, SimpleDataT>::Component::Component(
        Component &&other) noexcept
        : _rootIndex(other._rootIndex),
          _owner(other._owner),
          _size(other._size) { }

//----------------------------------------------------------------------------//
template<class KeyT,
         template<class> class RootDataT,
         template<class> class SimpleDataT>
gdsu::DSUWithData<KeyT, RootDataT, SimpleDataT>::Component::Component(
        const Component& other)
        : _rootIndex(other._rootIndex),
          _owner(other._owner),
          _size(other._size) { }

//----------------------------------------------------------------------------//
template<class KeyT,
         template<class> class RootDataT,
         template<class> class SimpleDataT>
const RootDataT<KeyT>&
gdsu::DSUWithData<KeyT, RootDataT, SimpleDataT>::Component::getRootData() {
    _rootIndex = _owner->_getRootIdxByIndex(_rootIndex);
    return _owner->_getRootIdxByIndex(_rootIndex);
}

//----------------------------------------------------------------------------//
template<class KeyT,
        template<class> class RootDataT,
        template<class> class SimpleDataT>
const RootDataT<KeyT>&
gdsu::DSUWithData<KeyT, RootDataT, SimpleDataT>::Component::getRootData() const {
    _rootIndex = _owner->_getRootIdxByIndex(_rootIndex);
    return _owner->_getRootIdxByIndex(_rootIndex);
}

//----------------------------------------------------------------------------//
template<class KeyT,
         template<class> class RootDataT,
         template<class> class SimpleDataT>
auto
gdsu::DSUWithData<KeyT, RootDataT, SimpleDataT>::Component::operator=(
        Component &&other)  noexcept -> Component& {
    _size = other._size;
    _owner = other._owner;
    _rootIndex = other._rootIndex;

    return *this;
}

//----------------------------------------------------------------------------//
template<class KeyT,
         template<class> class RootDataT,
         template<class> class SimpleDataT>
gdsu::DSUWithData<KeyT, RootDataT, SimpleDataT>::Component::Component() {
    assert(false);
}

//----------------------------------------------------------------------------//
template<class KeyT,
         template<class> class RootDataT,
         template<class> class SimpleDataT>
std::size_t
gdsu::DSUWithData<KeyT, RootDataT, SimpleDataT>::Component::getSize() const {
    return _size;
}

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template<class KeyT,
         template<class> class RootDataT,
         template<class> class SimpleDataT>
gdsu::DSUWithData<KeyT, RootDataT, SimpleDataT>::ElementPresenter::ElementPresenter(
        OwnerT* owner, std::size_t index) : _owner(owner), _index(index) {}
