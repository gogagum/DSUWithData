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

#include "DefaultDSUData.hpp"

namespace gdsu {

    ////////////////////////////////////////////////////////////////////////////
    // class DSUWithData<RootDataT, SimpleDataT>
    template<class KeyT,
             class RootDataT = BaseRootDSUData<KeyT>,
             class SimpleDataT = BaseSimpleDSUData<KeyT>,
             class Comp = std::less<KeyT>>
    class DSUWithData {
    public:

        ////////////////////////////////////////////////////////////////////////
        // class DSUWithData<RootDataT, SimpleDataT>::Component
        class Component{
        public:
            using OwnerT = DSUWithData<KeyT, RootDataT, SimpleDataT>;
        private:
            // Constructor
            // @param ownerPtr - DSU, in which component lays.
            // @param idx - implementation index of the component.
            // @param size - number of elements in the component.
            Component(OwnerT* ownerPtr,
                      std::size_t rootIdx,
                      std::size_t size);


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
            // @return root data.
            const RootDataT& getRootData() const;

            // Get component size.
            // @return size of the component.
            std::size_t getSize() const;

        private:

            // Move assignment.
            Component& operator=(Component&& other) noexcept;

        private:

            OwnerT* _owner;                  // Owner pointer
            mutable std::size_t _rootIndex;  // Implementation index of the root
            std::size_t _size;               // Size of the component

        private:
            friend class DSUWithData<KeyT, RootDataT, SimpleDataT>;
        };

    public:

        DSUWithData(std::initializer_list<KeyT> keys);

        DSUWithData(std::initializer_list<RootDataT> rootData);

        template<class IteratorT>
                requires std::is_same_v<
                             typename std::iterator_traits<IteratorT>::value_type,
                             KeyT>
        DSUWithData(IteratorT begin, IteratorT end);

        template<class IteratorT>
                requires std::is_same_v<
                        typename std::iterator_traits<IteratorT>::value_type,
                        RootDataT>
        DSUWithData(IteratorT begin, IteratorT end);

        DSUWithData(typename std::set<KeyT, Comp>::iterator begin,
                    typename std::set<KeyT, Comp>::iterator end);

        // Join two components
        Component& join(Component&& comp1, Component&& comp2);

        // Join two components by keys
        void joinByKeys(const KeyT& key1, const KeyT& key);

        bool inSameComponent(const KeyT& key1, const KeyT& key2) const;

        // Get component.
        // @param key - search key.
        // @return component of the key.
        Component& getComponent(const KeyT& key);

        // Get number of components.
        // @return number of components.
        [[nodiscard]] std::size_t getNumberOfComponents() const;

        // Root data by key.
        // @param key to search.
        const RootDataT& getRootData(const KeyT& key);

    private:

        void _postConstruct();

        std::size_t _getRootIdxByIndex(std::size_t idx) const;

        RootDataT& _getRootDataByIndex(std::size_t idx);

    private:
        using DataVar = std::variant<RootDataT, SimpleDataT>;

    private:
        mutable std::vector<std::size_t> _parents;
        std::vector<DataVar> _data;
        std::map<std::size_t, Component> _rootIdxToComponent;
        mutable std::map<KeyT, std::size_t, Comp> _keyToIndex;
    };
}

#endif //DSU_WITH_DATA_DSU_WITH_DATA_HPP

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template<class KeyT, class RootDataT, class SimpleDataT, class Comp>
gdsu::DSUWithData<KeyT, RootDataT, SimpleDataT, Comp>::DSUWithData(
    std::initializer_list<KeyT> keys)
        : _parents(keys.size()) {
    for (const auto& key : keys) {
        _data.push_back(RootDataT(key));
    }
    _postConstruct();
}

//----------------------------------------------------------------------------//
template<class KeyT, class RootDataT, class SimpleDataT, class Comp>
gdsu::DSUWithData<KeyT, RootDataT, SimpleDataT, Comp>::DSUWithData(
        std::initializer_list<RootDataT> rootData)
        : _parents(rootData.size()) {
    for (const auto& rootDataI : rootData) {
        _data.push_back(rootDataI);
    }
    _postConstruct();
}

//----------------------------------------------------------------------------//
template<class KeyT, class RootDataT, class SimpleDataT, class Comp>
template<class IteratorT>
requires std::is_same_v<
        typename std::iterator_traits<IteratorT>::value_type,
        KeyT>
