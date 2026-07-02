#pragma once

#include <cstddef>
#include <new>
#include <initializer_list>

#include "mystl/utility/utility.hpp"
#include "mystl/memory/memory.hpp"
#include "mystl/iterator/iterator.hpp"

namespace mystl {

template<typename T>
class list {
private:
    struct node_base {
        node_base* prev;
        node_base* next;
    };

    struct node : node_base {
        T value;
    };

public:
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = T&;
    using const_reference = const T&;
    using pointer = T*;
    using const_pointer = const T*;

    class const_iterator;

    class iterator {
    public:
        using iterator_category = mystl::bidirectional_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        iterator() : node_(nullptr) {}

        explicit iterator(node_base* n) : node_(n) {}

        reference operator*() const {
            return static_cast<node*>(node_)->value;
        }

        pointer operator->() const {
            return &static_cast<node*>(node_)->value;
        }

        iterator& operator++() {
            node_ = node_->next;
            return *this;
        }

        iterator operator++(int) {
            iterator tmp = *this;
            node_ = node_->next;
            return tmp;
        }

        iterator& operator--() {
            node_ = node_->prev;
            return *this;
        }

        iterator operator--(int) {
            iterator tmp = *this;
            node_ = node_->prev;
            return tmp;
        }

        bool operator==(const iterator& other) const {
            return node_ == other.node_;
        }

        bool operator!=(const iterator& other) const {
            return node_ != other.node_;
        }

    private:
        node_base* node_;

        friend class list;
        friend class const_iterator;
    };

    class const_iterator {
    public:
        using iterator_category = mystl::bidirectional_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = const T*;
        using reference = const T&;

        const_iterator() : node_(nullptr) {}

        explicit const_iterator(const node_base* n) : node_(n) {}

        const_iterator(const iterator& it) : node_(it.node_) {}

        reference operator*() const {
            return static_cast<const node*>(node_)->value;
        }

        pointer operator->() const {
            return &static_cast<const node*>(node_)->value;
        }

        const_iterator& operator++() {
            node_ = node_->next;
            return *this;
        }

        const_iterator operator++(int) {
            const_iterator tmp = *this;
            node_ = node_->next;
            return tmp;
        }

        const_iterator& operator--() {
            node_ = node_->prev;
            return *this;
        }

        const_iterator operator--(int) {
            const_iterator tmp = *this;
            node_ = node_->prev;
            return tmp;
        }

        bool operator==(const const_iterator& other) const {
            return node_ == other.node_;
        }

        bool operator!=(const const_iterator& other) const {
            return node_ != other.node_;
        }

    private:
        const node_base* node_;

        friend class list;
    };

    using reverse_iterator = mystl::reverse_iterator<iterator>;
    using const_reverse_iterator = mystl::reverse_iterator<const_iterator>;

private:
    node_base sentinel_;
    size_type size_ = 0;

    void init_sentinel() noexcept {
        sentinel_.prev = &sentinel_;
        sentinel_.next = &sentinel_;
    }

    static node* allocate_node() {
        return static_cast<node*>(::operator new(sizeof(node)));
    }

    static void deallocate_node(node* n) noexcept {
        ::operator delete(n);
    }

    template<typename... Args>
    node* make_node(Args&&... args) {
        node* n = allocate_node();
        try {
            mystl::construct_at(&n->value, mystl::forward<Args>(args)...);
        } catch (...) {
            deallocate_node(n);
            throw;
        }
        return n;
    }

    void free_node(node* n) noexcept {
        mystl::destroy_at(&n->value);
        deallocate_node(n);
    }

    void link_before(node_base* pos, node_base* n) noexcept {
        n->prev = pos->prev;
        n->next = pos;
        pos->prev->next = n;
        pos->prev = n;
    }

    static void unlink(node_base* n) noexcept {
        n->prev->next = n->next;
        n->next->prev = n->prev;
    }

    node_base* sentinel() noexcept {
        return &sentinel_;
    }

    const node_base* sentinel() const noexcept {
        return &sentinel_;
    }

public:
    list() {
        init_sentinel();
    }

    explicit list(size_type n) {
        init_sentinel();
        try {
            for (size_type i = 0; i < n; ++i) {
                emplace_back();
            }
        } catch (...) {
            clear();
            throw;
        }
    }

