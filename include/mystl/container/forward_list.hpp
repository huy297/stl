#pragma once

#include <cstddef>
#include <initializer_list>

#include "mystl/utility/utility.hpp"
#include "mystl/memory/memory.hpp"
#include "mystl/iterator/iterator.hpp"

namespace mystl {

template<typename T>
class forward_list {
private:
    struct node_base {
        node_base* next = nullptr;
    };

    struct node : node_base {
        T value;

        template<typename... Args>
        explicit node(Args&&... args) : value(mystl::forward<Args>(args)...) {}
    };

    node_base head_{};

    static node* as_node(node_base* p) {
        return static_cast<node*>(p);
    }

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
        using iterator_category = mystl::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        iterator() = default;

        explicit iterator(node_base* p) : node_(p) {}

        reference operator*() const {
            return as_node(node_)->value;
        }

        pointer operator->() const {
            return mystl::addressof(as_node(node_)->value);
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

        bool operator==(const iterator& other) const {
            return node_ == other.node_;
        }

        bool operator!=(const iterator& other) const {
            return node_ != other.node_;
        }

    private:
        friend class forward_list;
        friend class const_iterator;
        node_base* node_ = nullptr;
    };

    class const_iterator {
    public:
        using iterator_category = mystl::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = const T*;
        using reference = const T&;

        const_iterator() = default;

        explicit const_iterator(const node_base* p) : node_(p) {}

        const_iterator(const iterator& other) : node_(other.node_) {}

        reference operator*() const {
            return static_cast<const node*>(node_)->value;
        }

        pointer operator->() const {
            return mystl::addressof(static_cast<const node*>(node_)->value);
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

        bool operator==(const const_iterator& other) const {
            return node_ == other.node_;
        }

        bool operator!=(const const_iterator& other) const {
            return node_ != other.node_;
        }

    private:
        friend class forward_list;
        const node_base* node_ = nullptr;
    };

private:

    node_base* mutable_after(const_iterator pos) {
        node_base* p = &head_;
        while (p != pos.node_) {
            p = p->next;
        }
        return p;
    }

    template<typename... Args>
    node* make_node(Args&&... args) {
        node* n = static_cast<node*>(::operator new(sizeof(node)));
        try {
            mystl::construct_at(n, mystl::forward<Args>(args)...);
        } catch (...) {
            ::operator delete(n);
            throw;
        }
        return n;
    }

    void free_node(node* n) noexcept {
        mystl::destroy_at(n);
        ::operator delete(n);
    }

    node* insert_node_after(node_base* prev, node* n) noexcept {
        n->next = prev->next;
        prev->next = n;
        return n;
    }

public:
    forward_list() = default;

    explicit forward_list(size_type count) {
        node_base* tail = &head_;
        for (size_type i = 0; i < count; ++i) {
            tail = insert_node_after(tail, make_node());
        }
    }

    forward_list(size_type count, const T& value) {
        node_base* tail = &head_;
        for (size_type i = 0; i < count; ++i) {
            tail = insert_node_after(tail, make_node(value));
        }
    }

    forward_list(std::initializer_list<T> init) {
        node_base* tail = &head_;
        for (const T& x : init) {
            tail = insert_node_after(tail, make_node(x));
        }
    }

    forward_list(const forward_list& other) {
        node_base* tail = &head_;
        for (node_base* p = other.head_.next; p; p = p->next) {
            tail = insert_node_after(tail, make_node(as_node(p)->value));
        }
    }

    forward_list(forward_list&& other) noexcept {
        head_.next = other.head_.next;
        other.head_.next = nullptr;
    }

    ~forward_list() {
        clear();
    }

    forward_list& operator=(const forward_list& other) {
        if (this != &other) {
            forward_list tmp(other);
            swap(tmp);
        }
        return *this;
    }

    forward_list& operator=(forward_list&& other) noexcept {
        if (this != &other) {
            clear();
            head_.next = other.head_.next;
            other.head_.next = nullptr;
        }
        return *this;
    }

    forward_list& operator=(std::initializer_list<T> init) {
        forward_list tmp(init);
        swap(tmp);
        return *this;
    }

    iterator before_begin() {
        return iterator(&head_);
    }

    const_iterator before_begin() const {
        return const_iterator(&head_);
    }

    const_iterator cbefore_begin() const {
        return const_iterator(&head_);
    }

