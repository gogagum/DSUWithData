//
// Created by gogagum on 18.04.22.
//

#ifndef DSU_WITH_DATA_DSU_WITH_DATA_HPP
#define DSU_WITH_DATA_DSU_WITH_DATA_HPP

#include <cstdint>
#include <vector>
#include <variant>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <stdexcept>
#include <cassert>
#include <numeric>

#include "DefaultDSUData.hpp"

namespace gdsu {

    /**
     * @brief class DSUWithData<KeyT, RootDataT, SimpleDataT, Comp>
     * @tparam KeyT
     * @tparam RootDataT
     * @tparam Comp
     */
    template<class KeyT,
             class RootDataT = BaseRootDSUData<KeyT>,
             class Comp = std::less<KeyT>>
    class DSUWithData {
    private:

        struct KeyPtrComp {
            bool operator()(const KeyT* const key1,
                            const KeyT* const key2) const {
                return Comp()(*key1, *key2);
            }
        };

    public:

        /**
         * DSU constructor from keys.
         * @param keys - keys list.
         */
        DSUWithData(std::initializer_list<KeyT> keys);

        /**
         * DSU constructor from root data objects
         * @param rootData - list of root data.
         */
        DSUWithData(std::initializer_list<RootDataT> rootData);

        /**
         * DSU constructor from container with keys objects by pair of
         * iterators.
         * @tparam IteratorT - iterator type.
         * @param begin - range beginning.
         * @param end - range ending (position after last element).
         */
        template<class IteratorT>
        requires std::is_same_v<
                     typename std::iterator_traits<IteratorT>::value_type,
                     KeyT
                 >
        DSUWithData(IteratorT begin, IteratorT end);

        /**
         * DSU constructor from container with root data objects from pair of
         * iterators.
         * @tparam IteratorT - iterator type.
         * @param begin - range beginning.
         * @param end - range ending (position after last element).
         */
        template<class IteratorT>
        requires std::is_same_v<
                     typename std::iterator_traits<IteratorT>::value_type,
                     RootDataT
                 >
        DSUWithData(IteratorT begin, IteratorT end);

        /**
         * DSU constructor from keys set by pair of iterators.
         * @param begin - range beginning.
         * @param end - range ending (position after last element).
         */
        DSUWithData(typename std::set<KeyT, Comp>::iterator begin,
                    typename std::set<KeyT, Comp>::iterator end);

        /**
         * DSU constructor from keys unordered_set by pair of iterators.
         * @param begin - range beginning.
         * @param end - range ending (position after last element).
         */
        DSUWithData(typename std::unordered_set<KeyT, Comp>::iterator begin,
                    typename std::unordered_set<KeyT, Comp>::iterator end);

        /**
         * Join two components by keys.
         * @param key1 - first key.
         * @param key2 - second key.
         */
        void join(const KeyT& key1, const KeyT& key2);

        /**
         * Check if two keys are of the same component.
         * @param key1 - first key.
         * @param key2 - second key.
         * @return `true if key1 and key2 are of the same component.
         */
        bool inSameComponent(const KeyT& key1, const KeyT& key2) const;

        /**
         * Get component size.
         * @param key - key.
         * @return number of elements in the same component as key.
         */
        std::size_t getComponentSize(const KeyT& key) const;

        /**
         * Get number of components in the DSU.
         * @return number of components.
         */
        [[nodiscard]] std::size_t getNumberOfComponents() const;

        /**
         * Root data by key.
         * @param key - key to search.
         * @return rootDataOfTheComponent.
         */
        const RootDataT& getRootData(const KeyT& key);

    private:

        /**
         * Fill parents relations data.
         */
        void _postConstruct();

        /**
         * Get implementation index by key.
         * @param key - key to get index.
         * @return index
         */
        std::size_t _getIdxByKey(const KeyT& key) const;

        /**
         * Root index by element index.
         * @param idx - index if element.
         * @return index of the root element of the component.
         */
        std::size_t _getRootIdxByIndex(std::size_t idx) const;

        /**
         * Root data by component element index.
         * @param idx - component element index.
         * @return root data.
         */
        RootDataT& _getRootDataByIndex(std::size_t idx);

        /**
         * Root data by component element index.
         * @param idx - component element index.
         * @return root data.
         */
        const RootDataT& _getRootDataByIndex(std::size_t idx) const;

        /**
         * Join two components.
         * @param comp1Idx - first component index.
         * @param comp2Idx - second component index.
         */
        void _join(std::size_t comp1Idx, std::size_t comp2Idx);

        /**
         * Root data by root index.
         * @param rootIdx - root index.
         * @return root data.
         */
        RootDataT& _getRootData(std::size_t rootIdx);

        /**
         * Root data by root index.
         * @param rootIdx - root index.
         * @return root data.
         */
        const RootDataT& _getRootData(std::size_t rootIdx) const;

    private:
        // Parents relations info
        mutable std::vector<std::size_t> _parents;
        // Vector with data objects
        std::unordered_map<std::size_t, RootDataT> _data;
        // Key to index mapping
        std::map<KeyT, std::size_t, Comp> _keyToIndex;
    };
}