    list(size_type n, const T& value) {
        init_sentinel();
        try {
            for (size_type i = 0; i < n; ++i) {
                push_back(value);
            }
        } catch (...) {
            clear();
            throw;
        }
    }

    list(std::initializer_list<T> init) {
        init_sentinel();
        try {
            for (const T& x : init) {
                push_back(x);
            }
        } catch (...) {
            clear();
            throw;
        }
    }

    list(const list& other) {
        init_sentinel();
        try {
            for (const T& x : other) {
                push_back(x);
            }
        } catch (...) {
            clear();
            throw;
        }
    }

    list(list&& other) noexcept {
        init_sentinel();
        splice_all(other);
    }

    ~list() {
        clear();
    }

    list& operator=(const list& other) {
        if (this != &other) {
            list tmp(other);
            swap(tmp);
        }
        return *this;
    }

    list& operator=(list&& other) noexcept {
        if (this != &other) {
            clear();
            splice_all(other);
        }
        return *this;
    }

    list& operator=(std::initializer_list<T> init) {
        list tmp(init);
        swap(tmp);
        return *this;
    }

    iterator begin() noexcept {
        return iterator(sentinel_.next);
    }

    const_iterator begin() const noexcept {
        return const_iterator(sentinel_.next);
    }

    iterator end() noexcept {
        return iterator(&sentinel_);
    }

    const_iterator end() const noexcept {
        return const_iterator(&sentinel_);
    }

    const_iterator cbegin() const noexcept {
        return const_iterator(sentinel_.next);
    }

    const_iterator cend() const noexcept {
        return const_iterator(&sentinel_);
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
        return const_reverse_iterator(cend());
    }

    const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator(cbegin());
    }

    bool empty() const noexcept {
        return size_ == 0;
    }

    size_type size() const noexcept {
        return size_;
    }

    reference front() {
        return static_cast<node*>(sentinel_.next)->value;
    }

    const_reference front() const {
        return static_cast<const node*>(sentinel_.next)->value;
    }

    reference back() {
        return static_cast<node*>(sentinel_.prev)->value;
    }

    const_reference back() const {
        return static_cast<const node*>(sentinel_.prev)->value;
    }

    void clear() noexcept {
        node_base* cur = sentinel_.next;
        while (cur != &sentinel_) {
            node_base* nxt = cur->next;
            free_node(static_cast<node*>(cur));
            cur = nxt;
        }
        init_sentinel();
        size_ = 0;
    }

    template<typename... Args>
    reference emplace_back(Args&&... args) {
        node* n = make_node(mystl::forward<Args>(args)...);
        link_before(&sentinel_, n);
        ++size_;
        return n->value;
    }

    template<typename... Args>
    reference emplace_front(Args&&... args) {
        node* n = make_node(mystl::forward<Args>(args)...);
        link_before(sentinel_.next, n);
        ++size_;
        return n->value;
    }

    void push_back(const T& value) {
        emplace_back(value);
    }

    void push_back(T&& value) {
        emplace_back(mystl::move(value));
    }

    void push_front(const T& value) {
        emplace_front(value);
    }

    void push_front(T&& value) {
        emplace_front(mystl::move(value));
    }

    void pop_back() {
        node_base* n = sentinel_.prev;
        unlink(n);
        free_node(static_cast<node*>(n));
        --size_;
    }

    void pop_front() {
        node_base* n = sentinel_.next;
        unlink(n);
        free_node(static_cast<node*>(n));
        --size_;
    }

    iterator insert(const_iterator pos, const T& value) {
        node* n = make_node(value);
        node_base* p = const_cast<node_base*>(pos.node_);
        link_before(p, n);
        ++size_;
        return iterator(n);
    }

    iterator insert(const_iterator pos, T&& value) {
        node* n = make_node(mystl::move(value));
        node_base* p = const_cast<node_base*>(pos.node_);
        link_before(p, n);
        ++size_;
        return iterator(n);
    }

    iterator erase(const_iterator pos) {
        node_base* n = const_cast<node_base*>(pos.node_);
        node_base* nxt = n->next;
        unlink(n);
        free_node(static_cast<node*>(n));
        --size_;
        return iterator(nxt);
    }

