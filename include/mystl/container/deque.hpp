#pragma once

#include <cstddef>
#include <new>
#include <stdexcept>
#include <initializer_list>

#include "mystl/memory/memory.hpp"
#include "mystl/utility/utility.hpp"
#include "mystl/iterator/iterator.hpp"

namespace mystl {

template<typename T>
struct deque_block_size {
    enum { value = sizeof(T) < 256 ? (4096 / sizeof(T)) : 16 };
};

template<typename T>
class deque {
private:
    static constexpr std::size_t BLOCK =
        deque_block_size<T>::value < 1 ? 1 : deque_block_size<T>::value;

    using map_pointer = T**;

    static T* allocate_block() {
        return static_cast<T*>(::operator new(BLOCK * sizeof(T)));
    }

    static void deallocate_block(T* p) noexcept {
        ::operator delete(p);
    }

    static map_pointer allocate_map(std::size_t n) {
        return static_cast<map_pointer>(::operator new(n * sizeof(T*)));
    }

    static void deallocate_map(map_pointer p) noexcept {
        ::operator delete(p);
    }

public:

    template<typename Ref, typename Ptr>
    class deque_iterator {
    public:
        using iterator_category = mystl::random_access_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = Ptr;
        using reference = Ref;

        using self = deque_iterator;

        T* cur = nullptr;
        T* first = nullptr;
        T* last = nullptr;
        map_pointer node = nullptr;

        deque_iterator() = default;
        deque_iterator(const deque_iterator&) = default;
        deque_iterator& operator=(const deque_iterator&) = default;

        deque_iterator(T* c, map_pointer n)
            : cur(c), first(n ? *n : nullptr),
              last(n ? *n + static_cast<difference_type>(BLOCK) : nullptr),
              node(n) {}

        template<typename R2, typename P2,
                 typename = enable_if_t<is_same_v<R2, T&> && is_same_v<P2, T*>>>
        deque_iterator(const deque_iterator<R2, P2>& other)
            : cur(other.cur), first(other.first),
              last(other.last), node(other.node) {}

        void set_node(map_pointer new_node) {
            node = new_node;
            first = *new_node;
            last = first + static_cast<difference_type>(BLOCK);
        }

        reference operator*() const {
            return *cur;
        }

        pointer operator->() const {
            return cur;
        }

        self& operator++() {
            ++cur;
            if (cur == last) {
                set_node(node + 1);
                cur = first;
            }
            return *this;
        }

        self operator++(int) {
            self tmp = *this;
            ++*this;
            return tmp;
        }

        self& operator--() {
            if (cur == first) {
                set_node(node - 1);
                cur = last;
            }
            --cur;
            return *this;
        }

        self operator--(int) {
            self tmp = *this;
            --*this;
            return tmp;
        }

        self& operator+=(difference_type n) {
            difference_type offset =
                n + (cur - first);
            if (offset >= 0 &&
                offset < static_cast<difference_type>(BLOCK)) {
                cur += n;
            } else {
                difference_type node_offset;
                if (offset > 0) {
                    node_offset =
                        offset / static_cast<difference_type>(BLOCK);
                } else {
                    node_offset =
                        -((-offset - 1) / static_cast<difference_type>(BLOCK))
                        - 1;
                }
                set_node(node + node_offset);
                cur = first +
                    (offset -
                     node_offset * static_cast<difference_type>(BLOCK));
            }
            return *this;
        }

        self operator+(difference_type n) const {
            self tmp = *this;
            return tmp += n;
        }

        self& operator-=(difference_type n) {
            return *this += -n;
        }

        self operator-(difference_type n) const {
            self tmp = *this;
            return tmp -= n;
        }

        reference operator[](difference_type n) const {
            return *(*this + n);
        }

        friend difference_type operator-(const self& a, const self& b) {
            return static_cast<difference_type>(BLOCK) * (a.node - b.node - 1)
                + (a.cur - a.first) + (b.last - b.cur);
        }

        friend bool operator==(const self& a, const self& b) {
            return a.cur == b.cur;
        }

        friend bool operator!=(const self& a, const self& b) {
            return a.cur != b.cur;
        }

        friend bool operator<(const self& a, const self& b) {
            return a.node == b.node ? a.cur < b.cur : a.node < b.node;
        }

