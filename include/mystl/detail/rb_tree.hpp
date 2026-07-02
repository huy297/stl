#pragma once

#include <cstddef>

#include "mystl/utility/pair.hpp"
#include "mystl/utility/utility.hpp"
#include "mystl/memory/memory.hpp"
#include "mystl/memory/allocator.hpp"
#include "mystl/iterator/iterator.hpp"

namespace mystl {
namespace detail {

enum rb_color { rb_red, rb_black };

struct rb_node_base {
    rb_color color;
    rb_node_base* parent;
    rb_node_base* left;
    rb_node_base* right;
};

template<typename Value>
struct rb_node : rb_node_base {
    Value value;
};

inline rb_node_base* rb_minimum(rb_node_base* x) noexcept {
    while (x->left != nullptr) {
        x = x->left;
    }
    return x;
}

inline rb_node_base* rb_maximum(rb_node_base* x) noexcept {
    while (x->right != nullptr) {
        x = x->right;
    }
    return x;
}

inline rb_node_base* rb_increment(rb_node_base* x) noexcept {
    if (x->right != nullptr) {
        return rb_minimum(x->right);
    }
    rb_node_base* y = x->parent;
    while (x == y->right) {
        x = y;
        y = y->parent;
    }

    if (x->right != y) {
        x = y;
    }
    return x;
}

inline rb_node_base* rb_decrement(rb_node_base* x) noexcept {

    if (x->color == rb_red && x->parent->parent == x) {
        return x->right;
    }
    if (x->left != nullptr) {
        return rb_maximum(x->left);
    }
    rb_node_base* y = x->parent;
    while (x == y->left) {
        x = y;
        y = y->parent;
    }
    return y;
}

inline void rb_rotate_left(rb_node_base* x, rb_node_base*& root) noexcept {
    rb_node_base* y = x->right;
    x->right = y->left;
    if (y->left != nullptr) {
        y->left->parent = x;
    }
    y->parent = x->parent;
    if (x == root) {
        root = y;
    } else if (x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }
    y->left = x;
    x->parent = y;
}

inline void rb_rotate_right(rb_node_base* x, rb_node_base*& root) noexcept {
    rb_node_base* y = x->left;
    x->left = y->right;
    if (y->right != nullptr) {
        y->right->parent = x;
    }
    y->parent = x->parent;
    if (x == root) {
        root = y;
    } else if (x == x->parent->right) {
        x->parent->right = y;
    } else {
        x->parent->left = y;
    }
    y->right = x;
    x->parent = y;
}

inline void rb_insert_rebalance(rb_node_base* z, rb_node_base*& root) noexcept {
    z->color = rb_red;
    while (z != root && z->parent->color == rb_red) {
        rb_node_base* gp = z->parent->parent;
        if (z->parent == gp->left) {
            rb_node_base* uncle = gp->right;
            if (uncle != nullptr && uncle->color == rb_red) {
                z->parent->color = rb_black;
                uncle->color = rb_black;
                gp->color = rb_red;
                z = gp;
            } else {
                if (z == z->parent->right) {
                    z = z->parent;
                    rb_rotate_left(z, root);
                }
                z->parent->color = rb_black;
                gp->color = rb_red;
                rb_rotate_right(gp, root);
            }
        } else {
            rb_node_base* uncle = gp->left;
            if (uncle != nullptr && uncle->color == rb_red) {
                z->parent->color = rb_black;
                uncle->color = rb_black;
                gp->color = rb_red;
                z = gp;
            } else {
                if (z == z->parent->left) {
                    z = z->parent;
                    rb_rotate_right(z, root);
                }
                z->parent->color = rb_black;
                gp->color = rb_red;
                rb_rotate_left(gp, root);
            }
        }
    }
    root->color = rb_black;
}

inline rb_node_base* rb_erase_rebalance(rb_node_base* z, rb_node_base* header) noexcept {
    rb_node_base*& root = header->parent;
    rb_node_base*& leftmost = header->left;
    rb_node_base*& rightmost = header->right;

    rb_node_base* y = z;
    rb_node_base* x = nullptr;
    rb_node_base* x_parent = nullptr;

    if (y->left == nullptr) {
        x = y->right;
    } else if (y->right == nullptr) {
        x = y->left;
    } else {

        y = rb_minimum(z->right);
        x = y->right;
    }

    if (y != z) {

        z->left->parent = y;
        y->left = z->left;
        if (y != z->right) {
            x_parent = y->parent;
            if (x != nullptr) {
                x->parent = y->parent;
            }
            y->parent->left = x;
            y->right = z->right;
            z->right->parent = y;
        } else {
            x_parent = y;
        }
        if (root == z) {
            root = y;
        } else if (z->parent->left == z) {
            z->parent->left = y;
        } else {
            z->parent->right = y;
        }
        y->parent = z->parent;
        rb_color tmp = y->color;
        y->color = z->color;
        z->color = tmp;
        y = z;
    } else {

        x_parent = y->parent;
        if (x != nullptr) {
            x->parent = y->parent;
        }
        if (root == z) {
            root = x;
        } else if (z->parent->left == z) {
            z->parent->left = x;
        } else {
            z->parent->right = x;
        }

        if (leftmost == z) {
            if (z->right == nullptr) {
                leftmost = z->parent;
            } else {
                leftmost = rb_minimum(x);
            }
        }
        if (rightmost == z) {
            if (z->left == nullptr) {
                rightmost = z->parent;
            } else {
                rightmost = rb_maximum(x);
            }
        }
    }

    if (y->color == rb_black) {
        while (x != root && (x == nullptr || x->color == rb_black)) {
            if (x == x_parent->left) {
                rb_node_base* w = x_parent->right;
                if (w->color == rb_red) {
                    w->color = rb_black;
                    x_parent->color = rb_red;
                    rb_rotate_left(x_parent, root);
                    w = x_parent->right;
                }
                if ((w->left == nullptr || w->left->color == rb_black) &&
                    (w->right == nullptr || w->right->color == rb_black)) {
                    w->color = rb_red;
                    x = x_parent;
                    x_parent = x_parent->parent;
                } else {
                    if (w->right == nullptr || w->right->color == rb_black) {
                        if (w->left != nullptr) {
                            w->left->color = rb_black;
                        }
                        w->color = rb_red;
                        rb_rotate_right(w, root);
                        w = x_parent->right;
                    }
                    w->color = x_parent->color;
                    x_parent->color = rb_black;
                    if (w->right != nullptr) {
                        w->right->color = rb_black;
                    }
                    rb_rotate_left(x_parent, root);
                    break;
                }
            } else {
                rb_node_base* w = x_parent->left;
                if (w->color == rb_red) {
                    w->color = rb_black;
                    x_parent->color = rb_red;
                    rb_rotate_right(x_parent, root);
                    w = x_parent->left;
                }
                if ((w->right == nullptr || w->right->color == rb_black) &&
                    (w->left == nullptr || w->left->color == rb_black)) {
                    w->color = rb_red;
                    x = x_parent;
                    x_parent = x_parent->parent;
                } else {
                    if (w->left == nullptr || w->left->color == rb_black) {
                        if (w->right != nullptr) {
                            w->right->color = rb_black;
                        }
                        w->color = rb_red;
                        rb_rotate_left(w, root);
                        w = x_parent->left;
                    }
                    w->color = x_parent->color;
                    x_parent->color = rb_black;
                    if (w->left != nullptr) {
                        w->left->color = rb_black;
                    }
                    rb_rotate_right(x_parent, root);
                    break;
                }
            }
        }
        if (x != nullptr) {
            x->color = rb_black;
        }
    }

    return y;
}

template<typename Key, typename Value, typename KeyOfValue, typename Compare>
class rb_tree {
public:
    using key_type = Key;
    using value_type = Value;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = Value&;
    using const_reference = const Value&;
    using pointer = Value*;
    using const_pointer = const Value*;

private:
    using node = rb_node<Value>;
    using node_base = rb_node_base;
    using node_allocator = allocator<node>;

