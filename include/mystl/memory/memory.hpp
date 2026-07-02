#pragma once

#include <new>
#include <cstddef>

#include "mystl/utility/utility.hpp"

namespace mystl {

template <typename T, typename... Args>
constexpr T* construct_at(T* p, Args&&... args) {
    return ::new ((void*)p) T(mystl::forward<Args>(args)...);
}

template <typename T>
constexpr void destroy_at(T* p) {
    p->~T();
}

template <typename T>
constexpr T* addressof(T& r) noexcept {
    return __builtin_addressof(r);
}

template <typename ForwardIt>
void destroy(ForwardIt first, ForwardIt last) {
    for (; first != last; ++first) {
        mystl::destroy_at(mystl::addressof(*first));
    }
}

template <typename ForwardIt, typename Size>
ForwardIt destroy_n(ForwardIt first, Size n) {
    for (; n > 0; ++first, (void)--n) {
        mystl::destroy_at(mystl::addressof(*first));
    }
    return first;
}

template <typename InputIt, typename ForwardIt>
ForwardIt uninitialized_copy(InputIt first, InputIt last, ForwardIt d_first) {
    ForwardIt current = d_first;
    try {
        for (; first != last; ++first, (void)++current) {
            mystl::construct_at(mystl::addressof(*current), *first);
        }
        return current;
    } catch (...) {
        mystl::destroy(d_first, current);
        throw;
    }
}

template <typename InputIt, typename ForwardIt>
ForwardIt uninitialized_move(InputIt first, InputIt last, ForwardIt d_first) {
    ForwardIt current = d_first;
    try {
        for (; first != last; ++first, (void)++current) {
            mystl::construct_at(mystl::addressof(*current), mystl::move(*first));
        }
        return current;
    } catch (...) {
        mystl::destroy(d_first, current);
        throw;
    }
}

template <typename ForwardIt, typename T>
void uninitialized_fill(ForwardIt first, ForwardIt last, const T& value) {
    ForwardIt current = first;
    try {
        for (; current != last; ++current) {
            mystl::construct_at(mystl::addressof(*current), value);
        }
    } catch (...) {
        mystl::destroy(first, current);
        throw;
    }
}

template <typename ForwardIt>
void uninitialized_value_construct(ForwardIt first, ForwardIt last) {
    ForwardIt current = first;
    try {
        for (; current != last; ++current) {
            mystl::construct_at(mystl::addressof(*current));
        }
    } catch (...) {
        mystl::destroy(first, current);
        throw;
    }
}

}