        friend bool operator<=(const self& a, const self& b) {
            return !(b < a);
        }

        friend bool operator>(const self& a, const self& b) {
            return b < a;
        }

        friend bool operator>=(const self& a, const self& b) {
            return !(a < b);
        }
    };

    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = T&;
    using const_reference = const T&;
    using pointer = T*;
    using const_pointer = const T*;
    using iterator = deque_iterator<T&, T*>;
    using const_iterator = deque_iterator<const T&, const T*>;
    using reverse_iterator = mystl::reverse_iterator<iterator>;
    using const_reverse_iterator = mystl::reverse_iterator<const_iterator>;

private:
    map_pointer map_ = nullptr;
    size_type map_size_ = 0;
    iterator start_{};
    iterator finish_{};

    static size_type num_nodes_for(size_type num_elements) {
        return num_elements / BLOCK + 1;
    }

    void initialize_map(size_type num_elements) {
        size_type num_nodes = num_nodes_for(num_elements);

        map_size_ = (num_nodes + 2 < 8) ? 8 : num_nodes + 2;
        map_ = allocate_map(map_size_);

        map_pointer nstart = map_ + (map_size_ - num_nodes) / 2;
        map_pointer nfinish = nstart + num_nodes - 1;

        map_pointer cur = nstart;
        try {
            for (; cur <= nfinish; ++cur) {
                *cur = allocate_block();
            }
        } catch (...) {
            for (map_pointer p = nstart; p < cur; ++p) {
                deallocate_block(*p);
            }
            deallocate_map(map_);
            map_ = nullptr;
            map_size_ = 0;
            throw;
        }

        start_.set_node(nstart);
        start_.cur = start_.first;
        finish_.set_node(nfinish);
        finish_.cur = finish_.first + num_elements % BLOCK;
    }

    template<typename... Args>
    void fill_initialize(size_type num_elements, const Args&... args) {
        initialize_map(num_elements);
        map_pointer cur = start_.node;
        size_type constructed = 0;
        try {
            for (; cur < finish_.node; ++cur) {
                for (T* p = *cur; p < *cur + BLOCK; ++p) {
                    mystl::construct_at(p, args...);
                    ++constructed;
                }
            }
            for (T* p = finish_.first; p < finish_.cur; ++p) {
                mystl::construct_at(p, args...);
                ++constructed;
            }
        } catch (...) {
            destroy_range(start_, iterator(nullptr, nullptr), constructed);
            free_all_blocks();
            throw;
        }
    }

    void destroy_range(iterator from, iterator, size_type count) noexcept {
        iterator it = from;
        for (size_type i = 0; i < count; ++i) {
            mystl::destroy_at(it.cur);
            ++it;
        }
    }

    void destroy_elements() noexcept {
        for (iterator it = start_; it != finish_; ++it) {
            mystl::destroy_at(it.cur);
        }
    }

    void free_all_blocks() noexcept {
        if (!map_) {
            return;
        }
        for (map_pointer p = start_.node; p <= finish_.node; ++p) {
            if (*p) {
                deallocate_block(*p);
            }
        }
        deallocate_map(map_);
        map_ = nullptr;
        map_size_ = 0;
    }

    void reserve_map_at_back(size_type nodes_to_add = 1) {
        if (nodes_to_add + 1 >
            map_size_ - static_cast<size_type>(finish_.node - map_)) {
            reallocate_map(nodes_to_add, false);
        }
    }

    void reserve_map_at_front(size_type nodes_to_add = 1) {
        if (nodes_to_add >
            static_cast<size_type>(start_.node - map_)) {
            reallocate_map(nodes_to_add, true);
        }
    }