    node_base header_;
    size_type count_ = 0;
    Compare comp_{};
    node_allocator alloc_{};

    node* get_node() {
        return alloc_.allocate(1);
    }

    void put_node(node* p) noexcept {
        alloc_.deallocate(p, 1);
    }

    template<typename... Args>
    node* create_node(Args&&... args) {
        node* p = get_node();
        try {
            mystl::construct_at(mystl::addressof(p->value), mystl::forward<Args>(args)...);
        } catch (...) {
            put_node(p);
            throw;
        }
        p->left = nullptr;
        p->right = nullptr;
        return p;
    }

    void destroy_node(node* p) noexcept {
        mystl::destroy_at(mystl::addressof(p->value));
        put_node(p);
    }

    node* root() const noexcept {
        return static_cast<node*>(header_.parent);
    }

    node_base* header_ptr() const noexcept {

        return const_cast<node_base*>(&header_);
    }

    static const Key& key_of(node_base* n) {
        return KeyOfValue()(static_cast<node*>(n)->value);
    }

    void reset_header() noexcept {
        header_.color = rb_red;
        header_.parent = nullptr;
        header_.left = &header_;
        header_.right = &header_;
    }

    node* copy_subtree(node* src, node_base* p) {
        node* top = create_node(src->value);
        top->parent = p;
        top->color = src->color;
        try {
            if (src->right != nullptr) {
                top->right = copy_subtree(static_cast<node*>(src->right), top);
            } else {
                top->right = nullptr;
            }
            if (src->left != nullptr) {
                top->left = copy_subtree(static_cast<node*>(src->left), top);
            } else {
                top->left = nullptr;
            }
        } catch (...) {
            erase_subtree(top);
            throw;
        }
        return top;
    }

