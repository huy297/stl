#pragma once

#include <new>
#include <cstddef>

#include "mystl/utility.hpp"

namespace mystl {

template <typename T, typename... Args>
constexpr T* construct_at(T* p, Args&&... args) {
    return ::new ((void*)p) T(mystl::forward<Args>(args)...);
}


template <typename T>
constexpr void destroy_at(T* p) {
    p->~T();
}


}
