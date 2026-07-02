#pragma once

#include "mystl/type_traits/type_traits.hpp"
#include "mystl/utility/utility.hpp"

#include <cstddef>

namespace mystl {

struct input_iterator_tag {};

struct output_iterator_tag {};

struct forward_iterator_tag : input_iterator_tag {};

struct bidirectional_iterator_tag : forward_iterator_tag {};

struct random_access_iterator_tag : bidirectional_iterator_tag {};

template<typename It>
struct iterator_traits {
    using iterator_category = typename It::iterator_category;
    using value_type = typename It::value_type;
    using difference_type = typename It::difference_type;
    using pointer = typename It::pointer;
    using reference = typename It::reference;
};

template<typename T>
struct iterator_traits<T*> {
    using iterator_category = random_access_iterator_tag;
    using value_type = remove_cv_t<T>;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using reference = T&;
};

template<typename T>
struct iterator_traits<const T*> {
    using iterator_category = random_access_iterator_tag;
    using value_type = remove_cv_t<T>;
    using difference_type = std::ptrdiff_t;
    using pointer = const T*;
    using reference = const T&;
};

template<typename It>
typename iterator_traits<It>::difference_type
distance_impl(It first, It last, input_iterator_tag) {
    typename iterator_traits<It>::difference_type n = 0;
    while (first != last) {
        ++first;
        ++n;
    }
    return n;
}

template<typename It>
typename iterator_traits<It>::difference_type
distance_impl(It first, It last, random_access_iterator_tag) {
    return last - first;
}

template<typename It>
typename iterator_traits<It>::difference_type
distance(It first, It last) {
    return distance_impl(first, last,
        typename iterator_traits<It>::iterator_category());
}

template<typename It, typename Distance>
void advance_impl(It& it, Distance n, input_iterator_tag) {
    while (n > 0) {
        --n;
        ++it;
    }
}

template<typename It, typename Distance>
void advance_impl(It& it, Distance n, bidirectional_iterator_tag) {
    while (n > 0) {
        --n;
        ++it;
    }
    while (n < 0) {
        ++n;
        --it;
    }
}

template<typename It, typename Distance>
void advance_impl(It& it, Distance n, random_access_iterator_tag) {
    it += n;
}

template<typename It, typename Distance>
void advance(It& it, Distance n) {
    advance_impl(it, typename iterator_traits<It>::difference_type(n),
        typename iterator_traits<It>::iterator_category());
}

template<typename It>
It next(It it, typename iterator_traits<It>::difference_type n = 1) {
    mystl::advance(it, n);
    return it;
}

template<typename It>
It prev(It it, typename iterator_traits<It>::difference_type n = 1) {
    mystl::advance(it, -n);
    return it;
}

template<typename It>
class reverse_iterator {
public:
    using iterator_type = It;
    using iterator_category = typename iterator_traits<It>::iterator_category;
    using value_type = typename iterator_traits<It>::value_type;
    using difference_type = typename iterator_traits<It>::difference_type;
    using pointer = typename iterator_traits<It>::pointer;
    using reference = typename iterator_traits<It>::reference;

    reverse_iterator() : current() {}

    explicit reverse_iterator(It x) : current(x) {}

    template<typename U>
    reverse_iterator(const reverse_iterator<U>& other) : current(other.base()) {}

    template<typename U>
    reverse_iterator& operator=(const reverse_iterator<U>& other) {
        current = other.base();
        return *this;
    }

    It base() const {
        return current;
    }

    reference operator*() const {
        It tmp = current;
        --tmp;
        return *tmp;
    }

    pointer operator->() const {
        It tmp = current;
        --tmp;
        return tmp.operator->();
    }

    reverse_iterator& operator++() {
        --current;
        return *this;
    }

    reverse_iterator operator++(int) {
        reverse_iterator tmp = *this;
        --current;
        return tmp;
    }

    reverse_iterator& operator--() {
        ++current;
        return *this;
    }