    void erase_subtree(node* x) noexcept {
        while (x != nullptr) {
            erase_subtree(static_cast<node*>(x->right));
            node* left = static_cast<node*>(x->left);
            destroy_node(x);
            x = left;
        }
    }

public:

    class const_iterator;

    class iterator {
    public:
        using iterator_category = mystl::bidirectional_iterator_tag;
        using value_type = Value;
        using difference_type = std::ptrdiff_t;
        using pointer = Value*;
        using reference = Value&;

        iterator() noexcept : node_(nullptr) {}
        explicit iterator(node_base* n) noexcept : node_(n) {}

        reference operator*() const {
            return static_cast<node*>(node_)->value;
        }

        pointer operator->() const {
            return mystl::addressof(static_cast<node*>(node_)->value);
        }

        iterator& operator++() {
            node_ = rb_increment(node_);
            return *this;
        }

        iterator operator++(int) {
            iterator tmp = *this;
            node_ = rb_increment(node_);
            return tmp;
        }

        iterator& operator--() {
            node_ = rb_decrement(node_);
            return *this;
        }

        iterator operator--(int) {
            iterator tmp = *this;
            node_ = rb_decrement(node_);
            return tmp;
        }

        bool operator==(const iterator& other) const noexcept {
            return node_ == other.node_;
        }

        bool operator!=(const iterator& other) const noexcept {
            return node_ != other.node_;
        }

        node_base* base() const noexcept {
            return node_;
        }

    private:
        friend class const_iterator;
        friend class rb_tree;
        node_base* node_;
    };

    class const_iterator {
    public:
        using iterator_category = mystl::bidirectional_iterator_tag;
        using value_type = Value;
        using difference_type = std::ptrdiff_t;
        using pointer = const Value*;
        using reference = const Value&;

        const_iterator() noexcept : node_(nullptr) {}
        explicit const_iterator(node_base* n) noexcept : node_(n) {}
        const_iterator(const iterator& it) noexcept : node_(it.base()) {}

        reference operator*() const {
            return static_cast<node*>(node_)->value;
        }

        pointer operator->() const {
            return mystl::addressof(static_cast<node*>(node_)->value);
        }

        const_iterator& operator++() {
            node_ = rb_increment(node_);
            return *this;
        }

        const_iterator operator++(int) {
            const_iterator tmp = *this;
            node_ = rb_increment(node_);
            return tmp;
        }

        const_iterator& operator--() {
            node_ = rb_decrement(node_);
            return *this;
        }

        const_iterator operator--(int) {
            const_iterator tmp = *this;
            node_ = rb_decrement(node_);
            return tmp;
        }

        bool operator==(const const_iterator& other) const noexcept {
            return node_ == other.node_;
        }

        bool operator!=(const const_iterator& other) const noexcept {
            return node_ != other.node_;
        }

        node_base* base() const noexcept {
            return node_;
        }

    private:
        friend class rb_tree;
        node_base* node_;
    };

    using reverse_iterator = mystl::reverse_iterator<iterator>;
    using const_reverse_iterator = mystl::reverse_iterator<const_iterator>;

    rb_tree() {
        reset_header();
    }

