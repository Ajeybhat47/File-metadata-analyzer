#include <vector>
#include <stdexcept> // For std::out_of_range
#include <algorithm> // For std::find_if

template<typename KeyType, typename ValueType>
class CustomMap {
private:
    struct Pair {
        KeyType key;
        ValueType value;
    };

    std::vector<Pair> pairs;

public:
    // Default constructor
    CustomMap() = default;

    void insert(const KeyType& key, const ValueType& value) {
        auto it = std::find_if(pairs.begin(), pairs.end(), [&](const Pair& pair) {
            return pair.key == key;
        });

        if (it != pairs.end()) {
            it->value = value;
        } else {
            pairs.push_back({key, value});
        }
    }

    ValueType& operator[](const KeyType& key) {
    auto it = std::find_if(pairs.begin(), pairs.end(), [&](const Pair& pair) {
        return pair.key == key;
    });

    if (it != pairs.end()) {
        return it->value;
    } else {
        pairs.push_back({key, ValueType{}});
        return pairs.back().value;
    }
    }


    typename std::vector<Pair>::iterator begin() {
        return pairs.begin();
    }

    typename std::vector<Pair>::iterator end() {
        return pairs.end();
    }

    typename std::vector<Pair>::const_iterator begin() const {
        return pairs.begin();
    }

    typename std::vector<Pair>::const_iterator end() const {
        return pairs.end();
    }

    typename std::vector<Pair>::iterator find(const KeyType& key) {
        return std::find_if(pairs.begin(), pairs.end(), [&](const Pair& pair) {
            return pair.key == key;
        });
    }

    typename std::vector<Pair>::const_iterator find(const KeyType& key) const {
        return std::find_if(pairs.begin(), pairs.end(), [&](const Pair& pair) {
            return pair.key == key;
        });
    }

    void erase(const KeyType& key) {
        auto it = std::find_if(pairs.begin(), pairs.end(), [&](const Pair& pair) {
            return pair.key == key;
        });

        if (it != pairs.end()) {
            pairs.erase(it);
        }
    }

    std::size_t size() const {
        return pairs.size();
    }

    bool empty() const {
        return pairs.empty();
    }

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
