#pragma once

#include <cstddef>
#include <stdexcept>
#include <initializer_list>

#include "mystl/memory/allocator.hpp"
#include "mystl/memory/memory.hpp"
#include "mystl/utility/utility.hpp"
#include "mystl/iterator/iterator.hpp"
#include "mystl/algorithm/algorithm.hpp"

namespace mystl {

template<typename T>
class vector {
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
    pointer data_ = nullptr;
    size_type size_ = 0;
    size_type cap_ = 0;
    allocator<T> alloc_{};

    void destroy_all() noexcept {
        for (size_type i = 0; i < size_; ++i) {
            mystl::destroy_at(data_ + i);
        }
    }

    void swap_storage(vector& other) noexcept {
        mystl::swap(data_, other.data_);
        mystl::swap(size_, other.size_);
        mystl::swap(cap_, other.cap_);
    }

    void reallocate(size_type new_cap) {
        pointer new_data = alloc_.allocate(new_cap);

        size_type i = 0;
        try {
            for (; i < size_; ++i) {
                if constexpr (__is_nothrow_constructible(T, T&&)) {
                    mystl::construct_at(new_data + i, mystl::move(data_[i]));
                } else {
                    mystl::construct_at(new_data + i, data_[i]);
                }
            }
        } catch (...) {
            for (size_type j = 0; j < i; ++j) {
                mystl::destroy_at(new_data + j);
            }
            alloc_.deallocate(new_data, new_cap);
            throw;
        }

        for (size_type j = 0; j < size_; ++j) {
            mystl::destroy_at(data_ + j);
        }
        if (data_) {
            alloc_.deallocate(data_, cap_);
        }

        data_ = new_data;
        cap_ = new_cap;
    }

public:
    vector() = default;

    explicit vector(size_type count) {
        data_ = alloc_.allocate(count);
        cap_ = count;
        try {
            for (; size_ < count; ++size_) {
                mystl::construct_at(data_ + size_);
            }
        } catch (...) {
            destroy_all();
            alloc_.deallocate(data_, cap_);
            throw;
        }
    }

    vector(size_type count, const T& value) {
        data_ = alloc_.allocate(count);
        cap_ = count;
        try {
            for (; size_ < count; ++size_) {
                mystl::construct_at(data_ + size_, value);
            }
        } catch (...) {
            destroy_all();
            alloc_.deallocate(data_, cap_);
            throw;
        }
    }

    vector(std::initializer_list<T> init) {
        data_ = alloc_.allocate(init.size());
        cap_ = init.size();
        try {
            for (const T& x : init) {
                mystl::construct_at(data_ + size_, x);
                ++size_;
            }
        } catch (...) {
            destroy_all();
            alloc_.deallocate(data_, cap_);
            throw;
        }
    }

    vector(const vector& other) {
        data_ = alloc_.allocate(other.size_);
        cap_ = other.size_;
        try {
            for (; size_ < other.size_; ++size_) {
                mystl::construct_at(data_ + size_, other.data_[size_]);
            }
        } catch (...) {
            destroy_all();
            alloc_.deallocate(data_, cap_);
            throw;
        }
    }

    vector(vector&& other) noexcept
        : data_(other.data_), size_(other.size_), cap_(other.cap_) {
        other.data_ = nullptr;
        other.size_ = 0;
        other.cap_ = 0;
    }

    ~vector() {
        destroy_all();
        if (data_) {
            alloc_.deallocate(data_, cap_);
        }
    }

    vector& operator=(const vector& other) {
        if (this != &other) {
            vector tmp(other);
            swap_storage(tmp);
        }
        return *this;
    }

    vector& operator=(vector&& other) noexcept {
        if (this != &other) {
            destroy_all();
            if (data_) {
                alloc_.deallocate(data_, cap_);
            }
            data_ = other.data_;
            size_ = other.size_;
            cap_ = other.cap_;
            other.data_ = nullptr;
            other.size_ = 0;
            other.cap_ = 0;
        }
        return *this;
    }

    size_type size() const {
        return size_;
    }

    size_type capacity() const {
        return cap_;
    }

    bool empty() const {
        return size_ == 0;
    }

    void reserve(size_type new_cap) {
        if (new_cap > cap_) {
            reallocate(new_cap);
        }
    }

    void resize(size_type count) {
        if (count < size_) {
            while (size_ > count) {
                --size_;
                mystl::destroy_at(data_ + size_);
            }
        } else if (count > size_) {
            if (count > cap_) {
                reallocate(count);
            }
            while (size_ < count) {
                mystl::construct_at(data_ + size_);
                ++size_;
            }
        }
    }

    void shrink_to_fit() {
        if (cap_ > size_) {
            reallocate(size_);
        }
    }

    reference operator[](size_type pos) {
        return data_[pos];
    }

