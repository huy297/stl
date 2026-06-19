#pragma once

#include "mystl/utility.hpp"

namespace mystl {

template <typename T1, typename T2>
struct pair {
    T1 first;
    T2 second;

    pair() = default;

    explicit pair(const T1& a, const T2& b) : first(a), second(b) {}

    template <typename U1, typename U2>
    explicit pair(U1&& a, U2&& b) noexcept
        : first(mystl::forward<U1>(a)),
          second(mystl::forward<U2>(b)) {}


    bool operator < (const pair& other) const {
        return (first == other.first ? second < other.second : first < other.first);
    }
};

template <typename T1, typename T2>
pair<remove_cvref_t<T1>, remove_cvref_t<T2>>
make_pair(T1&& a, T2&& b) {
    return pair<remove_cvref_t<T1>, remove_cvref_t<T2>>(
        mystl::forward<T1>(a),
        mystl::forward<T2>(b)
    );
}

}