    iterator begin() {
        return iterator(head_.next);
    }

    const_iterator begin() const {
        return const_iterator(head_.next);
    }

    const_iterator cbegin() const {
        return const_iterator(head_.next);
    }

    iterator end() {
        return iterator(nullptr);
    }

    const_iterator end() const {
        return const_iterator(nullptr);
    }

    const_iterator cend() const {
        return const_iterator(nullptr);
    }

    reference front() {
        return as_node(head_.next)->value;
    }

    const_reference front() const {
        return as_node(head_.next)->value;
    }

    bool empty() const {
        return head_.next == nullptr;
    }

    void clear() noexcept {
        node_base* p = head_.next;
        while (p) {
            node_base* next = p->next;
            free_node(as_node(p));
            p = next;
        }
        head_.next = nullptr;
    }

    void push_front(const T& value) {
        insert_node_after(&head_, make_node(value));
    }

    void push_front(T&& value) {
        insert_node_after(&head_, make_node(mystl::move(value)));
    }

    template<typename... Args>
    reference emplace_front(Args&&... args) {
        node* n = make_node(mystl::forward<Args>(args)...);
        insert_node_after(&head_, n);
        return n->value;
    }

    void pop_front() {
        node_base* first = head_.next;
        head_.next = first->next;
        free_node(as_node(first));
    }

    iterator insert_after(const_iterator pos, const T& value) {
        node_base* prev = mutable_after(pos);
        return iterator(insert_node_after(prev, make_node(value)));
    }

    iterator insert_after(const_iterator pos, T&& value) {
        node_base* prev = mutable_after(pos);
        return iterator(insert_node_after(prev, make_node(mystl::move(value))));
    }

    template<typename... Args>
    iterator emplace_after(const_iterator pos, Args&&... args) {
        node_base* prev = mutable_after(pos);
        return iterator(insert_node_after(prev,
            make_node(mystl::forward<Args>(args)...)));
    }

    iterator erase_after(const_iterator pos) {
        node_base* prev = mutable_after(pos);
        node_base* target = prev->next;
        if (target) {
            prev->next = target->next;
            free_node(as_node(target));
        }
        return iterator(prev->next);
    }

    iterator erase_after(const_iterator first, const_iterator last) {
        node_base* prev = mutable_after(first);
        node_base* p = prev->next;
        while (p != last.node_) {
            node_base* next = p->next;
            free_node(as_node(p));
            p = next;
        }
        prev->next = p;
        return iterator(p);
    }

    void resize(size_type count) {
        node_base* p = &head_;
        size_type n = 0;
        while (p->next && n < count) {
            p = p->next;
            ++n;
        }
        if (n == count) {

            node_base* extra = p->next;
            while (extra) {
                node_base* next = extra->next;
                free_node(as_node(extra));
                extra = next;
            }
            p->next = nullptr;
        } else {
            for (; n < count; ++n) {
                p = insert_node_after(p, make_node());
            }
        }
    }

    void resize(size_type count, const T& value) {
        node_base* p = &head_;
        size_type n = 0;
        while (p->next && n < count) {
            p = p->next;
            ++n;
        }
        if (n == count) {
            node_base* extra = p->next;
            while (extra) {
                node_base* next = extra->next;
                free_node(as_node(extra));
                extra = next;
            }
            p->next = nullptr;
        } else {
            for (; n < count; ++n) {
                p = insert_node_after(p, make_node(value));
            }
        }
    }

    void swap(forward_list& other) noexcept {
        mystl::swap(head_.next, other.head_.next);
    }

    void remove(const T& value) {
        node_base* p = &head_;
        while (p->next) {
            if (as_node(p->next)->value == value) {
                node_base* target = p->next;
                p->next = target->next;
                free_node(as_node(target));
            } else {
                p = p->next;
            }
        }
    }

    template<typename Pred>
    void remove_if(Pred pred) {
        node_base* p = &head_;
        while (p->next) {
            if (pred(as_node(p->next)->value)) {
                node_base* target = p->next;
                p->next = target->next;
                free_node(as_node(target));
            } else {
                p = p->next;
            }
        }
    }

    void unique() {
        node_base* p = head_.next;
        if (!p) {
            return;
        }
        while (p->next) {
            if (as_node(p->next)->value == as_node(p)->value) {
                node_base* target = p->next;
                p->next = target->next;
                free_node(as_node(target));
            } else {
                p = p->next;
            }
        }
    }