    explicit rb_tree(const Compare& comp) : comp_(comp) {
        reset_header();
    }

    rb_tree(const rb_tree& other) : comp_(other.comp_) {
        reset_header();
        if (other.root() != nullptr) {
            node* new_root = copy_subtree(other.root(), &header_);
            header_.parent = new_root;
            header_.left = rb_minimum(new_root);
            header_.right = rb_maximum(new_root);
            count_ = other.count_;
        }
    }

    rb_tree(rb_tree&& other) noexcept : comp_(other.comp_) {
        reset_header();
        move_from(other);
    }

    ~rb_tree() {
        clear();
    }

    rb_tree& operator=(const rb_tree& other) {
        if (this != &other) {
            rb_tree tmp(other);
            swap(tmp);
        }
        return *this;
    }

    rb_tree& operator=(rb_tree&& other) noexcept {
        if (this != &other) {
            clear();
            comp_ = other.comp_;
            move_from(other);
        }
        return *this;
    }

private:

    void move_from(rb_tree& other) noexcept {
        if (other.root() != nullptr) {
            header_.parent = other.header_.parent;
            header_.parent->parent = &header_;
            header_.left = other.header_.left;
            header_.right = other.header_.right;
            count_ = other.count_;
            other.reset_header();
            other.count_ = 0;
        }
    }

public:

    size_type size() const noexcept {
        return count_;
    }

    bool empty() const noexcept {
        return count_ == 0;
    }

    iterator begin() noexcept {
        return iterator(header_.left);
    }

    const_iterator begin() const noexcept {
        return const_iterator(header_ptr()->left);
    }

    iterator end() noexcept {
        return iterator(&header_);
    }

    const_iterator end() const noexcept {
        return const_iterator(header_ptr());
    }

    const_iterator cbegin() const noexcept {
        return begin();
    }

    const_iterator cend() const noexcept {
        return end();
    }

    reverse_iterator rbegin() noexcept {
        return reverse_iterator(end());
    }

    const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(end());
    }

    reverse_iterator rend() noexcept {
        return reverse_iterator(begin());
    }

    const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(begin());
    }

    const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator(end());
    }

    const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator(begin());
    }

    void clear() noexcept {
        if (root() != nullptr) {
            erase_subtree(root());
            reset_header();
            count_ = 0;
        }
    }

    void swap(rb_tree& other) noexcept {
        if (this == &other) {
            return;
        }
        bool this_empty = (root() == nullptr);
        bool other_empty = (other.root() == nullptr);

        node_base* this_root = header_.parent;
        node_base* this_left = header_.left;
        node_base* this_right = header_.right;
        node_base* other_root = other.header_.parent;
        node_base* other_left = other.header_.left;
        node_base* other_right = other.header_.right;

        if (other_empty) {
            reset_header();
        } else {
            header_.parent = other_root;
            other_root->parent = &header_;
            header_.left = other_left;
            header_.right = other_right;
        }
        if (this_empty) {
            other.reset_header();
        } else {
            other.header_.parent = this_root;
            this_root->parent = &other.header_;
            other.header_.left = this_left;
            other.header_.right = this_right;
        }
        mystl::swap(count_, other.count_);
        mystl::swap(comp_, other.comp_);
    }

private:

    iterator insert_node(node_base* p, node* z, bool insert_left) {
        z->parent = p;
        if (insert_left) {
            p->left = z;
            if (p == &header_) {
                header_.parent = z;
                header_.right = z;
            } else if (p == header_.left) {
                header_.left = z;
            }
        } else {
            p->right = z;
            if (p == header_.right) {
                header_.right = z;
            }
        }
        rb_node_base* r = header_.parent;
        rb_insert_rebalance(z, r);
        header_.parent = r;
        header_.parent->parent = &header_;
        ++count_;
        return iterator(z);
    }