#endif //DSU_WITH_DATA_DSU_WITH_DATA_HPP

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template<class KeyT, class RootDataT, class Comp>
gdsu::DSUWithData<KeyT, RootDataT, Comp>::DSUWithData(
        std::initializer_list<KeyT> keys) {
    auto addedKeys = std::set<const KeyT*, KeyPtrComp>();
    for (const auto& key : keys) {
        if (!addedKeys.contains(&key)) {
            _data.emplace(std::make_pair(addedKeys.size(), RootDataT(key)));
            addedKeys.insert(&key);
        }
    }
    _postConstruct();
}

//----------------------------------------------------------------------------//
template<class KeyT, class RootDataT, class Comp>
gdsu::DSUWithData<KeyT, RootDataT, Comp>::DSUWithData(
        std::initializer_list<RootDataT> rootData) {
    auto addedKeys = std::set<const KeyT*, KeyPtrComp>();
    for (const auto& rootDataI : rootData) {
        if (const auto& newKey = rootDataI.getKey();
        addedKeys.contains(&newKey)) {
            throw std::invalid_argument("Root data keys must not repeat.");
        } else {
            _data.emplace(
                    std::make_pair(addedKeys.size(), RootDataT(rootDataI)));
            addedKeys.insert(&newKey);
        }
    }
    _postConstruct();
}

//----------------------------------------------------------------------------//
template<class KeyT, class RootDataT, class Comp>
template<class IteratorT>
requires std::is_same_v<
             typename std::iterator_traits<IteratorT>::value_type,
             KeyT
         >
gdsu::DSUWithData<KeyT, RootDataT, Comp>::DSUWithData(
        IteratorT begin, IteratorT end) : _data() {
    auto addedKeys = std::set<const KeyT*, KeyPtrComp>();
    for (auto keyIt = begin; keyIt != end; ++keyIt) {
        if (const KeyT& keyRef = *keyIt;
        !addedKeys.contains(&keyRef)) {
            _data.emplace(
                    std::make_pair(addedKeys.size(), RootDataT(keyRef)));
            addedKeys.insert(&keyRef);
        }
    }
    _postConstruct();
}

//----------------------------------------------------------------------------//
template<class KeyT, class RootDataT, class Comp>
template<class IteratorT>
requires std::is_same_v<
             typename std::iterator_traits<IteratorT>::value_type,
             RootDataT
         >
gdsu::DSUWithData<KeyT, RootDataT, Comp>::DSUWithData(
        IteratorT begin, IteratorT end)
        : _parents(end - begin), _data() {
    auto addedKeys = std::set<const KeyT*, KeyPtrComp>();
    for (auto rootDataIt = begin; rootDataIt != end; ++rootDataIt) {
        const KeyT& keyRef = rootDataIt->getKey();
        if (addedKeys.contains(&keyRef)) {
            throw std::invalid_argument("Root data keys must not repeat.");
        }
        _data.emplace(std::make_pair(addedKeys.size(), RootDataT(*rootDataIt)));
        addedKeys.insert(&keyRef);
    }
    _postConstruct();
}

//----------------------------------------------------------------------------//
template<class KeyT, class RootDataT, class Comp>
gdsu::DSUWithData<KeyT, RootDataT, Comp>::DSUWithData(
        typename std::set<KeyT, Comp>::iterator begin,
        typename std::set<KeyT, Comp>::iterator end) : _data() {
    for (auto [keyIt, i] = std::make_pair(begin, 0ull);
         keyIt != end;
         ++keyIt, ++i) {
        _data.emplace(std::make_pair(i, RootDataT(*keyIt)));
    }
    _postConstruct();
}

//----------------------------------------------------------------------------//
template<class KeyT, class RootDataT, class Comp>
gdsu::DSUWithData<KeyT, RootDataT, Comp>::DSUWithData(
        typename std::unordered_set<KeyT, Comp>::iterator begin,
        typename std::unordered_set<KeyT, Comp>::iterator end) : _data() {
    for (auto [keyIt, i] = std::make_pair(begin, 0ull);
         keyIt != end;
         ++keyIt, ++i) {
        _data.emplace(std::make_pair(i, RootDataT(*keyIt)));
    }
    _postConstruct();
}

//----------------------------------------------------------------------------//
template<class KeyT, class RootDataT, class Comp>
void
gdsu::DSUWithData<KeyT, RootDataT, Comp>::_join(
        std::size_t rootIdx1, std::size_t rootIdx2) {

    std::size_t smallerComponentRootIdx;
    std::size_t biggerComponentRootIdx;
    RootDataT* smallerDataPtr;
    RootDataT* biggerDataPtr;

    if (auto& firstData = _getRootData(rootIdx1),
              secondData = _getRootData(rootIdx2);
    firstData.getSize() < secondData.getSize()) {
        smallerComponentRootIdx = rootIdx1;
        biggerComponentRootIdx = rootIdx2;
        smallerDataPtr = &firstData;
        biggerDataPtr = &secondData;
    } else {
        smallerComponentRootIdx = rootIdx2;
        biggerComponentRootIdx = rootIdx1;
        smallerDataPtr = &secondData;
        biggerDataPtr = &firstData;
    }

    _parents[smallerComponentRootIdx] = biggerComponentRootIdx;
    biggerDataPtr->joinWith(*smallerDataPtr);
    _data.erase(smallerComponentRootIdx);
}

