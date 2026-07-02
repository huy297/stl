#pragma once

#include <cstddef>

#include "mystl/container/vector.hpp"
#include "mystl/utility/functional.hpp"
#include "mystl/utility/utility.hpp"

namespace mystl {

template<class T, class Container = mystl::vector<T>, class Compare = mystl::less<T>>
class priority_queue {
public:
    using value_type = typename Container::value_type;
    using reference = typename Container::reference;
    using const_reference = typename Container::const_reference;
    using size_type = typename Container::size_type;
    using container_type = Container;
    using value_compare = Compare;

protected:
    Container c;
    Compare comp;

private:

    void sift_up(size_type i) {
        while (i > 0) {
            size_type parent = (i - 1) / 2;
            if (comp(c[parent], c[i])) {
                mystl::swap(c[parent], c[i]);
                i = parent;
            } else {
                break;
            }
        }
    }

    void sift_down(size_type i, size_type count) {
        while (true) {
            size_type left = 2 * i + 1;
            size_type right = 2 * i + 2;
            size_type largest = i;

            if (left < count && comp(c[largest], c[left])) {
                largest = left;
            }
            if (right < count && comp(c[largest], c[right])) {
                largest = right;
            }
            if (largest == i) {
                break;
            }
            mystl::swap(c[i], c[largest]);
            i = largest;
        }
    }

    void build_heap() {
        size_type n = c.size();
        if (n < 2) {
            return;
        }

        size_type i = n / 2;
        while (i > 0) {
            --i;
            sift_down(i, n);
        }
    }

public:
    priority_queue() = default;

    explicit priority_queue(const Compare& compare) : c(), comp(compare) {}

    priority_queue(const Compare& compare, const Container& cont)
        : c(cont), comp(compare) {
        build_heap();
    }

    priority_queue(const Compare& compare, Container&& cont)
        : c(mystl::move(cont)), comp(compare) {
        build_heap();
    }

    priority_queue(const priority_queue& other)
        : c(other.c), comp(other.comp) {}

    priority_queue(priority_queue&& other)
        : c(mystl::move(other.c)), comp(mystl::move(other.comp)) {}

    priority_queue& operator=(const priority_queue& other) {
        c = other.c;
        comp = other.comp;
        return *this;
    }

    priority_queue& operator=(priority_queue&& other) {
        c = mystl::move(other.c);
        comp = mystl::move(other.comp);
        return *this;
    }

    bool empty() const {
        return c.empty();
    }

    size_type size() const {
        return c.size();
    }

    const_reference top() const {
        return c.front();
    }

    void push(const value_type& value) {
        c.push_back(value);
        sift_up(c.size() - 1);
    }

    void push(value_type&& value) {
        c.push_back(mystl::move(value));
        sift_up(c.size() - 1);
    }

    template<class... Args>
    void emplace(Args&&... args) {
        c.emplace_back(mystl::forward<Args>(args)...);
        sift_up(c.size() - 1);
    }

    void pop() {
        size_type n = c.size();
        if (n == 0) {
            return;
        }
        if (n > 1) {
            mystl::swap(c[0], c[n - 1]);
        }
        c.pop_back();
        if (c.size() > 1) {
            sift_down(0, c.size());
        }
    }

    void swap(priority_queue& other) {
        mystl::swap(c, other.c);
        mystl::swap(comp, other.comp);
    }
};

}
