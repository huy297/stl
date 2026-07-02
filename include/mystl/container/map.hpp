#pragma once

#include <cstddef>
#include <stdexcept>
#include <initializer_list>

#include "mystl/utility/pair.hpp"
#include "mystl/utility/functional.hpp"
#include "mystl/utility/utility.hpp"
#include "mystl/iterator/iterator.hpp"
#include "mystl/detail/rb_tree.hpp"

namespace mystl {

template<typename Key, typename T, typename Compare = mystl::less<Key>>
class map {
public:
    using key_type = Key;
    using mapped_type = T;
    using value_type = pair<const Key, T>;
    using key_compare = Compare;
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

    using tree_type = detail::rb_tree<Key, value_type, select_first, Compare>;
    tree_type tree_;

public:
    using iterator = typename tree_type::iterator;
    using const_iterator = typename tree_type::const_iterator;
    using reverse_iterator = typename tree_type::reverse_iterator;
    using const_reverse_iterator = typename tree_type::const_reverse_iterator;

    map() = default;

    explicit map(const Compare& comp) : tree_(comp) {}

    map(std::initializer_list<value_type> init) {
        for (const value_type& v : init) {
            tree_.insert_unique(v);
        }
    }

    map(const map& other) : tree_(other.tree_) {}

    map(map&& other) noexcept : tree_(mystl::move(other.tree_)) {}

    map& operator=(const map& other) {
        tree_ = other.tree_;
        return *this;
    }

    map& operator=(map&& other) noexcept {
        tree_ = mystl::move(other.tree_);
        return *this;
    }

    map& operator=(std::initializer_list<value_type> init) {
        map tmp(init);
        swap(tmp);
        return *this;
    }

    iterator begin() noexcept { return tree_.begin(); }
    const_iterator begin() const noexcept { return tree_.begin(); }
    iterator end() noexcept { return tree_.end(); }
    const_iterator end() const noexcept { return tree_.end(); }
    const_iterator cbegin() const noexcept { return tree_.cbegin(); }
    const_iterator cend() const noexcept { return tree_.cend(); }
    reverse_iterator rbegin() noexcept { return tree_.rbegin(); }
    const_reverse_iterator rbegin() const noexcept { return tree_.rbegin(); }
    reverse_iterator rend() noexcept { return tree_.rend(); }
    const_reverse_iterator rend() const noexcept { return tree_.rend(); }
    const_reverse_iterator crbegin() const noexcept { return tree_.crbegin(); }
    const_reverse_iterator crend() const noexcept { return tree_.crend(); }

    bool empty() const noexcept { return tree_.empty(); }
    size_type size() const noexcept { return tree_.size(); }

    void clear() noexcept { tree_.clear(); }

    pair<iterator, bool> insert(const value_type& v) {
        return tree_.insert_unique(v);
    }

    pair<iterator, bool> insert(value_type&& v) {
        return tree_.insert_unique(mystl::move(v));
    }

    template<typename... Args>
    pair<iterator, bool> emplace(Args&&... args) {
        return tree_.emplace_unique(mystl::forward<Args>(args)...);
    }

    size_type erase(const key_type& k) {
        return tree_.erase(k);
    }

    iterator erase(iterator pos) {
        return tree_.erase(pos);
    }

    iterator erase(const_iterator pos) {
        return tree_.erase(pos);
    }

    void swap(map& other) noexcept {
        tree_.swap(other.tree_);
    }

    T& operator[](const key_type& k) {
        iterator it = tree_.lower_bound(k);
        if (it == end() || key_compare()(k, it->first)) {
            it = tree_.insert_unique(value_type(k, T())).first;
        }
        return it->second;
    }

    T& operator[](key_type&& k) {
        iterator it = tree_.lower_bound(k);
        if (it == end() || key_compare()(k, it->first)) {
            it = tree_.insert_unique(value_type(mystl::move(k), T())).first;
        }
        return it->second;
    }

    T& at(const key_type& k) {
        iterator it = tree_.find(k);
        if (it == end()) {
            throw std::out_of_range("mystl::map::at");
        }
        return it->second;
    }

    const T& at(const key_type& k) const {
        const_iterator it = tree_.find(k);
        if (it == end()) {
            throw std::out_of_range("mystl::map::at");
        }
        return it->second;
    }

    iterator find(const key_type& k) { return tree_.find(k); }
    const_iterator find(const key_type& k) const { return tree_.find(k); }
    size_type count(const key_type& k) const { return tree_.count(k); }
    bool contains(const key_type& k) const { return tree_.contains(k); }
    iterator lower_bound(const key_type& k) { return tree_.lower_bound(k); }
    const_iterator lower_bound(const key_type& k) const { return tree_.lower_bound(k); }
    iterator upper_bound(const key_type& k) { return tree_.upper_bound(k); }
    const_iterator upper_bound(const key_type& k) const { return tree_.upper_bound(k); }

    pair<iterator, iterator> equal_range(const key_type& k) {
        return tree_.equal_range(k);
    }

    pair<const_iterator, const_iterator> equal_range(const key_type& k) const {
        return tree_.equal_range(k);
    }

    key_compare key_comp() const { return tree_.key_comp(); }
};

template<typename Key, typename T, typename Compare>
bool operator==(const map<Key, T, Compare>& a, const map<Key, T, Compare>& b) {
    if (a.size() != b.size()) {
        return false;
    }
    auto ia = a.begin();
    auto ib = b.begin();
    for (; ia != a.end(); ++ia, ++ib) {
        if (!(*ia == *ib)) {
            return false;
        }
    }
    return true;
}

template<typename Key, typename T, typename Compare>
bool operator!=(const map<Key, T, Compare>& a, const map<Key, T, Compare>& b) {
    return !(a == b);
}

template<typename Key, typename T, typename Compare>
bool operator<(const map<Key, T, Compare>& a, const map<Key, T, Compare>& b) {
    auto ia = a.begin();
    auto ib = b.begin();
    for (; ia != a.end() && ib != b.end(); ++ia, ++ib) {
        if (*ia < *ib) {
            return true;
        }
        if (*ib < *ia) {
            return false;
        }
    }
    return ia == a.end() && ib != b.end();
}

template<typename Key, typename T, typename Compare>
bool operator<=(const map<Key, T, Compare>& a, const map<Key, T, Compare>& b) {
    return !(b < a);
}

template<typename Key, typename T, typename Compare>
bool operator>(const map<Key, T, Compare>& a, const map<Key, T, Compare>& b) {
    return b < a;
}

template<typename Key, typename T, typename Compare>
bool operator>=(const map<Key, T, Compare>& a, const map<Key, T, Compare>& b) {
    return !(a < b);
}

template<typename Key, typename T, typename Compare>
void swap(map<Key, T, Compare>& a, map<Key, T, Compare>& b) noexcept {
    a.swap(b);
}

}