gdsu::DSUWithData<KeyT, RootDataT, SimpleDataT, Comp>::DSUWithData(
        IteratorT begin, IteratorT end) {
    std::set<KeyT, Comp> addedKeys;
    for (auto keyIt = begin; keyIt != end; ++keyIt) {
        if (addedKeys.find(*keyIt) == addedKeys.end()) {
            _data.push_back(RootDataT(*keyIt));
            addedKeys.insert(*keyIt);
        }
    }
    _parents = std::vector<std::size_t>(addedKeys.size());
    _postConstruct();
}

//----------------------------------------------------------------------------//
template<class KeyT, class RootDataT, class SimpleDataT, class Comp>
template<class IteratorT>
requires std::is_same_v<
        typename std::iterator_traits<IteratorT>::value_type,
        RootDataT>
gdsu::DSUWithData<KeyT, RootDataT, SimpleDataT, Comp>::DSUWithData(
        IteratorT begin, IteratorT end)
        : _parents(end - begin) {
    for (auto rootDataIt = begin; rootDataIt != end; ++rootDataIt) {
        _data.push_back(*rootDataIt);
    }
    _postConstruct();
}

//----------------------------------------------------------------------------//
template<class KeyT, class RootDataT, class SimpleDataT, class Comp>
gdsu::DSUWithData<KeyT, RootDataT, SimpleDataT, Comp>::DSUWithData(
        typename std::set<KeyT, Comp>::iterator begin,
        typename std::set<KeyT, Comp>::iterator end) {
    for (auto keyIt = begin; keyIt != end; ++keyIt) {
        _data.push_back(RootDataT(*keyIt));
    }
    _parents = std::vector<std::size_t>(_data.size());
    _postConstruct();
}

//----------------------------------------------------------------------------//
template<class KeyT, class RootDataT, class SimpleDataT, class Comp>
auto
gdsu::DSUWithData<KeyT, RootDataT, SimpleDataT, Comp>::join(
        Component&& comp1, Component&& comp2) -> Component& {
    assert(comp1._owner == comp2._owner);

    auto& smallerComponent = (comp1._size < comp2._size) ? comp1 : comp2;
    auto& biggerComponent = (comp1._size < comp2._size) ? comp2 : comp1;

    _parents[smallerComponent._rootIndex] = biggerComponent._rootIndex;
    biggerComponent._size += smallerComponent._size;

    auto& biggerComponentData =
            std::get<RootDataT>(_data[biggerComponent._rootIndex]);
    auto& smallerComponentData =
            std::get<RootDataT>(_data[smallerComponent._rootIndex]);

    SimpleDataT newSmallerComponentData =
            biggerComponentData.joinWith(std::move(smallerComponentData));

    _data[smallerComponent._rootIndex] = std::move(newSmallerComponentData);
    _rootIdxToComponent.erase(smallerComponent._rootIndex);
    _rootIdxToComponent[biggerComponent._rootIndex] = std::move(biggerComponent);

    return _rootIdxToComponent[biggerComponent._rootIndex];
}

//----------------------------------------------------------------------------//
template<class KeyT, class RootDataT, class SimpleDataT, class Comp>
void
gdsu::DSUWithData<KeyT, RootDataT, SimpleDataT, Comp>::joinByKeys(const KeyT &key1,
                                                            const KeyT &key2) {
    std::size_t root1 = _getRootIdxByIndex(_keyToIndex[key1]);
    std::size_t root2 = _getRootIdxByIndex(_keyToIndex[key2]);

    auto& comp1 = _rootIdxToComponent[root1];
    auto& comp2 = _rootIdxToComponent[root2];

    join(std::move(comp1), std::move(comp2));
}

//----------------------------------------------------------------------------//
template<class KeyT, class RootDataT, class SimpleDataT, class Comp>
bool gdsu::DSUWithData<KeyT, RootDataT, SimpleDataT, Comp>::inSameComponent(
        const KeyT& key1, const KeyT& key2) const {
    const std::size_t root1Idx = _getRootIdxByIndex(_keyToIndex[key1]);
    const std::size_t root2Idx = _getRootIdxByIndex(_keyToIndex[key2]);

    return root1Idx == root2Idx;
}

//----------------------------------------------------------------------------//
template<class KeyT, class RootDataT, class SimpleDataT, class Comp>
std::size_t
gdsu::DSUWithData<KeyT, RootDataT, SimpleDataT, Comp>::_getRootIdxByIndex(std::size_t idx) const {
    if(std::size_t parentIdx = _parents[idx]; parentIdx != idx) {
        // Rejoin
        _parents[idx] = _parents[parentIdx];
        return _getRootIdxByIndex(_parents[parentIdx]);
    }
    return idx;
}

