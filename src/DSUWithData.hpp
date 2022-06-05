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
#include <stdexcept>
#include <cassert>

#include "DefaultDSUData.hpp"

namespace gdsu {

    ////////////////////////////////////////////////////////////////////////////
    // class DSUWithData<KeyT, RootDataT, SimpleDataT, Comp>
    template<class KeyT,
             class RootDataT = BaseRootDSUData<KeyT>,
             class SimpleDataT = BaseSimpleDSUData<KeyT>,
             class Comp = std::less<KeyT>>
    class DSUWithData {
    private:

        struct KeyPtrComp {
            bool operator()(const KeyT* const key1, const KeyT* const key2) const {
                return Comp()(*key1, *key2);
            }
        };

    private:

        ////////////////////////////////////////////////////////////////////////
        // class src<RootDataT, SimpleDataT>::Component
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
            // TODO: to be erased somehow.
            Component();

            // Copy constructor
            // @param other - component to copy from.
            Component(const Component& other);

            // Move constructor
            // @param other - component to move from.
            Component(Component&& other) noexcept;

            // Get component size.
            // @return size of the component.
            std::size_t getSize() const;

        private:

            // Move assignment.
            Component& operator=(Component&& other) noexcept = default;

        private:

            mutable std::size_t _rootIndex;  // Implementation index of the root
            std::size_t _size;               // Size of the component

        private:
            friend class DSUWithData<KeyT, RootDataT, SimpleDataT>;
        };

    public:

        // DSU constructor from keys
        // @param keys - keys list
        DSUWithData(std::initializer_list<KeyT> keys);

        // DSU constructor from root data objects
        // @param rootData - list of rootData
        DSUWithData(std::initializer_list<RootDataT> rootData);

        // DSU constructor from container with keys objects by pair of iterators
        // @param begin - range beginning
        // @param end - range ending (position after last element)
        template<class IteratorT>
        requires std::is_same_v<
                     typename std::iterator_traits<IteratorT>::value_type,
                     KeyT
                 >
        DSUWithData(IteratorT begin, IteratorT end);

        // DSU constructor from container with rootDataObjects by pair of
        // iterators
        // @param begin - range beginning
        // @param end - range ending (position after last element)
        template<class IteratorT>
        requires std::is_same_v<
                     typename std::iterator_traits<IteratorT>::value_type,
                     RootDataT
                 >
        DSUWithData(IteratorT begin, IteratorT end);

        // DSU constructor from keys set by pair of iterators
        // @param begin - range beginning
        // @param end - range ending (position after last element)
        DSUWithData(typename std::set<KeyT, Comp>::iterator begin,
                    typename std::set<KeyT, Comp>::iterator end);

        // Join two components by keys
        void joinByKeys(const KeyT& key1, const KeyT& key);

        // Check if two keys are of the same component
        // @param key1 - firstKey
        // @param key2 - secondKey
        bool inSameComponent(const KeyT& key1, const KeyT& key2) const;

        // Check if two root data objects wrap keys from the same component
        // @param rootData1 - first root data object
        // @param rootData2 - second root data object
        bool inSameComponent(const RootDataT& rootData1,
                             const RootDataT& rootData2) const;

        // Get component size
        // @param key - key
        std::size_t getComponentSize(const KeyT& key) const;

        // Get number of components.
        // @return number of components.
        [[nodiscard]] std::size_t getNumberOfComponents() const;

        // Root data by key.
        // @param key to search.
        const RootDataT& getRootData(const KeyT& key);

    private:

        // Fill parents relations data
        void _postConstruct();

        // Root index by element index
        std::size_t _getRootIdxByIndex(std::size_t idx) const;

        RootDataT& _getRootDataByIndex(std::size_t idx);

        Component& _getComponent(const KeyT& key);

        const Component& _getComponent(const KeyT& key) const;

        // Join two components
        // @param comp1 - first component
        // @param comp2 - second component
        // @return - component of a join result.
        Component& _join(Component&& comp1, Component&& comp2);

    private:
        using DataVar = std::variant<RootDataT, SimpleDataT>;

    private:
        // Parents relations info
        mutable std::vector<std::size_t> _parents;
        // Vector with data objects
        std::vector<DataVar> _data;
        // Map with components
        mutable std::unordered_map<std::size_t, Component> _rootIdxToComponent;
        // Key to index mapping
        mutable std::map<KeyT, std::size_t, Comp> _keyToIndex;
    };
}

