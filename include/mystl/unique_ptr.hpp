#pragma once

#include <cstddef>

#include "mystl/utility.hpp"

namespace mystl {
template <typename T>
class unique_ptr {
private:
    T* ptr = nullptr;

public:
    unique_ptr() = default;

    unique_ptr(std::nullptr_t) noexcept : ptr(nullptr) {}

    explicit unique_ptr(T* p) : ptr(p) {}

    ~unique_ptr() {
        delete ptr;
    }

    unique_ptr(const unique_ptr&) = delete;
    unique_ptr& operator=(const unique_ptr&) = delete;

    unique_ptr(unique_ptr&& other) noexcept : ptr(other.ptr) {
        other.ptr = nullptr;
    }

    unique_ptr& operator=(unique_ptr&& other) noexcept {
        if (this == &other) return *this;

        reset(other.release());

        return *this;
    }

    unique_ptr& operator=(std::nullptr_t) noexcept {
        reset();

        return *this;
    }

    T& operator*() const {
        return *ptr;
    }

    T* operator->() const {
        return ptr;
    }

    T* get() const {
        return ptr;
    }

    T* release() {
        T* ans = ptr;
        ptr = nullptr;

        return ans;
    }

    void reset(T* p = nullptr) {
        delete ptr;
        ptr = p;
    }

    void swap(unique_ptr& other) noexcept {
        T* tmp = ptr;
        ptr = other.ptr;
        other.ptr = tmp;
    }


    explicit operator bool() const {
        return ptr != nullptr;
    }
};

template <typename T, typename... Args>
unique_ptr<T> make_unique(Args&&... args) {
    return unique_ptr<T>(new T(mystl::forward<Args>(args)...));
}

template <typename T>
void swap(unique_ptr<T>& a, unique_ptr<T>& b) noexcept {
    a.swap(b);
}

template <typename T>
bool operator==(const unique_ptr<T>& a, const unique_ptr<T>& b) {
    return a.get() == b.get();
}

template <typename T>
bool operator!=(const unique_ptr<T>& a, const unique_ptr<T>& b) {
    return a.get() != b.get();
}

template <typename T>
bool operator==(const unique_ptr<T>& a, std::nullptr_t) {
    return a.get() == nullptr;
}

template <typename T>
bool operator==(std::nullptr_t, const unique_ptr<T>& a) {
    return a.get() == nullptr;
}

template <typename T>
bool operator!=(const unique_ptr<T>& a, std::nullptr_t) {
    return a.get() != nullptr;
}

template <typename T>
bool operator!=(std::nullptr_t, const unique_ptr<T>& a) {
    return a.get() != nullptr;
}

}