//----------------------------------------------------------------------------//
template<class KeyT, class RootDataT, class SimpleDataT, class Comp>
RootDataT&
gdsu::DSUWithData<KeyT, RootDataT, SimpleDataT, Comp>::_getRootDataByIndex(
        std::size_t idx) {
    std::size_t rootIndex = _getRootIdxByIndex(idx);
    return std::get<RootDataT>(_data[rootIndex]);
}

//----------------------------------------------------------------------------//
template<class KeyT, class RootDataT, class SimpleDataT, class Comp>
auto
gdsu::DSUWithData<KeyT, RootDataT, SimpleDataT, Comp>::getComponent(
        const KeyT& key) -> Component& {
    std::size_t index = _keyToIndex[key];
    std::size_t rootIndex = _getRootIdxByIndex(index);

    return _rootIdxToComponent[rootIndex];
}

//----------------------------------------------------------------------------//
template<class KeyT, class RootDataT, class SimpleDataT, class Comp>
std::size_t
gdsu::DSUWithData<KeyT, RootDataT, SimpleDataT, Comp>::getNumberOfComponents() const {
    return _rootIdxToComponent.size();
}

//----------------------------------------------------------------------------//
template<class KeyT, class RootDataT, class SimpleDataT, class Comp>
const RootDataT&
gdsu::DSUWithData<KeyT, RootDataT, SimpleDataT, Comp>::getRootData(
        const KeyT& key) {
    return _getRootDataByIndex(_keyToIndex[key]);
}

//----------------------------------------------------------------------------//
template<class KeyT, class RootDataT, class SimpleDataT, class Comp>
void gdsu::DSUWithData<KeyT, RootDataT, SimpleDataT, Comp>::_postConstruct() {
    for (std::size_t i = 0; i < _parents.size(); ++i) {
        _parents[i] = i;
    }

    for (std::size_t i = 0; i < _data.size(); ++i) {
        _rootIdxToComponent.template emplace(std::make_pair(i, Component(this, i, 1)));
        _keyToIndex[std::get<RootDataT>(_data[i]).getKey()] = i;
    }
}

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template<class KeyT, class RootDataT, class SimpleDataT, class Comp>
gdsu::DSUWithData<KeyT, RootDataT, SimpleDataT, Comp>::Component::Component(
        OwnerT *ownerPtr, std::size_t rootIdx, std::size_t size)
        : _owner(ownerPtr), _rootIndex(rootIdx), _size(size) {}

//----------------------------------------------------------------------------//
template<class KeyT, class RootDataT, class SimpleDataT, class Comp>
gdsu::DSUWithData<KeyT, RootDataT, SimpleDataT, Comp>::Component::Component(
        Component &&other) noexcept
        : _rootIndex(other._rootIndex),
          _owner(other._owner),
          _size(other._size) { }

//----------------------------------------------------------------------------//
template<class KeyT, class RootDataT, class SimpleDataT, class Comp>
gdsu::DSUWithData<KeyT, RootDataT, SimpleDataT, Comp>::Component::Component(
        const Component& other)
        : _rootIndex(other._rootIndex),
          _owner(other._owner),
          _size(other._size) { }

//----------------------------------------------------------------------------//
template<class KeyT, class RootDataT, class SimpleDataT, class Comp>
const RootDataT&
gdsu::DSUWithData<KeyT, RootDataT, SimpleDataT, Comp>::Component::getRootData() const {
    _rootIndex = _owner->_getRootIdxByIndex(_rootIndex);
    return _owner->getRootData(_rootIndex);
}

//----------------------------------------------------------------------------//
template<class KeyT, class RootDataT, class SimpleDataT, class Comp>
auto
gdsu::DSUWithData<KeyT, RootDataT, SimpleDataT, Comp>::Component::operator=(
        Component &&other)  noexcept -> Component& {
    _size = other._size;
    _owner = other._owner;
    _rootIndex = other._rootIndex;

    return *this;
}

//----------------------------------------------------------------------------//
template<class KeyT, class RootDataT, class SimpleDataT, class Comp>
gdsu::DSUWithData<KeyT, RootDataT, SimpleDataT, Comp>::Component::Component()
        : _rootIndex{}, _size{} {
    assert(false);
}

//----------------------------------------------------------------------------//
template<class KeyT, class RootDataT, class SimpleDataT, class Comp>
std::size_t
gdsu::DSUWithData<KeyT, RootDataT, SimpleDataT, Comp>::Component::getSize() const {
    return _size;
}