    template<typename BinaryPred>
    void unique(BinaryPred pred) {
        node_base* p = head_.next;
        if (!p) {
            return;
        }
        while (p->next) {
            if (pred(as_node(p)->value, as_node(p->next)->value)) {
                node_base* target = p->next;
                p->next = target->next;
                free_node(as_node(target));
            } else {
                p = p->next;
            }
        }
    }

    void reverse() noexcept {
        node_base* prev = nullptr;
        node_base* cur = head_.next;
        while (cur) {
            node_base* next = cur->next;
            cur->next = prev;
            prev = cur;
            cur = next;
        }
        head_.next = prev;
    }

    template<typename Compare>
    void merge(forward_list& other, Compare comp) {
        if (this == &other) {
            return;
        }
        node_base* p = &head_;
        node_base* a = head_.next;
        node_base* b = other.head_.next;
        while (a && b) {
            if (comp(as_node(b)->value, as_node(a)->value)) {
                p->next = b;
                b = b->next;
            } else {
                p->next = a;
                a = a->next;
            }
            p = p->next;
        }
        p->next = a ? a : b;
        other.head_.next = nullptr;
    }

    void merge(forward_list& other) {
        merge(other, less{});
    }

    template<typename Compare>
    void sort(Compare comp) {
        head_.next = merge_sort(head_.next, comp);
    }

    void sort() {
        sort(less{});
    }

    void splice_after(const_iterator pos, forward_list& other) {
        if (other.head_.next == nullptr) {
            return;
        }
        node_base* prev = mutable_after(pos);
        node_base* first = other.head_.next;
        node_base* last = first;
        while (last->next) {
            last = last->next;
        }
        last->next = prev->next;
        prev->next = first;
        other.head_.next = nullptr;
    }

private:
    struct less {
        bool operator()(const T& a, const T& b) const {
            return a < b;
        }
    };

    template<typename Compare>
    static node_base* merge_sorted(node_base* a, node_base* b, Compare comp) {
        node_base dummy{};
        node_base* p = &dummy;
        while (a && b) {
            if (comp(as_node(b)->value, as_node(a)->value)) {
                p->next = b;
                b = b->next;
            } else {
                p->next = a;
                a = a->next;
            }
            p = p->next;
        }
        p->next = a ? a : b;
        return dummy.next;
    }

    template<typename Compare>
    static node_base* merge_sort(node_base* head, Compare comp) {
        if (!head || !head->next) {
            return head;
        }

        node_base* slow = head;
        node_base* fast = head->next;
        while (fast && fast->next) {
            slow = slow->next;
            fast = fast->next->next;
        }
        node_base* second = slow->next;
        slow->next = nullptr;

        node_base* left = merge_sort(head, comp);
        node_base* right = merge_sort(second, comp);
        return merge_sorted(left, right, comp);
    }
};

template<typename T>
void swap(forward_list<T>& a, forward_list<T>& b) noexcept {
    a.swap(b);
}

template<typename T>
bool operator==(const forward_list<T>& a, const forward_list<T>& b) {
    auto ai = a.begin();
    auto bi = b.begin();
    auto ae = a.end();
    auto be = b.end();
    while (ai != ae && bi != be) {
        if (!(*ai == *bi)) {
            return false;
        }
        ++ai;
        ++bi;
    }
    return ai == ae && bi == be;
}

template<typename T>
bool operator!=(const forward_list<T>& a, const forward_list<T>& b) {
    return !(a == b);
}

template<typename T>
bool operator<(const forward_list<T>& a, const forward_list<T>& b) {
    auto ai = a.begin();
    auto bi = b.begin();
    auto ae = a.end();
    auto be = b.end();
    while (ai != ae && bi != be) {
        if (*ai < *bi) {
            return true;
        }
        if (*bi < *ai) {
            return false;
        }
        ++ai;
        ++bi;
    }
    return ai == ae && bi != be;
}

template<typename T>
bool operator<=(const forward_list<T>& a, const forward_list<T>& b) {
    return !(b < a);
}

template<typename T>
bool operator>(const forward_list<T>& a, const forward_list<T>& b) {
    return b < a;
}

template<typename T>
bool operator>=(const forward_list<T>& a, const forward_list<T>& b) {
    return !(a < b);
}

}