    reverse_iterator operator--(int) {
        reverse_iterator tmp = *this;
        ++current;
        return tmp;
    }

    reverse_iterator operator+(difference_type n) const {
        return reverse_iterator(current - n);
    }

    reverse_iterator& operator+=(difference_type n) {
        current -= n;
        return *this;
    }

    reverse_iterator operator-(difference_type n) const {
        return reverse_iterator(current + n);
    }

    reverse_iterator& operator-=(difference_type n) {
        current += n;
        return *this;
    }

    reference operator[](difference_type n) const {
        return *(*this + n);
    }

private:
    It current;
};

template<typename It1, typename It2>
bool operator==(const reverse_iterator<It1>& a, const reverse_iterator<It2>& b) {
    return a.base() == b.base();
}

template<typename It1, typename It2>
bool operator!=(const reverse_iterator<It1>& a, const reverse_iterator<It2>& b) {
    return a.base() != b.base();
}

template<typename It1, typename It2>
bool operator<(const reverse_iterator<It1>& a, const reverse_iterator<It2>& b) {
    return a.base() > b.base();
}

template<typename It1, typename It2>
bool operator<=(const reverse_iterator<It1>& a, const reverse_iterator<It2>& b) {
    return a.base() >= b.base();
}

template<typename It1, typename It2>
bool operator>(const reverse_iterator<It1>& a, const reverse_iterator<It2>& b) {
    return a.base() < b.base();
}

template<typename It1, typename It2>
bool operator>=(const reverse_iterator<It1>& a, const reverse_iterator<It2>& b) {
    return a.base() <= b.base();
}

template<typename It1, typename It2>
auto operator-(const reverse_iterator<It1>& a, const reverse_iterator<It2>& b)
    -> decltype(b.base() - a.base()) {
    return b.base() - a.base();
}

template<typename It>
reverse_iterator<It> operator+(
    typename reverse_iterator<It>::difference_type n,
    const reverse_iterator<It>& it) {
    return it + n;
}

template<typename It>
reverse_iterator<It> make_reverse_iterator(It it) {
    return reverse_iterator<It>(it);
}

template<typename Container>
class back_insert_iterator {
public:
    using iterator_category = output_iterator_tag;
    using value_type = void;
    using difference_type = std::ptrdiff_t;
    using pointer = void;
    using reference = void;
    using container_type = Container;

    explicit back_insert_iterator(Container& c) : container(&c) {}

    back_insert_iterator& operator=(const typename Container::value_type& value) {
        container->push_back(value);
        return *this;
    }

    back_insert_iterator& operator=(typename Container::value_type&& value) {
        container->push_back(mystl::move(value));
        return *this;
    }

    back_insert_iterator& operator*() {
        return *this;
    }

    back_insert_iterator& operator++() {
        return *this;
    }

    back_insert_iterator& operator++(int) {
        return *this;
    }

private:
    Container* container;
};

template<typename Container>
back_insert_iterator<Container> back_inserter(Container& c) {
    return back_insert_iterator<Container>(c);
}

template<typename C>
auto begin(C& c) -> decltype(c.begin()) {
    return c.begin();
}

template<typename C>
auto begin(const C& c) -> decltype(c.begin()) {
    return c.begin();
}

template<typename C>
auto end(C& c) -> decltype(c.end()) {
    return c.end();
}

template<typename C>
auto end(const C& c) -> decltype(c.end()) {
    return c.end();
}

template<typename T, std::size_t N>
T* begin(T (&array)[N]) {
    return array;
}

template<typename T, std::size_t N>
T* end(T (&array)[N]) {
    return array + N;
}

template<typename C>
auto cbegin(const C& c) -> decltype(mystl::begin(c)) {
    return mystl::begin(c);
}

template<typename C>
auto cend(const C& c) -> decltype(mystl::end(c)) {
    return mystl::end(c);
}

}
