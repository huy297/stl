#pragma once

#include <cstddef>

#include "mystl/container/deque.hpp"
#include "mystl/utility/utility.hpp"

namespace mystl {

template<class T, class Container = mystl::deque<T>>
class stack {
public:
    using value_type = typename Container::value_type;
    using reference = typename Container::reference;
    using const_reference = typename Container::const_reference;
    using size_type = typename Container::size_type;
    using container_type = Container;

protected:
    Container c;

public:
    stack() = default;

    explicit stack(const Container& cont) : c(cont) {}

    explicit stack(Container&& cont) : c(mystl::move(cont)) {}

    stack(const stack& other) : c(other.c) {}

    stack(stack&& other) : c(mystl::move(other.c)) {}

    stack& operator=(const stack& other) {
        c = other.c;
        return *this;
    }

    stack& operator=(stack&& other) {
        c = mystl::move(other.c);
        return *this;
    }

    bool empty() const {
        return c.empty();
    }

    size_type size() const {
        return c.size();
    }

    reference top() {
        return c.back();
    }

    const_reference top() const {
        return c.back();
    }

    void push(const value_type& value) {
        c.push_back(value);
    }

    void push(value_type&& value) {
        c.push_back(mystl::move(value));
    }

    template<class... Args>
    reference emplace(Args&&... args) {
        return c.emplace_back(mystl::forward<Args>(args)...);
    }

    void pop() {
        c.pop_back();
    }

    void swap(stack& other) {
        mystl::swap(c, other.c);
    }

    template<class U, class C>
    friend bool operator==(const stack<U, C>& a, const stack<U, C>& b);
    template<class U, class C>
    friend bool operator!=(const stack<U, C>& a, const stack<U, C>& b);
    template<class U, class C>
    friend bool operator<(const stack<U, C>& a, const stack<U, C>& b);
    template<class U, class C>
    friend bool operator<=(const stack<U, C>& a, const stack<U, C>& b);
    template<class U, class C>
    friend bool operator>(const stack<U, C>& a, const stack<U, C>& b);
    template<class U, class C>
    friend bool operator>=(const stack<U, C>& a, const stack<U, C>& b);
};

template<class T, class Container>
bool operator==(const stack<T, Container>& a, const stack<T, Container>& b) {
    return a.c == b.c;
}

template<class T, class Container>
bool operator!=(const stack<T, Container>& a, const stack<T, Container>& b) {
    return a.c != b.c;
}

template<class T, class Container>
bool operator<(const stack<T, Container>& a, const stack<T, Container>& b) {
    return a.c < b.c;
}

template<class T, class Container>
bool operator<=(const stack<T, Container>& a, const stack<T, Container>& b) {
    return a.c <= b.c;
}

template<class T, class Container>
bool operator>(const stack<T, Container>& a, const stack<T, Container>& b) {
    return a.c > b.c;
}

template<class T, class Container>
bool operator>=(const stack<T, Container>& a, const stack<T, Container>& b) {
    return a.c >= b.c;
}

}