    const_reference operator[](size_type pos) const {
        return data_[pos];
    }

    reference at(size_type pos) {
        if (pos >= size_) {
            throw std::out_of_range("mystl::vector::at");
        }
        return data_[pos];
    }

    const_reference at(size_type pos) const {
        if (pos >= size_) {
            throw std::out_of_range("mystl::vector::at");
        }
        return data_[pos];
    }

    reference front() {
        return data_[0];
    }

    const_reference front() const {
        return data_[0];
    }

    reference back() {
        return data_[size_ - 1];
    }

    const_reference back() const {
        return data_[size_ - 1];
    }

    pointer data() {
        return data_;
    }

    const_pointer data() const {
        return data_;
    }

    void push_back(const T& value) {
        emplace_back(value);
    }

    void push_back(T&& value) {
        emplace_back(mystl::move(value));
    }

    template<typename... Args>
    reference emplace_back(Args&&... args) {
        if (size_ == cap_) {
            reallocate(cap_ ? cap_ * 2 : 1);
        }
        mystl::construct_at(data_ + size_, mystl::forward<Args>(args)...);
        ++size_;
        return data_[size_ - 1];
    }

    void pop_back() {
        --size_;
        mystl::destroy_at(data_ + size_);
    }

    void clear() noexcept {
        destroy_all();
        size_ = 0;
    }

    void swap(vector& other) noexcept {
        swap_storage(other);
    }

    iterator begin() {
        return data_;
    }

    const_iterator begin() const {
        return data_;
    }

    iterator end() {
        return data_ + size_;
    }

    const_iterator end() const {
        return data_ + size_;
    }

    const_iterator cbegin() const {
        return data_;
    }

    const_iterator cend() const {
        return data_ + size_;
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

    iterator insert(const_iterator pos, const T& value) {
        size_type index = static_cast<size_type>(pos - cbegin());
        if (size_ == cap_) {
            reallocate(cap_ ? cap_ * 2 : 1);
        }
        if (index == size_) {
            mystl::construct_at(data_ + size_, value);
        } else {
            mystl::construct_at(data_ + size_, mystl::move(data_[size_ - 1]));
            for (size_type i = size_ - 1; i > index; --i) {
                data_[i] = mystl::move(data_[i - 1]);
            }
            data_[index] = value;
        }
        ++size_;
        return data_ + index;
    }

    iterator insert(const_iterator pos, T&& value) {
        size_type index = static_cast<size_type>(pos - cbegin());
        if (size_ == cap_) {
            reallocate(cap_ ? cap_ * 2 : 1);
        }
        if (index == size_) {
            mystl::construct_at(data_ + size_, mystl::move(value));
        } else {
            mystl::construct_at(data_ + size_, mystl::move(data_[size_ - 1]));
            for (size_type i = size_ - 1; i > index; --i) {
                data_[i] = mystl::move(data_[i - 1]);
            }
            data_[index] = mystl::move(value);
        }
        ++size_;
        return data_ + index;
    }

    iterator erase(const_iterator pos) {
        size_type index = static_cast<size_type>(pos - cbegin());
        for (size_type i = index; i + 1 < size_; ++i) {
            data_[i] = mystl::move(data_[i + 1]);
        }
        --size_;
        mystl::destroy_at(data_ + size_);
        return data_ + index;
    }

    iterator erase(const_iterator first, const_iterator last) {
        size_type index = static_cast<size_type>(first - cbegin());
        size_type count = static_cast<size_type>(last - first);
        if (count == 0) {
            return data_ + index;
        }
        for (size_type i = index; i + count < size_; ++i) {
            data_[i] = mystl::move(data_[i + count]);
        }
        size_type new_size = size_ - count;
        for (size_type i = new_size; i < size_; ++i) {
            mystl::destroy_at(data_ + i);
        }
        size_ = new_size;
        return data_ + index;
    }

    void assign(size_type count, const T& value) {
        clear();
        if (count > cap_) {
            reallocate(count);
        }
        for (size_type i = 0; i < count; ++i) {
            mystl::construct_at(data_ + i, value);
            ++size_;
        }
    }
};

template<typename T>
bool operator==(const vector<T>& a, const vector<T>& b) {
    return a.size() == b.size() && mystl::equal(a.begin(), a.end(), b.begin());
}

template<typename T>
bool operator!=(const vector<T>& a, const vector<T>& b) {
    return !(a == b);
}

template<typename T>
bool operator<(const vector<T>& a, const vector<T>& b) {
    return mystl::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end());
}

template<typename T>
bool operator<=(const vector<T>& a, const vector<T>& b) {
    return !(b < a);
}

template<typename T>
bool operator>(const vector<T>& a, const vector<T>& b) {
    return b < a;
}

template<typename T>
bool operator>=(const vector<T>& a, const vector<T>& b) {
    return !(a < b);
}

}
