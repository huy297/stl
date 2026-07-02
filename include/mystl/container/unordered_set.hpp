#pragma once

#include <cstddef>
#include <initializer_list>

#include "mystl/utility/pair.hpp"
#include "mystl/utility/hash.hpp"
#include "mystl/utility/functional.hpp"
#include "mystl/utility/utility.hpp"
#include "mystl/detail/hash_table.hpp"

namespace mystl {

template<typename Key,
         typename Hash = mystl::hash<Key>,
         typename KeyEqual = mystl::equal_to<Key>>
class unordered_set {
public:
    using key_type = Key;
    using value_type = Key;
    using hasher = Hash;
    using key_equal = KeyEqual;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;

private:

    struct identity {
        const Key& operator()(const value_type& v) const {
            return v;
        }
    };

    using table_type =
        detail::hash_table<Key, value_type, identity, Hash, KeyEqual>;
    table_type table_;

public:

    using iterator = typename table_type::const_iterator;
    using const_iterator = typename table_type::const_iterator;

    unordered_set() = default;

    unordered_set(std::initializer_list<value_type> init) {
        for (const value_type& v : init) {
            table_.insert_unique(v);
        }
    }

    unordered_set(const unordered_set& other) : table_(other.table_) {}

    unordered_set(unordered_set&& other) noexcept
        : table_(mystl::move(other.table_)) {}

    unordered_set& operator=(const unordered_set& other) {
        table_ = other.table_;
        return *this;
    }

    unordered_set& operator=(unordered_set&& other) noexcept {
        table_ = mystl::move(other.table_);
        return *this;
    }

    unordered_set& operator=(std::initializer_list<value_type> init) {
        unordered_set tmp(init);
        swap(tmp);
        return *this;
    }

    iterator begin() const noexcept { return table_.cbegin(); }
    iterator end() const noexcept { return table_.cend(); }
    const_iterator cbegin() const noexcept { return table_.cbegin(); }
    const_iterator cend() const noexcept { return table_.cend(); }

    bool empty() const noexcept { return table_.empty(); }
    size_type size() const noexcept { return table_.size(); }

    void clear() noexcept { table_.clear(); }

    pair<iterator, bool> insert(const value_type& v) {
        auto r = table_.insert_unique(v);
        return pair<iterator, bool>(r.first, r.second);
    }

    pair<iterator, bool> insert(value_type&& v) {
        auto r = table_.insert_unique(mystl::move(v));
        return pair<iterator, bool>(r.first, r.second);
    }

    template<typename... Args>
    pair<iterator, bool> emplace(Args&&... args) {
        auto r = table_.emplace_unique(mystl::forward<Args>(args)...);
        return pair<iterator, bool>(r.first, r.second);
    }

    size_type erase(const key_type& k) {
        return table_.erase(k);
    }

    iterator erase(const_iterator pos) {
        return table_.erase(pos);
    }

    void swap(unordered_set& other) noexcept {
        table_.swap(other.table_);
    }

    iterator find(const key_type& k) const { return table_.find(k); }
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

template<typename Key, typename Hash, typename KeyEqual>
void swap(unordered_set<Key, Hash, KeyEqual>& a,
          unordered_set<Key, Hash, KeyEqual>& b) noexcept {
    a.swap(b);
}

}
