#include <cstring>
#include <utility>

class String {
public:
    String() : data_(nullptr), length_(0), capacity_(0) {}

    String(const char* str) {
        length_ = std::strlen(str);
        capacity_ = length_ + 1;
        data_ = new char[capacity_];
        std::strcpy(data_, str);
    }

    String(const String& other) : data_(nullptr), length_(other.length_), capacity_(other.length_ + 1) {
        data_ = new char[capacity_];
        std::strcpy(data_, other.data_);
    }

    String(String&& other) noexcept : data_(other.data_), length_(other.length_), capacity_(other.capacity_) {
        other.data_ = nullptr;
        other.length_ = 0;
        other.capacity_ = 0;
    }

    ~String() {
        delete[] data_;
    }

    String& operator=(const String& other) {
        if (this != &other) {
            delete[] data_;
            length_ = other.length_;
            capacity_ = other.capacity_;
            data_ = new char[capacity_];
            std::strcpy(data_, other.data_);
        }
        return *this;
    }

    String& operator=(String&& other) noexcept {
        if (this != &other) {
            delete[] data_;
            data_ = other.data_;
            length_ = other.length_;
            capacity_ = other.capacity_;
            other.data_ = nullptr;
            other.length_ = 0;
            other.capacity_ = 0;
        }
        return *this;
    }

    bool operator==(const String& other) const {
        return std::strcmp(data_, other.data_) == 0;
    }

    bool operator!=(const String& other) const {
        return !(*this == other);
    }

    const char* c_str() const {
        return data_;
    }

private:
    char* data_;
    size_t length_;
    size_t capacity_;
};