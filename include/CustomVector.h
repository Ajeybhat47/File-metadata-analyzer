#ifndef CUSTOM_VECTOR_H
#define CUSTOM_VECTOR_H

#include <cstddef>
#include <utility>
#include <stdexcept>

template <typename T>
class CustomVector {
public:
    // Typedefs for various types associated with the vector
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using iterator = pointer;
    using const_iterator = const_pointer;

    // Default constructor
    CustomVector() : data(nullptr), size_(0), capacity_(0) {}

    // Destructor
    ~CustomVector() {
        clear();
        ::operator delete(data, sizeof(value_type) * capacity_);
    }

    // Copy constructor
    CustomVector(const CustomVector& other) : data(nullptr), size_(0), capacity_(0) {
        reserve(other.capacity_);
        for (size_type i = 0; i < other.size_; ++i) {
            new (data + i) value_type(other.data[i]);
        }
        size_ = other.size_;
    }

    // Copy assignment operator
    CustomVector& operator=(const CustomVector& other) {
        if (this != &other) {
            CustomVector temp(other);
            swap(temp);
        }
        return *this;
    }

    // Move constructor
    CustomVector(CustomVector&& other) noexcept : data(other.data), size_(other.size_), capacity_(other.capacity_) {
        other.data = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    // Move assignment operator
    CustomVector& operator=(CustomVector&& other) noexcept {
        if (this != &other) {
            clear();
            ::operator delete(data, sizeof(value_type) * capacity_);
            data = other.data;
            size_ = other.size_;
            capacity_ = other.capacity_;
            other.data = nullptr;
            other.size_ = 0;
            other.capacity_ = 0;
        }
        return *this;
    }

    // Adds a new element to the end of the vector
    void push_back(const value_type& value) {
        if (size_ == capacity_) {
            reserve(capacity_ ? capacity_ * 2 : 1);
        }
        new (data + size_) value_type(value);
        ++size_;
    }

    // Adds a new element to the end of the vector (move version)
    void push_back(value_type&& value) {
        if (size_ == capacity_) {
            reserve(capacity_ ? capacity_ * 2 : 1);
        }
        new (data + size_) value_type(std::move(value));
        ++size_;
    }

    // Subscript operator for accessing elements
    reference operator[](size_type index) {
        return data[index];
    }

    // Const subscript operator for accessing elements
    const_reference operator[](size_type index) const {
        return data[index];
    }

    // Iterator to the beginning of the vector
    iterator begin() { return data; }
    const_iterator begin() const { return data; }

    // Iterator to the end of the vector
    iterator end() { return data + size_; }
    const_iterator end() const { return data + size_; }

    // Returns the number of elements in the vector
    size_type size() const { return size_; }

    // Returns the current capacity of the vector
    size_type capacity() const { return capacity_; }

    // Checks if the vector is empty
    bool empty() const { return size_ == 0; }

    // Returns a reference to the last element in the vector
    const value_type& back() const {
        if (size_ == 0) {
            throw std::out_of_range("CustomVector is empty");
        }
        return data[size_ - 1];
    }

    // Returns a reference to the last element in the vector
    value_type& back() {
        if (size_ == 0) {
            throw std::out_of_range("CustomVector is empty");
        }
        return data[size_ - 1];
    }

    // Removes all elements from the vector
    void clear() {
        for (size_type i = 0; i < size_; ++i) {
            data[i].~value_type();
        }
        size_ = 0;
    }

    // Increases the capacity of the vector to at least the specified value
    void reserve(size_type new_capacity) {
        if (new_capacity > capacity_) {
            pointer new_data = static_cast<pointer>(::operator new(sizeof(value_type) * new_capacity));
            for (size_type i = 0; i < size_; ++i) {
                new (new_data + i) value_type(std::move(data[i]));
                data[i].~value_type();
            }
            std::swap(data, new_data);
            ::operator delete(new_data, sizeof(value_type) * capacity_);
            capacity_ = new_capacity;
        }
    }

    // Swaps the contents of two vectors
    void swap(CustomVector& other) noexcept {
        std::swap(data, other.data);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }

private:
    pointer data;
    size_type size_;
    size_type capacity_;
};

#endif // CUSTOM_VECTOR_H