#endif //DSU_WITH_DATA_DSU_WITH_DATA_HPP

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template<class KeyT, class RootDataT, class SimpleDataT, class Comp>
gdsu::DSUWithData<KeyT, RootDataT, SimpleDataT, Comp>::DSUWithData(
        std::initializer_list<KeyT> keys) {
    auto addedKeys = std::set<const KeyT*, KeyPtrComp>();
    for (const auto& key : keys) {
        if (!addedKeys.contains(&key)) {
            _data.push_back(RootDataT(key));
            addedKeys.insert(&key);
        }
    }
    _postConstruct();
}

//----------------------------------------------------------------------------//
template<class KeyT, class RootDataT, class SimpleDataT, class Comp>
gdsu::DSUWithData<KeyT, RootDataT, SimpleDataT, Comp>::DSUWithData(
        std::initializer_list<RootDataT> rootData) {
    auto addedKeys = std::set<const KeyT*, KeyPtrComp>();
    for (const auto& rootDataI : rootData) {
        if (const auto& newKey = rootDataI.getKey();
        addedKeys.contains(&newKey)) {
            throw std::invalid_argument("Root data keys must not repeat.");
        } else {
            _data.push_back(rootDataI);
            addedKeys.insert(&newKey);
        }
    }
    _postConstruct();
}

//----------------------------------------------------------------------------//
template<class KeyT, class RootDataT, class SimpleDataT, class Comp>
template<class IteratorT>
requires std::is_same_v<
             typename std::iterator_traits<IteratorT>::value_type,
             KeyT
         >
gdsu::DSUWithData<KeyT, RootDataT, SimpleDataT, Comp>::DSUWithData(
        IteratorT begin, IteratorT end) {
    auto addedKeys = std::set<const KeyT*, KeyPtrComp>();
    for (auto keyIt = begin; keyIt != end; ++keyIt) {
        if (const KeyT& keyRef = *keyIt;
        !addedKeys.contains(&keyRef)) {
            _data.push_back(RootDataT(keyRef));
            addedKeys.insert(&keyRef);
        }
    }
    _postConstruct();
}

//----------------------------------------------------------------------------//
template<class KeyT, class RootDataT, class SimpleDataT, class Comp>
template<class IteratorT>
requires std::is_same_v<
             typename std::iterator_traits<IteratorT>::value_type,
             RootDataT
         >
