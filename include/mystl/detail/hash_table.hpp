#pragma once

#include <cstddef>

#include "mystl/utility/pair.hpp"
#include "mystl/container/vector.hpp"
#include "mystl/utility/utility.hpp"
#include "mystl/memory/memory.hpp"
#include "mystl/iterator/iterator.hpp"

namespace mystl {
namespace detail {

template<typename Key, typename Value, typename KeyOfValue,
         typename Hash, typename KeyEqual>
class hash_table {
    struct node {
        Value value;
        node* next;
    };

public:
    using key_type = Key;
    using value_type = Value;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using hasher = Hash;
    using key_equal = KeyEqual;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;

private:
    using bucket_vector = mystl::vector<node*>;

    bucket_vector buckets_;
    size_type size_ = 0;
    float max_load_factor_ = 1.0f;
    hasher hash_{};
    key_equal equal_{};

    static const Key& key_of(const Value& v) {
        return KeyOfValue()(v);
    }

    size_type bucket_for(const Key& k, size_type bc) const {
        return hash_(k) & (bc - 1);
    }

    template<typename... Args>
    node* make_node(Args&&... args) {
        node* n = static_cast<node*>(::operator new(sizeof(node)));
        try {
            mystl::construct_at(mystl::addressof(n->value),
                                mystl::forward<Args>(args)...);
        } catch (...) {
            ::operator delete(n);
            throw;
        }
        n->next = nullptr;
        return n;
    }

    void free_node(node* n) noexcept {
        mystl::destroy_at(mystl::addressof(n->value));
        ::operator delete(n);
    }

    static size_type round_up_pow2(size_type n) {
        size_type bc = 8;
        while (bc < n) {
            bc <<= 1;
        }
        return bc;
    }

public:

    class const_iterator;

    class iterator {
        friend class hash_table;
        friend class const_iterator;

        const hash_table* table_ = nullptr;
        node* node_ = nullptr;
        size_type bucket_ = 0;

        void skip_to_valid() {
            while (node_ == nullptr && bucket_ + 1 < table_->buckets_.size()) {
                ++bucket_;
                node_ = table_->buckets_[bucket_];
            }
        }

        iterator(const hash_table* t, node* n, size_type b)
            : table_(t), node_(n), bucket_(b) {}

    public:
        using iterator_category = mystl::forward_iterator_tag;
        using value_type = hash_table::value_type;
        using difference_type = hash_table::difference_type;
        using pointer = hash_table::pointer;
        using reference = hash_table::reference;

        iterator() = default;

        reference operator*() const {
            return node_->value;
        }

        pointer operator->() const {
            return mystl::addressof(node_->value);
        }

        iterator& operator++() {
            node_ = node_->next;
            skip_to_valid();
            return *this;
        }

        iterator operator++(int) {
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const iterator& other) const {
            return node_ == other.node_;
        }

        bool operator!=(const iterator& other) const {
            return node_ != other.node_;
        }
    };

    class const_iterator {
        friend class hash_table;

        const hash_table* table_ = nullptr;
        node* node_ = nullptr;
        size_type bucket_ = 0;

        void skip_to_valid() {
            while (node_ == nullptr && bucket_ + 1 < table_->buckets_.size()) {
                ++bucket_;
                node_ = table_->buckets_[bucket_];
            }
        }

        const_iterator(const hash_table* t, node* n, size_type b)
            : table_(t), node_(n), bucket_(b) {}

    public:
        using iterator_category = mystl::forward_iterator_tag;
        using value_type = hash_table::value_type;
        using difference_type = hash_table::difference_type;
        using pointer = hash_table::const_pointer;
        using reference = hash_table::const_reference;

        const_iterator() = default;

        const_iterator(const iterator& it)
            : table_(it.table_), node_(it.node_), bucket_(it.bucket_) {}

        reference operator*() const {
            return node_->value;
        }

        pointer operator->() const {
            return mystl::addressof(node_->value);
        }

        const_iterator& operator++() {
            node_ = node_->next;
            skip_to_valid();
            return *this;
        }

