#pragma once

#include <cstddef>
#include <stdexcept>

#include "mystl/utility/utility.hpp"
#include "mystl/iterator/iterator.hpp"
#include "mystl/algorithm/algorithm.hpp"

namespace mystl {

template<typename T, std::size_t N>
class array {
public:
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    using reference = T&;
    using const_reference = const T&;

    using pointer = T*;
    using const_pointer = const T*;

    using iterator = T*;
    using const_iterator = const T*;

    using reverse_iterator = mystl::reverse_iterator<iterator>;
    using const_reverse_iterator = mystl::reverse_iterator<const_iterator>;

private:
    T elems[N];

public:
    reference operator[](size_type pos) {
        return elems[pos];
    }

    const_reference operator[](size_type pos) const {
        return elems[pos];
    }

    reference at(size_type pos) {
        if (pos >= N) {
            throw std::out_of_range("mystl::array::at");
        }

        return elems[pos];
    }

    const_reference at(size_type pos) const {
        if (pos >= N) {
            throw std::out_of_range("mystl::array::at");
        }

        return elems[pos];
    }

    reference front() {
        return elems[0];
    }

    const_reference front() const {
        return elems[0];
    }

    reference back() {
        return elems[N - 1];
    }

    const_reference back() const {
        return elems[N - 1];
    }

    pointer data() {
        return elems;
    }

    const_pointer data() const {
        return elems;
    }

    iterator begin() {
        return elems;
    }

    const_iterator begin() const {
        return elems;
    }

    iterator end() {
        return elems + N;
    }

    const_iterator end() const {
        return elems + N;
    }

    const_iterator cbegin() const {
        return elems;
    }

    const_iterator cend() const {
        return elems + N;
    }

    reverse_iterator rbegin() {
        return reverse_iterator(end());
    }

    const_reverse_iterator rbegin() const {
        return const_reverse_iterator(end());
    }

    reverse_iterator rend() {
        return reverse_iterator(begin());
    }

    const_reverse_iterator rend() const {
        return const_reverse_iterator(begin());
    }

    const_reverse_iterator crbegin() const {
        return const_reverse_iterator(cend());
    }

    const_reverse_iterator crend() const {
        return const_reverse_iterator(cbegin());
    }

    bool empty() const {
        return N == 0;
    }

    size_type size() const {
        return N;
    }

    size_type max_size() const {
        return N;
    }

    void fill(const T& value) {
        for (size_type i = 0; i < N; i++) {
            elems[i] = value;
        }
    }

    void swap(array& other) {
        for (size_type i = 0; i < N; i++) {
            mystl::swap(elems[i], other.elems[i]);
        }
    }

};

template<typename T, std::size_t N>
bool operator==(const array<T, N>& lhs, const array<T, N>& rhs) {
    return mystl::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template<typename T, std::size_t N>
bool operator!=(const array<T, N>& lhs, const array<T, N>& rhs) {
    return !(lhs == rhs);
}

template<typename T, std::size_t N>
bool operator<(const array<T, N>& lhs, const array<T, N>& rhs) {
    return mystl::lexicographical_compare(
        lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template<typename T, std::size_t N>
bool operator<=(const array<T, N>& lhs, const array<T, N>& rhs) {
    return !(rhs < lhs);
}

template<typename T, std::size_t N>
bool operator>(const array<T, N>& lhs, const array<T, N>& rhs) {
    return rhs < lhs;
}

template<typename T, std::size_t N>
bool operator>=(const array<T, N>& lhs, const array<T, N>& rhs) {
    return !(lhs < rhs);
}

template<std::size_t I, typename T, std::size_t N>
T& get(array<T, N>& a) {
    static_assert(I < N, "mystl::get: index out of bounds");
    return a[I];
}

template<std::size_t I, typename T, std::size_t N>
const T& get(const array<T, N>& a) {
    static_assert(I < N, "mystl::get: index out of bounds");
    return a[I];
}

}