    iterator erase(const_iterator first, const_iterator last) {
        node_base* f = const_cast<node_base*>(first.node_);
        node_base* l = const_cast<node_base*>(last.node_);
        while (f != l) {
            node_base* nxt = f->next;
            unlink(f);
            free_node(static_cast<node*>(f));
            --size_;
            f = nxt;
        }
        return iterator(l);
    }

    void resize(size_type count) {
        while (size_ > count) {
            pop_back();
        }
        while (size_ < count) {
            emplace_back();
        }
    }

    void resize(size_type count, const T& value) {
        while (size_ > count) {
            pop_back();
        }
        while (size_ < count) {
            push_back(value);
        }
    }

    void swap(list& other) noexcept {
        if (this == &other) {
            return;
        }
        if (size_ == 0 && other.size_ == 0) {
            return;
        }

        node_base* this_first = sentinel_.next;
        node_base* this_last = sentinel_.prev;
        node_base* other_first = other.sentinel_.next;
        node_base* other_last = other.sentinel_.prev;

        if (other.size_ != 0) {
            sentinel_.next = other_first;
            sentinel_.prev = other_last;
            other_first->prev = &sentinel_;
            other_last->next = &sentinel_;
        } else {
            init_sentinel();
        }

        if (size_ != 0) {
            other.sentinel_.next = this_first;
            other.sentinel_.prev = this_last;
            this_first->prev = &other.sentinel_;
            this_last->next = &other.sentinel_;
        } else {
            other.init_sentinel();
        }

        mystl::swap(size_, other.size_);
    }

private:

    void splice_all(list& other) noexcept {
        if (other.size_ == 0) {
            return;
        }
        node_base* first = other.sentinel_.next;
        node_base* last = other.sentinel_.prev;
        node_base* at = &sentinel_;

        first->prev = at->prev;
        at->prev->next = first;
        last->next = at;
        at->prev = last;

        size_ += other.size_;
        other.init_sentinel();
        other.size_ = 0;
    }

public:
    void splice(const_iterator pos, list& other) {
        if (this == &other || other.size_ == 0) {
            return;
        }
        node_base* p = const_cast<node_base*>(pos.node_);
        node_base* first = other.sentinel_.next;
        node_base* last = other.sentinel_.prev;

        first->prev = p->prev;
        p->prev->next = first;
        last->next = p;
        p->prev = last;

        size_ += other.size_;
        other.init_sentinel();
        other.size_ = 0;
    }

    void remove(const T& value) {
        node_base* cur = sentinel_.next;
        while (cur != &sentinel_) {
            node_base* nxt = cur->next;
            if (static_cast<node*>(cur)->value == value) {
                unlink(cur);
                free_node(static_cast<node*>(cur));
                --size_;
            }
            cur = nxt;
        }
    }

    template<typename UnaryPredicate>
    void remove_if(UnaryPredicate pred) {
        node_base* cur = sentinel_.next;
        while (cur != &sentinel_) {
            node_base* nxt = cur->next;
            if (pred(static_cast<node*>(cur)->value)) {
                unlink(cur);
                free_node(static_cast<node*>(cur));
                --size_;
            }
            cur = nxt;
        }
    }

    void unique() {
        if (size_ < 2) {
            return;
        }
        node_base* cur = sentinel_.next;
        while (cur->next != &sentinel_) {
            node_base* nxt = cur->next;
            if (static_cast<node*>(cur)->value == static_cast<node*>(nxt)->value) {
                unlink(nxt);
                free_node(static_cast<node*>(nxt));
                --size_;
            } else {
                cur = nxt;
            }
        }
    }

    template<typename BinaryPredicate>
    void unique(BinaryPredicate pred) {
        if (size_ < 2) {
            return;
        }
        node_base* cur = sentinel_.next;
        while (cur->next != &sentinel_) {
            node_base* nxt = cur->next;
            if (pred(static_cast<node*>(cur)->value, static_cast<node*>(nxt)->value)) {
                unlink(nxt);
                free_node(static_cast<node*>(nxt));
                --size_;
            } else {
                cur = nxt;
            }
        }
    }