        const_iterator operator++(int) {
            const_iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const const_iterator& other) const {
            return node_ == other.node_;
        }

        bool operator!=(const const_iterator& other) const {
            return node_ != other.node_;
        }
    };

    hash_table() {
        buckets_.assign(8, nullptr);
    }

    hash_table(const hash_table& other)
        : max_load_factor_(other.max_load_factor_),
          hash_(other.hash_), equal_(other.equal_) {
        buckets_.assign(other.buckets_.size(), nullptr);
        try {
            for (const_iterator it = other.cbegin(); it != other.cend(); ++it) {
                insert_no_rehash(*it);
            }
        } catch (...) {
            clear();
            throw;
        }
    }

    hash_table(hash_table&& other) noexcept
        : buckets_(mystl::move(other.buckets_)),
          size_(other.size_),
          max_load_factor_(other.max_load_factor_),
          hash_(mystl::move(other.hash_)),
          equal_(mystl::move(other.equal_)) {
        other.size_ = 0;
        other.buckets_.assign(8, nullptr);
    }

    ~hash_table() {
        clear();
    }

    hash_table& operator=(const hash_table& other) {
        if (this != &other) {
            hash_table tmp(other);
            swap(tmp);
        }
        return *this;
    }

    hash_table& operator=(hash_table&& other) noexcept {
        if (this != &other) {
            clear();
            buckets_ = mystl::move(other.buckets_);
            size_ = other.size_;
            max_load_factor_ = other.max_load_factor_;
            hash_ = mystl::move(other.hash_);
            equal_ = mystl::move(other.equal_);
            other.size_ = 0;
            other.buckets_.assign(8, nullptr);
        }
        return *this;
    }

    iterator begin() noexcept {
        for (size_type b = 0; b < buckets_.size(); ++b) {
            if (buckets_[b]) {
                return iterator(this, buckets_[b], b);
            }
        }
        return end();
    }

    const_iterator begin() const noexcept {
        return cbegin();
    }

    iterator end() noexcept {
        return iterator(this, nullptr, buckets_.size());
    }

    const_iterator end() const noexcept {
        return cend();
    }

    const_iterator cbegin() const noexcept {
        for (size_type b = 0; b < buckets_.size(); ++b) {
            if (buckets_[b]) {
                return const_iterator(this, buckets_[b], b);
            }
        }
        return cend();
    }

    const_iterator cend() const noexcept {
        return const_iterator(this, nullptr, buckets_.size());
    }

    bool empty() const noexcept {
        return size_ == 0;
    }

    size_type size() const noexcept {
        return size_;
    }

    void clear() noexcept {
        for (size_type b = 0; b < buckets_.size(); ++b) {
            node* n = buckets_[b];
            while (n) {
                node* next = n->next;
                free_node(n);
                n = next;
            }
            buckets_[b] = nullptr;
        }
        size_ = 0;
    }

private:

    node* find_node(const Key& k, size_type& out_bucket) const {
        size_type b = bucket_for(k, buckets_.size());
        out_bucket = b;
        for (node* n = buckets_[b]; n; n = n->next) {
            if (equal_(key_of(n->value), k)) {
                return n;
            }
        }
        return nullptr;
    }

    void link_node(node* n) {
        size_type b = bucket_for(key_of(n->value), buckets_.size());
        n->next = buckets_[b];
        buckets_[b] = n;
        ++size_;
    }

    void insert_no_rehash(const Value& v) {
        node* n = make_node(v);
        link_node(n);
    }

    void maybe_rehash() {
        if (static_cast<float>(size_) >
            static_cast<float>(buckets_.size()) * max_load_factor_) {
            rehash(buckets_.size() * 2);
        }
    }

public:
    template<typename V>
    pair<iterator, bool> insert_unique(V&& v) {
        size_type b;
        node* existing = find_node(key_of(v), b);
        if (existing) {
            return pair<iterator, bool>(iterator(this, existing, b), false);
        }
        node* n = make_node(mystl::forward<V>(v));
        link_node(n);
        maybe_rehash();

        size_type nb = bucket_for(key_of(n->value), buckets_.size());
        return pair<iterator, bool>(iterator(this, n, nb), true);
    }

