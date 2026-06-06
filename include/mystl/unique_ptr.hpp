#pragma once

#include "mystl/utility.hpp"


namespace mystl {
template <typename T>
class unique_ptr {
private:
    T* ptr = nullptr;

public: 
    unique_ptr() = default;

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


    explicit operator bool() const {
        return ptr != nullptr;
    }
};

template <typename T, typename... Args>
unique_ptr<T> make_unique(Args&&... args) {
    return unique_ptr<T>(new T(mystl::forward<Args>(args)...));
}



}
