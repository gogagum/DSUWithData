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
             template<class> class RootDataT,
             template<class> class SimpleDataT>
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
            // @return root data.
            const RootDataT<KeyT>& getRootData() const;

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
        // Constructor from data
        DSUWithData(std::initializer_list<KeyT> keys);

        // Join two nodes
        Component& join(Component&& comp1, Component&& comp2);

        // Get component.
        // @param key - search key.
        // @return component of the key.
        Component& getComponent(const KeyT& key);

        // Get number of components.
        // @return number of components.
        [[nodiscard]] std::size_t getNumberOfComponents() const;

        // Root data by key.
        // @param key to search.
        const RootDataT<KeyT>& getRootData(const KeyT& key);

    private:

        std::size_t _getRootIdxByIndex(std::size_t idx);

        RootDataT<KeyT>& _getRootDataByIndex(std::size_t idx);

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
    std::size_t i;
    for (i = 0; i < _parents.size(); ++i) {
        _parents[i] = i;
    }

    for (auto& key : keys) {
        _data.push_back(RootDataT<KeyT>(key));
    }

    for (i = 0; i < _data.size(); ++i) {
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

    auto& biggerComponentData = std::get<RootDataT<KeyT>>(_data[biggerComponent._rootIndex]);
    auto& smallerComponentData = std::get<RootDataT<KeyT>>(_data[smallerComponent._rootIndex]);

    SimpleDataT newSmallerComponentData = biggerComponentData.joinWith(std::move(smallerComponentData));

    _data[smallerComponent._rootIndex] = std::move(newSmallerComponentData);
    _rootIdxToComponent.erase(smallerComponent._rootIndex);
    _rootIdxToComponent[biggerComponent._rootIndex] = std::move(biggerComponent);

    return _rootIdxToComponent[biggerComponent._rootIndex];
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
    std::size_t rootIndex = _getRootIdxByIndex(idx);
    return std::get<RootDataT<KeyT>>(_data[rootIndex]);
}

//----------------------------------------------------------------------------//
template<class KeyT,
         template<class> class RootDataT,
         template<class> class SimpleDataT>
auto
gdsu::DSUWithData<KeyT, RootDataT, SimpleDataT>::getComponent(
        const KeyT& key) -> Component& {
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
gdsu::DSUWithData<KeyT, RootDataT, SimpleDataT>::getRootData(
        const KeyT& key) {
    return _getRootDataByIndex(_keyToIndex[key]);
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
gdsu::DSUWithData<KeyT, RootDataT, SimpleDataT>::Component::getRootData() const {
    _rootIndex = _owner->_getRootIdxByIndex(_rootIndex);
    return _owner->getRootData(_rootIndex);
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
gdsu::DSUWithData<KeyT, RootDataT, SimpleDataT>::Component::Component() : _rootIndex{}, _size{} {
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
