#ifndef CUSTOM_MAP_H
#define CUSTOM_MAP_H

#include "CustomVector.h"

/**
 A custom implementation of a key-value map data structure.
 @tparam KeyType The type of the keys stored in the map.
 @tparam ValueType The type of the values stored in the map.
 */
template<typename KeyType, typename ValueType>
class CustomMap {
private:
    struct Pair {
        KeyType key;
        ValueType value;
    };

    CustomVector<Pair> pairs; //storage for the key-value pairs

public:
    // Default constructor
    CustomMap() = default;

    // Custom find_if function
    template <typename Iterator, typename Predicate>
    Iterator custom_find_if(Iterator begin, Iterator end, Predicate pred) {
        for (Iterator it = begin; it != end; ++it) {
            if (pred(*it)) {
                return it;
            }
        }
        return end;
    }

    // Inserts a new key-value pair into the map
    void insert(const KeyType& key, const ValueType& value) {
        auto it = custom_find_if(pairs.begin(), pairs.end(), [&](const Pair& pair) {
            return pair.key == key;
        });

        if (it != pairs.end()) {
            it->value = value;
        } else {
            pairs.push_back({key, value});
        }
    }

    ValueType& operator[](const KeyType& key) {
        auto it = custom_find_if(pairs.begin(), pairs.end(), [&](const Pair& pair) {
            return pair.key == key;
        });

        if (it != pairs.end()) {
            return it->value;
        } else {
            pairs.push_back({key, ValueType{}});
            return pairs.back().value;
        }
    }

    // Iterators for accessing the key-value pairs
    typename CustomVector<Pair>::iterator begin() {
        return pairs.begin();
    }

    typename CustomVector<Pair>::iterator end() {
        return pairs.end();
    }

    typename CustomVector<Pair>::const_iterator begin() const {
        return pairs.begin();
    }

    typename CustomVector<Pair>::const_iterator end() const {
        return pairs.end();
    }

    /**
     Finds the iterator pointing to the key-value pair with the given key.
     @param key The key to be searched for.
     @return An iterator pointing to the key-value pair, or the end iterator if the key is not found.
     */
    typename CustomVector<Pair>::iterator find(const KeyType& key) {
        return custom_find_if(pairs.begin(), pairs.end(), [&](const Pair& pair) {
            return pair.key == key;
        });
    }

    /**
     Finds the const_iterator pointing to the key-value pair with the given key.
     @param key The key to be searched for.
     @return A const_iterator pointing to the key-value pair, or the end iterator if the key is not found.
     */
    typename CustomVector<Pair>::const_iterator find(const KeyType& key) const {
        return custom_find_if(pairs.begin(), pairs.end(), [&](const Pair& pair) {
            return pair.key == key;
        });
    }

    // Removes the key-value pair with the given key
    void erase(const KeyType& key) {
        auto it = custom_find_if(pairs.begin(), pairs.end(), [&](const Pair& pair) {
            return pair.key == key;
        });

        if (it != pairs.end()) {
            pairs.erase(it);
        }
    }

    // Returns the number of key-value pairs stored in the map.
    typename CustomVector<Pair>::size_type size() const {
        return pairs.size();
    }

    // Checks if the map is empty.
    bool empty() const {
        return pairs.empty();
    }

    // Removes all key-value pairs from the map
    void clear() {
        pairs.clear();
    }

    // Copy constructor
    CustomMap(const CustomMap& other) : pairs(other.pairs) {}

    // Move constructor
    CustomMap(CustomMap&& other) noexcept : pairs(std::move(other.pairs)) {}

    // Copy assignment operator
    CustomMap& operator=(const CustomMap& other) {
        if (this != &other) {
            pairs = other.pairs;
        }
        return *this;
    }

    // Move assignment operator
    CustomMap& operator=(CustomMap&& other) noexcept {
        if (this != &other) {
            pairs = std::move(other.pairs);
        }
        return *this;
    }

    // Equality operator
    bool operator==(const CustomMap& other) const {
        return pairs == other.pairs;
    }

    // Inequality operator
    bool operator!=(const CustomMap& other) const {
        return !(*this == other);
    }
};

#endif // CUSTOM_MAP_H