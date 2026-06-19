#pragma once

#include "mystl/utility.hpp"

namespace mystl {

template <typename ForwardIt, typename T>
void fill(ForwardIt first, ForwardIt last, const T& value) {
    while (first != last) {
        *first = value;
        ++first;
    }
}

template <typename InputIt, typename OutputIt>
OutputIt copy(InputIt first, InputIt last, OutputIt dest) {
    while (first != last) {
        *dest = *first;
        first++;
        dest++;
    }

    return dest;
}

template <typename InputIt, typename OutputIt>
OutputIt move(InputIt first, InputIt last, OutputIt dest) {
    while (first != last) {
        *dest = mystl::move(*first);

        first++;
        dest++;
    }

    return dest;
}

template<typename InputIt1, typename InputIt2>
bool equal(InputIt1 first1, InputIt1 last1, InputIt2 first2) {
    while (first1 != last1) {
        if (!(*first1 == *first2)) {
            return false;
        }

        ++first1;
        ++first2;
    }

    return true;
}

template<typename InputIt1, typename InputIt2>
bool lexicographical_compare(
    InputIt1 first1,
    InputIt1 last1,
    InputIt2 first2,
    InputIt2 last2
) {
    while (first1 != last1 && first2 != last2) {
        if (*first1 < *first2) {
            return true;
        }

        if (*first2 < *first1) {
            return false;
        }

        ++first1;
        ++first2;
    }

    return first1 == last1 && first2 != last2;
}

template <typename ForwardIt1, typename ForwardIt2>
ForwardIt2 swap_ranges(ForwardIt1 first1, ForwardIt1 last1, ForwardIt2 first2) {
    while (first1 != last1) {
        mystl::swap(*first1, *first2);

        ++first1;
        ++first2;
    }

    return first2;
}


}