gdsu::DSUWithData<KeyT, RootDataT, SimpleDataT, Comp>::DSUWithData(
        IteratorT begin, IteratorT end)
        : _parents(end - begin) {
    auto addedKeys = std::set<const KeyT*, KeyPtrComp>();
    for (auto rootDataIt = begin; rootDataIt != end; ++rootDataIt) {
        const KeyT& keyRef = rootDataIt->getKey();
        if (addedKeys.contains(&keyRef)) {
            throw std::invalid_argument("Root data keys must not repeat.");
        }
        _data.push_back(*rootDataIt);
        addedKeys.insert(&keyRef);
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
    _postConstruct();
}

//----------------------------------------------------------------------------//
template<class KeyT, class RootDataT, class SimpleDataT, class Comp>
auto
gdsu::DSUWithData<KeyT, RootDataT, SimpleDataT, Comp>::_join(
        Component&& comp1, Component&& comp2) -> Component& {
    if (&comp1 == &comp2) {
        return comp1;
    }

    Component&& smallerComponent =
            std::move((comp1._size < comp2._size) ? comp1 : comp2);
    Component&& biggerComponent =
            std::move((comp1._size < comp2._size) ? comp2 : comp1);

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
    _rootIdxToComponent[biggerComponent._rootIndex] =
            std::move(biggerComponent);

    _rootIdxToComponent[biggerComponent._rootIndex];
}

//----------------------------------------------------------------------------//
template<class KeyT, class RootDataT, class SimpleDataT, class Comp>
void
gdsu::DSUWithData<KeyT, RootDataT, SimpleDataT, Comp>::joinByKeys(
        const KeyT &key1,
        const KeyT &key2) {
    const std::size_t rootIdx1 = _getRootIdxByIndex(_keyToIndex[key1]);
    const std::size_t rootIdx2 = _getRootIdxByIndex(_keyToIndex[key2]);

    if (rootIdx1 == rootIdx2) {
        return;
    }

    Component& comp1 = _rootIdxToComponent[rootIdx1];
    Component& comp2 = _rootIdxToComponent[rootIdx2];

    _join(std::move(comp1), std::move(comp2));
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
bool gdsu::DSUWithData<KeyT, RootDataT, SimpleDataT, Comp>::inSameComponent(
        const RootDataT& rootData1, const RootDataT& rootData2) const {
    const std::size_t root1Idx =
            _getRootIdxByIndex(_keyToIndex[rootData1.getKey()]);
    const std::size_t root2Idx =
            _getRootIdxByIndex(_keyToIndex[rootData2.getKey()]);

    return root1Idx == root2Idx;
}

//----------------------------------------------------------------------------//
template<class KeyT, class RootDataT, class SimpleDataT, class Comp>
auto
gdsu::DSUWithData<KeyT, RootDataT, SimpleDataT, Comp>::_getComponent(
        const KeyT& key) -> Component& {
    std::size_t index = _keyToIndex[key];
    std::size_t rootIndex = _getRootIdxByIndex(index);

    return _rootIdxToComponent[rootIndex];
}

//----------------------------------------------------------------------------//
template<class KeyT, class RootDataT, class SimpleDataT, class Comp>
std::size_t
gdsu::DSUWithData<KeyT, RootDataT, SimpleDataT, Comp>::getNumberOfComponents(
        ) const {
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
std::size_t
gdsu::DSUWithData<KeyT, RootDataT, SimpleDataT, Comp>::_getRootIdxByIndex(
        std::size_t idx) const {
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
void gdsu::DSUWithData<KeyT, RootDataT, SimpleDataT, Comp>::_postConstruct() {
    _parents.resize(_data.size());
    for (std::size_t i = 0; i < _parents.size(); ++i) {
        _parents[i] = i;
    }
    for (std::size_t i = 0; i < _data.size(); ++i) {
        _rootIdxToComponent.emplace(std::make_pair(i, Component(this, i, 1)));
        _keyToIndex[std::get<RootDataT>(_data[i]).getKey()] = i;
    }
}

template<class KeyT, class RootDataT, class SimpleDataT, class Comp>
std::size_t
gdsu::DSUWithData<KeyT, RootDataT, SimpleDataT, Comp>::getComponentSize(const KeyT& key) const {
    return _getComponent(key).getSize();
}

template<class KeyT, class RootDataT, class SimpleDataT, class Comp>
auto
gdsu::DSUWithData<KeyT, RootDataT, SimpleDataT, Comp>::_getComponent(
        const KeyT &key) const -> const Component& {
    std::size_t index = _keyToIndex[key];
    std::size_t rootIndex = _getRootIdxByIndex(index);

    return _rootIdxToComponent[rootIndex];
}

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template<class KeyT, class RootDataT, class SimpleDataT, class Comp>
gdsu::DSUWithData<KeyT, RootDataT, SimpleDataT, Comp>::Component::Component(
        OwnerT *ownerPtr, std::size_t rootIdx, std::size_t size)
        : _rootIndex(rootIdx), _size(size) {}

//----------------------------------------------------------------------------//
template<class KeyT, class RootDataT, class SimpleDataT, class Comp>
gdsu::DSUWithData<KeyT, RootDataT, SimpleDataT, Comp>::Component::Component(
        Component &&other) noexcept
        : _rootIndex(other._rootIndex),
          _size(other._size) { }

//----------------------------------------------------------------------------//
template<class KeyT, class RootDataT, class SimpleDataT, class Comp>
gdsu::DSUWithData<KeyT, RootDataT, SimpleDataT, Comp>::Component::Component(
        const Component& other)
        : _rootIndex(other._rootIndex),
          _size(other._size) { }

//----------------------------------------------------------------------------//
template<class KeyT, class RootDataT, class SimpleDataT, class Comp>
gdsu::DSUWithData<KeyT, RootDataT, SimpleDataT, Comp>::Component::Component()
        : _rootIndex{}, _size{} {
    assert(false);
}

//----------------------------------------------------------------------------//
template<class KeyT, class RootDataT, class SimpleDataT, class Comp>
std::size_t
gdsu::DSUWithData<KeyT, RootDataT, SimpleDataT, Comp>::Component::getSize(
        ) const {
    return _size;
}