    template<typename... Args>
    pair<iterator, bool> emplace_unique(Args&&... args) {

        node* n = make_node(mystl::forward<Args>(args)...);
        size_type b;
        node* existing = find_node(key_of(n->value), b);
        if (existing) {
            free_node(n);
            return pair<iterator, bool>(iterator(this, existing, b), false);
        }
        link_node(n);
        maybe_rehash();
        size_type nb = bucket_for(key_of(n->value), buckets_.size());
        return pair<iterator, bool>(iterator(this, n, nb), true);
    }

    size_type erase(const Key& k) {
        size_type b = bucket_for(k, buckets_.size());
        node* prev = nullptr;
        for (node* n = buckets_[b]; n; prev = n, n = n->next) {
            if (equal_(key_of(n->value), k)) {
                if (prev) {
                    prev->next = n->next;
                } else {
                    buckets_[b] = n->next;
                }
                free_node(n);
                --size_;
                return 1;
            }
        }
        return 0;
    }

    iterator erase(const_iterator pos) {
        node* target = pos.node_;
        size_type b = pos.bucket_;

        iterator next(this, target->next, b);
        next.skip_to_valid();

        node* prev = nullptr;
        for (node* n = buckets_[b]; n; prev = n, n = n->next) {
            if (n == target) {
                if (prev) {
                    prev->next = n->next;
                } else {
                    buckets_[b] = n->next;
                }
                free_node(n);
                --size_;
                break;
            }
        }
        return next;
    }

    void swap(hash_table& other) noexcept {

        bucket_vector tmp = mystl::move(buckets_);
        buckets_ = mystl::move(other.buckets_);
        other.buckets_ = mystl::move(tmp);
        mystl::swap(size_, other.size_);
        mystl::swap(max_load_factor_, other.max_load_factor_);
        mystl::swap(hash_, other.hash_);
        mystl::swap(equal_, other.equal_);
    }

    iterator find(const Key& k) {
        size_type b;
        node* n = find_node(k, b);
        if (n) {
            return iterator(this, n, b);
        }
        return end();
    }

    const_iterator find(const Key& k) const {
        size_type b;
        node* n = find_node(k, b);
        if (n) {
            return const_iterator(this, n, b);
        }
        return cend();
    }

    size_type count(const Key& k) const {
        size_type b;
        return find_node(k, b) ? 1 : 0;
    }

    bool contains(const Key& k) const {
        size_type b;
        return find_node(k, b) != nullptr;
    }

    size_type bucket_count() const noexcept {
        return buckets_.size();
    }

    float load_factor() const noexcept {
        return static_cast<float>(size_) / static_cast<float>(buckets_.size());
    }

    float max_load_factor() const noexcept {
        return max_load_factor_;
    }

    void max_load_factor(float ml) {
        max_load_factor_ = ml;
    }

    void rehash(size_type n) {
        size_type new_bc = round_up_pow2(n < 8 ? 8 : n);
        if (new_bc == buckets_.size()) {
            return;
        }
        bucket_vector new_buckets;
        new_buckets.assign(new_bc, nullptr);

        for (size_type b = 0; b < buckets_.size(); ++b) {
            node* n2 = buckets_[b];
            while (n2) {
                node* next = n2->next;
                size_type nb = hash_(key_of(n2->value)) & (new_bc - 1);
                n2->next = new_buckets[nb];
                new_buckets[nb] = n2;
                n2 = next;
            }
        }
        buckets_ = mystl::move(new_buckets);
    }

    void reserve(size_type n) {

        if (max_load_factor_ > 0.0f) {
            size_type needed =
                static_cast<size_type>(static_cast<float>(n) / max_load_factor_) + 1;
            if (needed > buckets_.size()) {
                rehash(needed);
            }
        }
    }

    hasher hash_function() const {
        return hash_;
    }

    key_equal key_eq() const {
        return equal_;
    }
};

}
}
