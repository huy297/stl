#pragma once

#include <new>
#include <stdexcept>

#include "mystl/utility.hpp"

namespace mystl {

template<typename T>
class optional {
private:
    alignas(T) unsigned char storage[sizeof(T)];
    bool engaged = false;

    T* ptr() {
        return reinterpret_cast<T*>(storage);
    }

    const T* ptr() const {
        return reinterpret_cast<const T*>(storage);
    }

public:
    optional() = default;

    explicit optional(const T& value) {
        emplace(value);
    }

    explicit optional(T&& value) noexcept {
        emplace(mystl::move(value));
    }

    optional(const optional& other) {
        if (other.engaged) {
            emplace(*other.ptr());
        }
    }

    optional(optional&& other) noexcept {
        if (other.engaged) {
            emplace(mystl::move(*other.ptr()));
            other.reset();
        }
    }

    optional& operator=(const optional& other) {
        if (this == &other) return *this;

        reset();

        if (other.engaged) {
            emplace(*other.ptr());
        }

        return *this;
    }


    optional& operator=(optional&& other) noexcept {
        if (this == &other) return *this;

        reset();

        if (other.engaged) {
            emplace(mystl::move(*other.ptr()));
            other.reset();
        }

        return *this;
    }

    ~optional() {
        reset();
    }



    template<typename... Args>
    T& emplace(Args&&... args) {
        reset();

        new (storage) T(mystl::forward<Args>(args)...);
        engaged = true;

        return *ptr();
    }

    void reset() {
        if (engaged) {
            ptr()->~T();
            engaged = false;
        }
    }

    bool has_value() const {
        return engaged;
    }

    explicit operator bool() const {
        return engaged;
    }

    T& value() {
        if (!engaged) {
            throw std::runtime_error("bad optional access");
        }

        return *ptr();
    }

    const T& value() const {
        if (!engaged) {
            throw std::runtime_error("bad optional access");
        }

        return *ptr();
    }

    T& operator*() {
        return value();
    }

    const T& operator*() const {
        return value();
    }

    T* operator->() {
        return &value();
    }
};
}
