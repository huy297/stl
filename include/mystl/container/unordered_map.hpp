#pragma once

#include <cstddef>
#include <stdexcept>
#include <initializer_list>

#include "mystl/utility/pair.hpp"
#include "mystl/utility/hash.hpp"
#include "mystl/utility/functional.hpp"
#include "mystl/utility/utility.hpp"
#include "mystl/detail/hash_table.hpp"

namespace mystl {

template<typename Key, typename T,
         typename Hash = mystl::hash<Key>,
         typename KeyEqual = mystl::equal_to<Key>>
class unordered_map {
public:
    using key_type = Key;
    using mapped_type = T;
    using value_type = pair<const Key, T>;
    using hasher = Hash;
    using key_equal = KeyEqual;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;

private:

    struct select_first {
        const Key& operator()(const value_type& v) const {
            return v.first;
        }
    };

    using table_type =
        detail::hash_table<Key, value_type, select_first, Hash, KeyEqual>;
    table_type table_;

public:
    using iterator = typename table_type::iterator;
    using const_iterator = typename table_type::const_iterator;

    unordered_map() = default;

    unordered_map(std::initializer_list<value_type> init) {
        for (const value_type& v : init) {
            table_.insert_unique(v);
        }
    }

    unordered_map(const unordered_map& other) : table_(other.table_) {}

    unordered_map(unordered_map&& other) noexcept
        : table_(mystl::move(other.table_)) {}

    unordered_map& operator=(const unordered_map& other) {
        table_ = other.table_;
        return *this;
    }

    unordered_map& operator=(unordered_map&& other) noexcept {
        table_ = mystl::move(other.table_);
        return *this;
    }

    unordered_map& operator=(std::initializer_list<value_type> init) {
        unordered_map tmp(init);
        swap(tmp);
        return *this;
    }

    iterator begin() noexcept { return table_.begin(); }
    const_iterator begin() const noexcept { return table_.begin(); }
    iterator end() noexcept { return table_.end(); }
    const_iterator end() const noexcept { return table_.end(); }
    const_iterator cbegin() const noexcept { return table_.cbegin(); }
    const_iterator cend() const noexcept { return table_.cend(); }

    bool empty() const noexcept { return table_.empty(); }
    size_type size() const noexcept { return table_.size(); }

    void clear() noexcept { table_.clear(); }

    pair<iterator, bool> insert(const value_type& v) {
        return table_.insert_unique(v);
    }

    pair<iterator, bool> insert(value_type&& v) {
        return table_.insert_unique(mystl::move(v));
    }

    template<typename... Args>
    pair<iterator, bool> emplace(Args&&... args) {
        return table_.emplace_unique(mystl::forward<Args>(args)...);
    }

    size_type erase(const key_type& k) {
        return table_.erase(k);
    }

    iterator erase(const_iterator pos) {
        return table_.erase(pos);
    }

    void swap(unordered_map& other) noexcept {
        table_.swap(other.table_);
    }

    T& operator[](const key_type& k) {
        iterator it = table_.find(k);
        if (it == end()) {
            it = table_.insert_unique(value_type(k, T())).first;
        }
        return it->second;
    }

    T& operator[](key_type&& k) {
        iterator it = table_.find(k);
        if (it == end()) {
            it = table_.insert_unique(value_type(mystl::move(k), T())).first;
        }
        return it->second;
    }

    T& at(const key_type& k) {
        iterator it = table_.find(k);
        if (it == end()) {
            throw std::out_of_range("mystl::unordered_map::at");
        }
        return it->second;
    }

    const T& at(const key_type& k) const {
        const_iterator it = table_.find(k);
        if (it == end()) {
            throw std::out_of_range("mystl::unordered_map::at");
        }
        return it->second;
    }

    iterator find(const key_type& k) { return table_.find(k); }
    const_iterator find(const key_type& k) const { return table_.find(k); }
    size_type count(const key_type& k) const { return table_.count(k); }
    bool contains(const key_type& k) const { return table_.contains(k); }

    size_type bucket_count() const noexcept { return table_.bucket_count(); }
    float load_factor() const noexcept { return table_.load_factor(); }
    float max_load_factor() const noexcept { return table_.max_load_factor(); }
    void max_load_factor(float ml) { table_.max_load_factor(ml); }
    void rehash(size_type n) { table_.rehash(n); }
    void reserve(size_type n) { table_.reserve(n); }

    hasher hash_function() const { return table_.hash_function(); }
    key_equal key_eq() const { return table_.key_eq(); }
};

template<typename Key, typename T, typename Hash, typename KeyEqual>
void swap(unordered_map<Key, T, Hash, KeyEqual>& a,
          unordered_map<Key, T, Hash, KeyEqual>& b) noexcept {
    a.swap(b);
}

}
