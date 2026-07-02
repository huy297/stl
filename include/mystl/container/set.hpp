#pragma once

#include <cstddef>
#include <initializer_list>

#include "mystl/utility/pair.hpp"
#include "mystl/utility/functional.hpp"
#include "mystl/utility/utility.hpp"
#include "mystl/iterator/iterator.hpp"
#include "mystl/detail/rb_tree.hpp"

namespace mystl {

template<typename Key, typename Compare = mystl::less<Key>>
class set {
public:
    using key_type = Key;
    using value_type = Key;
    using key_compare = Compare;
    using value_compare = Compare;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = const value_type&;
    using const_reference = const value_type&;
    using pointer = const value_type*;
    using const_pointer = const value_type*;

private:

    struct identity {
        const Key& operator()(const Key& v) const {
            return v;
        }
    };

    using tree_type = detail::rb_tree<Key, Key, identity, Compare>;
    tree_type tree_;

public:

    using iterator = typename tree_type::const_iterator;
    using const_iterator = typename tree_type::const_iterator;
    using reverse_iterator = mystl::reverse_iterator<const_iterator>;
    using const_reverse_iterator = mystl::reverse_iterator<const_iterator>;

    set() = default;

    explicit set(const Compare& comp) : tree_(comp) {}

    set(std::initializer_list<value_type> init) {
        for (const value_type& v : init) {
            tree_.insert_unique(v);
        }
    }

    set(const set& other) : tree_(other.tree_) {}

    set(set&& other) noexcept : tree_(mystl::move(other.tree_)) {}

    set& operator=(const set& other) {
        tree_ = other.tree_;
        return *this;
    }

    set& operator=(set&& other) noexcept {
        tree_ = mystl::move(other.tree_);
        return *this;
    }

    set& operator=(std::initializer_list<value_type> init) {
        set tmp(init);
        swap(tmp);
        return *this;
    }

    iterator begin() const noexcept { return tree_.begin(); }
    iterator end() const noexcept { return tree_.end(); }
    const_iterator cbegin() const noexcept { return tree_.cbegin(); }
    const_iterator cend() const noexcept { return tree_.cend(); }
    reverse_iterator rbegin() const noexcept { return reverse_iterator(end()); }
    reverse_iterator rend() const noexcept { return reverse_iterator(begin()); }
    const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(end()); }
    const_reverse_iterator crend() const noexcept { return const_reverse_iterator(begin()); }

    bool empty() const noexcept { return tree_.empty(); }
    size_type size() const noexcept { return tree_.size(); }

    void clear() noexcept { tree_.clear(); }

    pair<iterator, bool> insert(const value_type& v) {
        auto r = tree_.insert_unique(v);
        return pair<iterator, bool>(const_iterator(r.first), r.second);
    }

    pair<iterator, bool> insert(value_type&& v) {
        auto r = tree_.insert_unique(mystl::move(v));
        return pair<iterator, bool>(const_iterator(r.first), r.second);
    }

    template<typename... Args>
    pair<iterator, bool> emplace(Args&&... args) {
        auto r = tree_.emplace_unique(mystl::forward<Args>(args)...);
        return pair<iterator, bool>(const_iterator(r.first), r.second);
    }

    size_type erase(const key_type& k) {
        return tree_.erase(k);
    }

    iterator erase(const_iterator pos) {
        return tree_.erase(pos);
    }

    void swap(set& other) noexcept {
        tree_.swap(other.tree_);
    }

    iterator find(const key_type& k) const { return tree_.find(k); }
    size_type count(const key_type& k) const { return tree_.count(k); }
    bool contains(const key_type& k) const { return tree_.contains(k); }
    iterator lower_bound(const key_type& k) const { return tree_.lower_bound(k); }
    iterator upper_bound(const key_type& k) const { return tree_.upper_bound(k); }

    pair<iterator, iterator> equal_range(const key_type& k) const {
        return tree_.equal_range(k);
    }

    key_compare key_comp() const { return tree_.key_comp(); }
    value_compare value_comp() const { return tree_.key_comp(); }
};

template<typename Key, typename Compare>
bool operator==(const set<Key, Compare>& a, const set<Key, Compare>& b) {
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

template<typename Key, typename Compare>
bool operator!=(const set<Key, Compare>& a, const set<Key, Compare>& b) {
    return !(a == b);
}

template<typename Key, typename Compare>
bool operator<(const set<Key, Compare>& a, const set<Key, Compare>& b) {
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

template<typename Key, typename Compare>
bool operator<=(const set<Key, Compare>& a, const set<Key, Compare>& b) {
    return !(b < a);
}

template<typename Key, typename Compare>
bool operator>(const set<Key, Compare>& a, const set<Key, Compare>& b) {
    return b < a;
}

template<typename Key, typename Compare>
bool operator>=(const set<Key, Compare>& a, const set<Key, Compare>& b) {
    return !(a < b);
}

template<typename Key, typename Compare>
void swap(set<Key, Compare>& a, set<Key, Compare>& b) noexcept {
    a.swap(b);
}

}
