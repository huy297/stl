#pragma once

#include "type_traits.hpp"

namespace mystl {


// move


template<typename T>
constexpr remove_reference_t<T>&& move(T&& x) noexcept {
    return static_cast<remove_reference_t<T>&&>(x);
}

// forward

template<typename T>
constexpr T&& forward(remove_reference_t<T>& x) noexcept {
    return static_cast<T&&>(x);
}

// template <typename T>
// constexpr T&& forward(remove_reference_t<T>&& x) noexcept {
//     static_assert(!is_lvalue_reference_v<T>, "bad forward");
//     return static_cast<T&&>(x);
// }


// swap

template<typename T>
void swap(T& a, T& b) {
    T tmp = mystl::move(a);
    a = mystl::move(b);
    b = mystl::move(tmp);
}

// exchange

template<typename T, typename U = T>
T exchange(T& obj, U&& new_value) {
    T old = mystl::move(obj);
    obj = mystl::forward<U>(new_value);

    return old;
}


}