public:
    template<typename... Args>
    pair<iterator, bool> emplace_unique(Args&&... args) {
        node* z = create_node(mystl::forward<Args>(args)...);
        const Key& k = KeyOfValue()(z->value);

        node_base* p = &header_;
        node_base* cur = header_.parent;
        bool insert_left = true;
        while (cur != nullptr) {
            p = cur;
            if (comp_(k, key_of(cur))) {
                insert_left = true;
                cur = cur->left;
            } else if (comp_(key_of(cur), k)) {
                insert_left = false;
                cur = cur->right;
            } else {

                destroy_node(z);
                return pair<iterator, bool>(iterator(p), false);
            }
        }
        return pair<iterator, bool>(insert_node(p, z, insert_left), true);
    }

    pair<iterator, bool> insert_unique(const value_type& v) {
        const Key& k = KeyOfValue()(v);
        node_base* p = &header_;
        node_base* cur = header_.parent;
        bool insert_left = true;
        while (cur != nullptr) {
            p = cur;
            if (comp_(k, key_of(cur))) {
                insert_left = true;
                cur = cur->left;
            } else if (comp_(key_of(cur), k)) {
                insert_left = false;
                cur = cur->right;
            } else {
                return pair<iterator, bool>(iterator(p), false);
            }
        }
        node* z = create_node(v);
        return pair<iterator, bool>(insert_node(p, z, insert_left), true);
    }

    pair<iterator, bool> insert_unique(value_type&& v) {
        const Key& k = KeyOfValue()(v);
        node_base* p = &header_;
        node_base* cur = header_.parent;
        bool insert_left = true;
        while (cur != nullptr) {
            p = cur;
            if (comp_(k, key_of(cur))) {
                insert_left = true;
                cur = cur->left;
            } else if (comp_(key_of(cur), k)) {
                insert_left = false;
                cur = cur->right;
            } else {
                return pair<iterator, bool>(iterator(p), false);
            }
        }
        node* z = create_node(mystl::move(v));
        return pair<iterator, bool>(insert_node(p, z, insert_left), true);
    }

    iterator erase(iterator pos) {
        iterator next = pos;
        ++next;
        node* z = static_cast<node*>(pos.base());
        rb_erase_rebalance(z, &header_);
        destroy_node(z);
        --count_;
        return next;
    }

    iterator erase(const_iterator pos) {
        const_iterator next = pos;
        ++next;
        node* z = static_cast<node*>(pos.base());
        rb_erase_rebalance(z, &header_);
        destroy_node(z);
        --count_;
        return iterator(next.base());
    }

    size_type erase(const key_type& k) {
        pair<iterator, iterator> range = equal_range(k);
        size_type removed = 0;
        while (range.first != range.second) {
            range.first = erase(range.first);
            ++removed;
        }
        return removed;
    }

    iterator find(const key_type& k) {
        node_base* j = lower_bound_node(k);
        if (j == &header_ || comp_(k, key_of(j))) {
            return end();
        }
        return iterator(j);
    }

    const_iterator find(const key_type& k) const {
        node_base* j = lower_bound_node(k);
        if (j == header_ptr() || comp_(k, key_of(j))) {
            return end();
        }
        return const_iterator(j);
    }

    size_type count(const key_type& k) const {
        return find(k) == end() ? 0 : 1;
    }

    bool contains(const key_type& k) const {
        return find(k) != end();
    }

    iterator lower_bound(const key_type& k) {
        return iterator(lower_bound_node(k));
    }

    const_iterator lower_bound(const key_type& k) const {
        return const_iterator(lower_bound_node(k));
    }

    iterator upper_bound(const key_type& k) {
        return iterator(upper_bound_node(k));
    }

    const_iterator upper_bound(const key_type& k) const {
        return const_iterator(upper_bound_node(k));
    }

    pair<iterator, iterator> equal_range(const key_type& k) {
        return pair<iterator, iterator>(lower_bound(k), upper_bound(k));
    }

    pair<const_iterator, const_iterator> equal_range(const key_type& k) const {
        return pair<const_iterator, const_iterator>(lower_bound(k), upper_bound(k));
    }

    const Compare& key_comp() const noexcept {
        return comp_;
    }

private:

    node_base* lower_bound_node(const key_type& k) const {
        node_base* y = header_ptr();
        node_base* x = header_ptr()->parent;
        while (x != nullptr) {
            if (!comp_(key_of(x), k)) {
                y = x;
                x = x->left;
            } else {
                x = x->right;
            }
        }
        return y;
    }

    node_base* upper_bound_node(const key_type& k) const {
        node_base* y = header_ptr();
        node_base* x = header_ptr()->parent;
        while (x != nullptr) {
            if (comp_(k, key_of(x))) {
                y = x;
                x = x->left;
            } else {
                x = x->right;
            }
        }
        return y;
    }
};

}
}
