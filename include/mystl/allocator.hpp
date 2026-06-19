#pragma once

#include <cstddef>

#include "mystl/utility.hpp"


namespace mystl {

template<typename T>
class allocator {
public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = std::size_t;
    using difference_type = std::size_t;

    template <typename U>
    struct rebind {
        using other = allocator<U>;
    };

    allocator() noexcept = default;

    template<typename U>
    allocator(const allocator<U>&) noexcept {}

    pointer allocate(size_type n) {
        if (n == 0) {
            return nullptr;
        }

        return static_cast<pointer>(::operator new(n * sizeof(T)));
    }

    void deallocate(pointer p, size_type) noexcept {
        ::operator delete(p);
    }

    template<typename... Args>
    void construct(pointer p, Args&&... args) {
        ::new (static_cast<void*>(p)) T(mystl::forward<Args>(args)...);
    }

    void destroy(pointer p) {
        p->~T();
    }
};


template <typename T, typename U>
bool operator==(const allocator<T>&, const allocator<U>&) noexcept {
    return true;
}

template <typename T, typename U>
bool operator!=(const allocator<T>&, const allocator<U>&) noexcept {
    return false;
}

}