    void reallocate_map(size_type nodes_to_add, bool add_at_front) {
        size_type old_num_nodes =
            static_cast<size_type>(finish_.node - start_.node) + 1;
        size_type new_num_nodes = old_num_nodes + nodes_to_add;

        map_pointer new_nstart;
        if (map_size_ > 2 * new_num_nodes) {

            new_nstart = map_ + (map_size_ - new_num_nodes) / 2
                + (add_at_front ? nodes_to_add : 0);
            if (new_nstart < start_.node) {
                copy_nodes_forward(start_.node, finish_.node + 1, new_nstart);
            } else {
                copy_nodes_backward(start_.node, finish_.node + 1,
                                    new_nstart + old_num_nodes);
            }
        } else {
            size_type new_map_size =
                map_size_ + (map_size_ > nodes_to_add ? map_size_
                                                      : nodes_to_add) + 2;
            map_pointer new_map = allocate_map(new_map_size);
            new_nstart = new_map + (new_map_size - new_num_nodes) / 2
                + (add_at_front ? nodes_to_add : 0);
            copy_nodes_forward(start_.node, finish_.node + 1, new_nstart);
            deallocate_map(map_);
            map_ = new_map;
            map_size_ = new_map_size;
        }

        start_.set_node(new_nstart);
        finish_.set_node(new_nstart + old_num_nodes - 1);
    }

    static void copy_nodes_forward(map_pointer first, map_pointer last,
                                   map_pointer dest) noexcept {
        for (; first != last; ++first, ++dest) {
            *dest = *first;
        }
    }

    static void copy_nodes_backward(map_pointer first, map_pointer last,
                                    map_pointer dest_last) noexcept {
        while (first != last) {
            *--dest_last = *--last;
        }
    }

    void new_block_at_back() {
        reserve_map_at_back();
        *(finish_.node + 1) = allocate_block();
    }

    void new_block_at_front() {
        reserve_map_at_front();
        *(start_.node - 1) = allocate_block();
    }

    void copy_from(const deque& other) {
        initialize_map(other.size());
        const_iterator src = other.start_;
        const_iterator src_end = other.finish_;
        iterator dst = start_;
        size_type constructed = 0;
        try {
            for (; src != src_end; ++src, ++dst) {
                mystl::construct_at(dst.cur, *src);
                ++constructed;
            }
        } catch (...) {
            destroy_range(start_, iterator(nullptr, nullptr), constructed);
            free_all_blocks();
            throw;
        }
    }

public:

    deque() {
        initialize_map(0);
    }

    explicit deque(size_type n) {
        fill_initialize(n);
    }

    deque(size_type n, const T& value) {
        fill_initialize(n, value);
    }

    deque(std::initializer_list<T> init) {
        initialize_map(init.size());
        iterator dst = start_;
        size_type constructed = 0;
        try {
            for (const T& x : init) {
                mystl::construct_at(dst.cur, x);
                ++dst;
                ++constructed;
            }
        } catch (...) {
            destroy_range(start_, iterator(nullptr, nullptr), constructed);
            free_all_blocks();
            throw;
        }
    }

    deque(const deque& other) {
        copy_from(other);
    }

    deque(deque&& other) noexcept
        : map_(other.map_), map_size_(other.map_size_),
          start_(other.start_), finish_(other.finish_) {
        other.map_ = nullptr;
        other.map_size_ = 0;
        other.start_ = iterator();
        other.finish_ = iterator();
    }

    ~deque() {
        destroy_elements();
        free_all_blocks();
    }

    deque& operator=(const deque& other) {
        if (this != &other) {
            deque tmp(other);
            swap(tmp);
        }
        return *this;
    }

    deque& operator=(deque&& other) noexcept {
        if (this != &other) {
            destroy_elements();
            free_all_blocks();
            map_ = other.map_;
            map_size_ = other.map_size_;
            start_ = other.start_;
            finish_ = other.finish_;
            other.map_ = nullptr;
            other.map_size_ = 0;
            other.start_ = iterator();
            other.finish_ = iterator();
        }
        return *this;
    }

    deque& operator=(std::initializer_list<T> init) {
        deque tmp(init);
        swap(tmp);
        return *this;
    }

    iterator begin() {
        return start_;
    }

    const_iterator begin() const {
        return start_;
    }

    iterator end() {
        return finish_;
    }

    const_iterator end() const {
        return finish_;
    }

    const_iterator cbegin() const {
        return start_;
    }

