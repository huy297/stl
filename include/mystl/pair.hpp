#pragma once

#include "mystl/utility.hpp"
#include "mystl/type_traits.hpp"

namespace mystl {

template <typename T1, typename T2>
struct pair {
    T1 first;
    T2 second;

    pair() = default;

    pair(const T1& a, const T2& b) : first(a), second(b) {}

    template <typename U1, typename U2>
    pair(U1&& a, U2&& b)
        : first(mystl::forward<U1>(a)),
          second(mystl::forward<U2>(b)) {}

    void swap(pair& other) {
        mystl::swap(first, other.first);
        mystl::swap(second, other.second);
    }
};

template <typename T1, typename T2>
bool operator == (const pair<T1, T2>& lhs, const pair<T1, T2>& rhs) {
    return lhs.first == rhs.first && lhs.second == rhs.second;
}

template <typename T1, typename T2>
bool operator != (const pair<T1, T2>& lhs, const pair<T1, T2>& rhs) {
    return !(lhs == rhs);
}

template <typename T1, typename T2>
bool operator < (const pair<T1, T2>& lhs, const pair<T1, T2>& rhs) {
    return lhs.first < rhs.first ||
           (!(rhs.first < lhs.first) && lhs.second < rhs.second);
}

template <typename T1, typename T2>
bool operator <= (const pair<T1, T2>& lhs, const pair<T1, T2>& rhs) {
    return !(rhs < lhs);
}

template <typename T1, typename T2>
bool operator > (const pair<T1, T2>& lhs, const pair<T1, T2>& rhs) {
    return rhs < lhs;
}

template <typename T1, typename T2>
bool operator >= (const pair<T1, T2>& lhs, const pair<T1, T2>& rhs) {
    return !(lhs < rhs);
}

template <unsigned long I, typename T1, typename T2>
struct pair_element;

template <typename T1, typename T2>
struct pair_element<0, T1, T2> {
    using type = T1;
};

template <typename T1, typename T2>
struct pair_element<1, T1, T2> {
    using type = T2;
};

template <unsigned long I, typename T1, typename T2>
typename pair_element<I, T1, T2>::type& get(pair<T1, T2>& p) {
    if constexpr (I == 0) {
        return p.first;
    } else {
        return p.second;
    }
}

template <unsigned long I, typename T1, typename T2>
const typename pair_element<I, T1, T2>::type& get(const pair<T1, T2>& p) {
    if constexpr (I == 0) {
        return p.first;
    } else {
        return p.second;
    }
}

template <typename T1, typename T2>
pair<remove_cvref_t<T1>, remove_cvref_t<T2>>
make_pair(T1&& a, T2&& b) {
    return pair<remove_cvref_t<T1>, remove_cvref_t<T2>>(
        mystl::forward<T1>(a),
        mystl::forward<T2>(b)
    );
}

}
