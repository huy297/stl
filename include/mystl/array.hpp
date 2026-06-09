#pragma once

#include <cstddef>
#include <stdexcept>

#include "mystl/utility.hpp"

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

}