    const_iterator cend() const {
        return finish_;
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

    size_type size() const {
        return static_cast<size_type>(finish_ - start_);
    }

    bool empty() const {
        return finish_ == start_;
    }

    reference operator[](size_type pos) {
        return start_[static_cast<difference_type>(pos)];
    }

    const_reference operator[](size_type pos) const {
        return start_[static_cast<difference_type>(pos)];
    }

    reference at(size_type pos) {
        if (pos >= size()) {
            throw std::out_of_range("mystl::deque::at");
        }
        return start_[static_cast<difference_type>(pos)];
    }

    const_reference at(size_type pos) const {
        if (pos >= size()) {
            throw std::out_of_range("mystl::deque::at");
        }
        return start_[static_cast<difference_type>(pos)];
    }

    reference front() {
        return *start_;
    }

    const_reference front() const {
        return *start_;
    }

    reference back() {
        iterator tmp = finish_;
        --tmp;
        return *tmp;
    }

    const_reference back() const {
        const_iterator tmp = finish_;
        --tmp;
        return *tmp;
    }

    template<typename... Args>
    reference emplace_back(Args&&... args) {
        if (finish_.cur != finish_.last - 1) {
            mystl::construct_at(finish_.cur, mystl::forward<Args>(args)...);
            ++finish_.cur;
        } else {
            new_block_at_back();
            mystl::construct_at(finish_.cur, mystl::forward<Args>(args)...);
            finish_.set_node(finish_.node + 1);
            finish_.cur = finish_.first;
        }
        iterator tmp = finish_;
        --tmp;
        return *tmp;
    }

    template<typename... Args>
    reference emplace_front(Args&&... args) {
        if (start_.cur != start_.first) {
            mystl::construct_at(start_.cur - 1, mystl::forward<Args>(args)...);
            --start_.cur;
        } else {
            new_block_at_front();
            start_.set_node(start_.node - 1);
            start_.cur = start_.last - 1;
            mystl::construct_at(start_.cur, mystl::forward<Args>(args)...);
        }
        return *start_;
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
        if (finish_.cur != finish_.first) {
            --finish_.cur;
            mystl::destroy_at(finish_.cur);
        } else {

            deallocate_block(finish_.first);
            finish_.set_node(finish_.node - 1);
            finish_.cur = finish_.last - 1;
            mystl::destroy_at(finish_.cur);
        }
    }

    void pop_front() {
        if (start_.cur != start_.last - 1) {
            mystl::destroy_at(start_.cur);
            ++start_.cur;
        } else {

            mystl::destroy_at(start_.cur);
            T* old_first = start_.first;
            start_.set_node(start_.node + 1);
            start_.cur = start_.first;
            deallocate_block(old_first);
        }
    }

    void clear() noexcept {
        destroy_elements();

        for (map_pointer node = start_.node + 1;
             node <= finish_.node; ++node) {
            deallocate_block(*node);
        }
        finish_ = start_;
    }

    iterator insert(const_iterator pos, const T& value) {
        return insert_impl(pos, value);
    }

    iterator insert(const_iterator pos, T&& value) {
        return insert_impl(pos, mystl::move(value));
    }

    iterator erase(const_iterator pos) {
        difference_type index = pos - cbegin();
        iterator result = start_ + index;
        difference_type n = static_cast<difference_type>(size());

        if (index < n - index - 1) {

            for (iterator it = result; it != start_; --it) {
                iterator prev = it;
                --prev;
                *it = mystl::move(*prev);
            }
            pop_front();
            return start_ + index;
        } else {

            for (iterator it = result; ; ++it) {
                iterator next = it;
                ++next;
                if (next == finish_) {
                    break;
                }
                *it = mystl::move(*next);
            }
            pop_back();
            return start_ + index;
        }
    }

    iterator erase(const_iterator first, const_iterator last) {
        difference_type index = first - cbegin();
        difference_type count = last - first;
        if (count <= 0) {
            return start_ + index;
        }

        iterator first_it = start_ + index;
        iterator last_it = first_it + count;
        difference_type elems_before = index;
        difference_type elems_after =
            static_cast<difference_type>(size()) - index - count;

        if (elems_before < elems_after) {

            iterator src = first_it;
            iterator dst = last_it;
            while (src != start_) {
                --src;
                --dst;
                *dst = mystl::move(*src);
            }
            iterator new_start = start_ + count;
            for (iterator it = start_; it != new_start; ++it) {
                mystl::destroy_at(it.cur);
            }
            free_front_blocks(new_start);
            start_ = new_start;
            return start_ + index;
        } else {

            iterator src = last_it;
            iterator dst = first_it;
            while (src != finish_) {
                *dst = mystl::move(*src);
                ++dst;
                ++src;
            }
            iterator new_finish = finish_ - count;
            for (iterator it = new_finish; it != finish_; ++it) {
                mystl::destroy_at(it.cur);
            }
            free_back_blocks(new_finish);
            finish_ = new_finish;
            return start_ + index;
        }
    }

    void resize(size_type n) {
        size_type len = size();
        if (n < len) {
            for (size_type i = 0; i < len - n; ++i) {
                pop_back();
            }
        } else {
            for (size_type i = 0; i < n - len; ++i) {
                emplace_back();
            }
        }
    }

    void resize(size_type n, const T& value) {
        size_type len = size();
        if (n < len) {
            for (size_type i = 0; i < len - n; ++i) {
                pop_back();
            }
        } else {
            for (size_type i = 0; i < n - len; ++i) {
                push_back(value);
            }
        }
    }

    void swap(deque& other) noexcept {
        mystl::swap(map_, other.map_);
        mystl::swap(map_size_, other.map_size_);
        mystl::swap(start_, other.start_);
        mystl::swap(finish_, other.finish_);
    }

private:

    void free_front_blocks(iterator new_start) noexcept {
        for (map_pointer node = start_.node; node < new_start.node; ++node) {
            deallocate_block(*node);
        }
    }

    void free_back_blocks(iterator new_finish) noexcept {
        for (map_pointer node = finish_.node; node > new_finish.node; --node) {
            deallocate_block(*node);
        }
    }

    template<typename U>
    iterator insert_impl(const_iterator pos, U&& value) {
        difference_type index = pos - cbegin();
        difference_type n = static_cast<difference_type>(size());

        if (index == 0) {
            emplace_front(mystl::forward<U>(value));
            return start_;
        }
        if (index == n) {
            emplace_back(mystl::forward<U>(value));
            iterator tmp = finish_;
            --tmp;
            return tmp;
        }

        if (index < n - index) {

            emplace_front(mystl::move(*start_));
            iterator front1 = start_;
            ++front1;
            iterator front2 = front1;
            ++front2;
            iterator target = start_ + (index + 1);
            for (; front2 != target; ++front1, ++front2) {
                *front1 = mystl::move(*front2);
            }
            *front1 = mystl::forward<U>(value);
            return start_ + index;
        } else {

            iterator last = finish_;
            --last;
            emplace_back(mystl::move(*last));
            iterator back1 = finish_;
            --back1;
            --back1;
            iterator back2 = back1;
            --back2;
            iterator target = start_ + index;
            for (; back1 != target; --back1, --back2) {
                *back1 = mystl::move(*back2);
            }
            *back1 = mystl::forward<U>(value);
            return start_ + index;
        }
    }
};

template<typename T>
bool operator==(const deque<T>& a, const deque<T>& b) {
    if (a.size() != b.size()) {
        return false;
    }
    auto ai = a.begin();
    auto bi = b.begin();
    for (; ai != a.end(); ++ai, ++bi) {
        if (!(*ai == *bi)) {
            return false;
        }
    }
    return true;
}

template<typename T>
bool operator!=(const deque<T>& a, const deque<T>& b) {
    return !(a == b);
}

template<typename T>
bool operator<(const deque<T>& a, const deque<T>& b) {
    auto ai = a.begin();
    auto bi = b.begin();
    for (; ai != a.end() && bi != b.end(); ++ai, ++bi) {
        if (*ai < *bi) {
            return true;
        }
        if (*bi < *ai) {
            return false;
        }
    }
    return ai == a.end() && bi != b.end();
}

template<typename T>
bool operator<=(const deque<T>& a, const deque<T>& b) {
    return !(b < a);
}

template<typename T>
bool operator>(const deque<T>& a, const deque<T>& b) {
    return b < a;
}

template<typename T>
bool operator>=(const deque<T>& a, const deque<T>& b) {
    return !(a < b);
}

template<typename T>
void swap(deque<T>& a, deque<T>& b) noexcept {
    a.swap(b);
}

}