//----------------------------------------------------------------------------//
template<class KeyT, class RootDataT, class Comp>
void
gdsu::DSUWithData<KeyT, RootDataT, Comp>::join(
        const KeyT &key1, const KeyT &key2) {
    const std::size_t rootIdx1 = _getRootIdxByIndex(_getIdxByKey(key1));
    const std::size_t rootIdx2 = _getRootIdxByIndex(_getIdxByKey(key2));

    if (rootIdx1 != rootIdx2) {
        _join(rootIdx1, rootIdx2);
    }
}

//----------------------------------------------------------------------------//
template<class KeyT, class RootDataT, class Comp>
bool gdsu::DSUWithData<KeyT, RootDataT, Comp>::inSameComponent(
        const KeyT& key1, const KeyT& key2) const {
    return _getRootIdxByIndex(_getIdxByKey(key1))
        == _getRootIdxByIndex(_getIdxByKey(key2));
}

//----------------------------------------------------------------------------//
template<class KeyT, class RootDataT, class Comp>
std::size_t
gdsu::DSUWithData<KeyT, RootDataT, Comp>::getNumberOfComponents() const {
    return _data.size();
}

//----------------------------------------------------------------------------//
template<class KeyT, class RootDataT, class Comp>
const RootDataT&
gdsu::DSUWithData<KeyT, RootDataT, Comp>::getRootData(const KeyT& key) {
    return _getRootDataByIndex(_getIdxByKey(key));
}

//----------------------------------------------------------------------------//
template<class KeyT, class RootDataT, class Comp>
std::size_t
gdsu::DSUWithData<KeyT, RootDataT, Comp>::_getRootIdxByIndex(
        std::size_t idx) const {
    if(std::size_t parentIdx = _parents[idx]; parentIdx != idx) {
        // Rejoin
        _parents[idx] = _parents[parentIdx];
        return _getRootIdxByIndex(_parents[parentIdx]);
    }
    return idx;
}

//----------------------------------------------------------------------------//
template<class KeyT, class RootDataT, class Comp>
auto
gdsu::DSUWithData<KeyT, RootDataT, Comp>::_getRootDataByIndex(
        std::size_t idx) -> RootDataT& {
    return _getRootData(_getRootIdxByIndex(idx));
}


//----------------------------------------------------------------------------//
template<class KeyT, class RootDataT, class Comp>
auto
gdsu::DSUWithData<KeyT, RootDataT, Comp>::_getRootDataByIndex(
        std::size_t idx) const -> const RootDataT& {
    return _getRootData(_getRootIdxByIndex(idx));
}


//----------------------------------------------------------------------------//
template<class KeyT, class RootDataT, class Comp>
void gdsu::DSUWithData<KeyT, RootDataT, Comp>::_postConstruct() {
    _parents.resize(_data.size());
    std::iota(_parents.begin(), _parents.end(), 0);
    for (auto& [i, rootData] : _data) {
        _keyToIndex[rootData.getKey()] = i;
    }
}

//----------------------------------------------------------------------------//
template<class KeyT, class RootDataT, class Comp>
size_t gdsu::DSUWithData<KeyT, RootDataT, Comp>::_getIdxByKey(
        const KeyT &key) const {
    if (auto it = _keyToIndex.find(key); it == _keyToIndex.end()) {
        throw std::invalid_argument("No such key.");
    } else {
        return it->second;
    }
}

//----------------------------------------------------------------------------//
template<class KeyT, class RootDataT, class Comp>
std::size_t
gdsu::DSUWithData<KeyT, RootDataT, Comp>::getComponentSize(const KeyT& key) const {
    return _getRootDataByIndex(_getIdxByKey(key)).getSize();
}

//----------------------------------------------------------------------------//
template<class KeyT, class RootDataT, class Comp>
auto
gdsu::DSUWithData<KeyT, RootDataT, Comp>::_getRootData(
        std::size_t rootIdx) -> RootDataT& {
    if (auto dataIt = _data.find(rootIdx); dataIt == _data.end()) {
        throw std::runtime_error("No root data in index.");
    } else {
        return dataIt->second;
    }
}

//----------------------------------------------------------------------------//
template<class KeyT, class RootDataT, class Comp>
auto
gdsu::DSUWithData<KeyT, RootDataT, Comp>::_getRootData(
        std::size_t rootIdx) const -> const RootDataT& {
    if (auto dataIt = _data.find(rootIdx); dataIt == _data.end()) {
        throw std::runtime_error("No root data in index.");
    } else {
        return dataIt->second;
    }
}