    void reverse() noexcept {
        if (size_ < 2) {
            return;
        }
        node_base* cur = &sentinel_;
        do {
            mystl::swap(cur->prev, cur->next);
            cur = cur->prev;
        } while (cur != &sentinel_);
    }

    template<typename Compare>
    void merge(list& other, Compare comp) {
        if (this == &other || other.size_ == 0) {
            return;
        }
        node_base* a = sentinel_.next;
        node_base* b = other.sentinel_.next;
        node_base* a_end = &sentinel_;
        node_base* b_end = &other.sentinel_;

        while (a != a_end && b != b_end) {
            if (comp(static_cast<node*>(b)->value, static_cast<node*>(a)->value)) {

                node_base* b_next = b->next;
                b->prev = a->prev;
                a->prev->next = b;
                b->next = a;
                a->prev = b;
                b = b_next;
            } else {
                a = a->next;
            }
        }

        if (b != b_end) {

            node_base* last = b_end->prev;
            node_base* p = a_end;
            b->prev = p->prev;
            p->prev->next = b;
            last->next = p;
            p->prev = last;
        }

        size_ += other.size_;
        other.init_sentinel();
        other.size_ = 0;
    }

    void merge(list& other) {
        merge(other, [](const T& x, const T& y) { return x < y; });
    }

    template<typename Compare>
    void sort(Compare comp) {
        if (size_ < 2) {
            return;
        }

        node_base* head = sentinel_.next;
        sentinel_.prev->next = nullptr;

        const size_type total = size_;
        for (size_type width = 1; width < total; width *= 2) {
            node_base* new_head = nullptr;
            node_base* tail = nullptr;
            node_base* cur = head;

            while (cur != nullptr) {
                node_base* left = cur;
                node_base* right = split(left, width);
                cur = split(right, width);
                node_base* merged = merge_runs(left, right, comp);

                if (new_head == nullptr) {
                    new_head = merged;
                } else {
                    tail->next = merged;
                }

                tail = merged;
                while (tail->next != nullptr) {
                    tail = tail->next;
                }
            }
            head = new_head;
        }

        relink(head);
    }

    void sort() {
        sort([](const T& x, const T& y) { return x < y; });
    }

private:

    static node_base* split(node_base* n, size_type width) {
        if (n == nullptr) {
            return nullptr;
        }
        for (size_type i = 1; i < width && n->next != nullptr; ++i) {
            n = n->next;
        }
        node_base* rest = n->next;
        n->next = nullptr;
        return rest;
    }

    template<typename Compare>
    static node_base* merge_runs(node_base* a, node_base* b, Compare& comp) {
        node_base dummy;
        node_base* tail = &dummy;
        dummy.next = nullptr;

        while (a != nullptr && b != nullptr) {
            if (comp(static_cast<node*>(b)->value, static_cast<node*>(a)->value)) {
                tail->next = b;
                b = b->next;
            } else {
                tail->next = a;
                a = a->next;
            }
            tail = tail->next;
        }
        tail->next = (a != nullptr) ? a : b;
        return dummy.next;
    }

    void relink(node_base* head) noexcept {
        node_base* prev = &sentinel_;
        node_base* cur = head;
        while (cur != nullptr) {
            cur->prev = prev;
            prev->next = cur;
            prev = cur;
            cur = cur->next;
        }
        prev->next = &sentinel_;
        sentinel_.prev = prev;
    }
};

template<typename T>
bool operator==(const list<T>& a, const list<T>& b) {
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

template<typename T>
bool operator!=(const list<T>& a, const list<T>& b) {
    return !(a == b);
}

template<typename T>
bool operator<(const list<T>& a, const list<T>& b) {
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
    return (ia == a.end()) && (ib != b.end());
}

template<typename T>
bool operator>(const list<T>& a, const list<T>& b) {
    return b < a;
}

template<typename T>
bool operator<=(const list<T>& a, const list<T>& b) {
    return !(b < a);
}

template<typename T>
bool operator>=(const list<T>& a, const list<T>& b) {
    return !(a < b);
}

template<typename T>
void swap(list<T>& a, list<T>& b) noexcept {
    a.swap(b);
}

}
