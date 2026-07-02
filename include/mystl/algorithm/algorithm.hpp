#pragma once

#include "mystl/utility/utility.hpp"
#include "mystl/utility/functional.hpp"

namespace mystl {

template <typename ForwardIt, typename T>
void fill(ForwardIt first, ForwardIt last, const T& value) {
    while (first != last) {
        *first = value;
        ++first;
    }
}

template <typename InputIt, typename OutputIt>
OutputIt copy(InputIt first, InputIt last, OutputIt dest) {
    while (first != last) {
        *dest = *first;
        first++;
        dest++;
    }

    return dest;
}

template <typename InputIt, typename OutputIt>
OutputIt move(InputIt first, InputIt last, OutputIt dest) {
    while (first != last) {
        *dest = mystl::move(*first);

        first++;
        dest++;
    }

    return dest;
}

template<typename InputIt1, typename InputIt2>
bool equal(InputIt1 first1, InputIt1 last1, InputIt2 first2) {
    while (first1 != last1) {
        if (!(*first1 == *first2)) {
            return false;
        }

        ++first1;
        ++first2;
    }

    return true;
}

template<typename InputIt1, typename InputIt2>
bool lexicographical_compare(
    InputIt1 first1,
    InputIt1 last1,
    InputIt2 first2,
    InputIt2 last2
) {
    while (first1 != last1 && first2 != last2) {
        if (*first1 < *first2) {
            return true;
        }

        if (*first2 < *first1) {
            return false;
        }

        ++first1;
        ++first2;
    }

    return first1 == last1 && first2 != last2;
}

template <typename ForwardIt1, typename ForwardIt2>
ForwardIt2 swap_ranges(ForwardIt1 first1, ForwardIt1 last1, ForwardIt2 first2) {
    while (first1 != last1) {
        mystl::swap(*first1, *first2);

        ++first1;
        ++first2;
    }

    return first2;
}

template <typename T>
const T& min(const T& a, const T& b) {
    return (b < a) ? b : a;
}

template <typename T, typename Compare>
const T& min(const T& a, const T& b, Compare comp) {
    return comp(b, a) ? b : a;
}

template <typename T>
const T& max(const T& a, const T& b) {
    return (a < b) ? b : a;
}

template <typename T, typename Compare>
const T& max(const T& a, const T& b, Compare comp) {
    return comp(a, b) ? b : a;
}

template <typename ForwardIt>
ForwardIt min_element(ForwardIt first, ForwardIt last) {
    if (first == last) {
        return last;
    }

    ForwardIt smallest = first;
    ++first;
    while (first != last) {
        if (*first < *smallest) {
            smallest = first;
        }
        ++first;
    }

    return smallest;
}

template <typename ForwardIt, typename Compare>
ForwardIt min_element(ForwardIt first, ForwardIt last, Compare comp) {
    if (first == last) {
        return last;
    }

    ForwardIt smallest = first;
    ++first;
    while (first != last) {
        if (comp(*first, *smallest)) {
            smallest = first;
        }
        ++first;
    }

    return smallest;
}

template <typename ForwardIt>
ForwardIt max_element(ForwardIt first, ForwardIt last) {
    if (first == last) {
        return last;
    }

    ForwardIt largest = first;
    ++first;
    while (first != last) {
        if (*largest < *first) {
            largest = first;
        }
        ++first;
    }

    return largest;
}

template <typename ForwardIt, typename Compare>
ForwardIt max_element(ForwardIt first, ForwardIt last, Compare comp) {
    if (first == last) {
        return last;
    }

    ForwardIt largest = first;
    ++first;
    while (first != last) {
        if (comp(*largest, *first)) {
            largest = first;
        }
        ++first;
    }

    return largest;
}

template <typename InputIt, typename T>
InputIt find(InputIt first, InputIt last, const T& value) {
    while (first != last) {
        if (*first == value) {
            return first;
        }
        ++first;
    }

    return last;
}

template <typename InputIt, typename UnaryPredicate>
InputIt find_if(InputIt first, InputIt last, UnaryPredicate pred) {
    while (first != last) {
        if (pred(*first)) {
            return first;
        }
        ++first;
    }

    return last;
}

template <typename InputIt, typename UnaryPredicate>
InputIt find_if_not(InputIt first, InputIt last, UnaryPredicate pred) {
    while (first != last) {
        if (!pred(*first)) {
            return first;
        }
        ++first;
    }

    return last;
}

template <typename InputIt, typename T>
auto count(InputIt first, InputIt last, const T& value)
    -> mystl::remove_reference_t<decltype(last - first)> {
    mystl::remove_reference_t<decltype(last - first)> result = 0;
    while (first != last) {
        if (*first == value) {
            ++result;
        }
        ++first;
    }

    return result;
}

template <typename InputIt, typename UnaryPredicate>
auto count_if(InputIt first, InputIt last, UnaryPredicate pred)
    -> mystl::remove_reference_t<decltype(last - first)> {
    mystl::remove_reference_t<decltype(last - first)> result = 0;
    while (first != last) {
        if (pred(*first)) {
            ++result;
        }
        ++first;
    }

    return result;
}

template <typename InputIt, typename UnaryPredicate>
bool all_of(InputIt first, InputIt last, UnaryPredicate pred) {
    while (first != last) {
        if (!pred(*first)) {
            return false;
        }
        ++first;
    }

    return true;
}

template <typename InputIt, typename UnaryPredicate>
bool any_of(InputIt first, InputIt last, UnaryPredicate pred) {
    while (first != last) {
        if (pred(*first)) {
            return true;
        }
        ++first;
    }

    return false;
}

template <typename InputIt, typename UnaryPredicate>
bool none_of(InputIt first, InputIt last, UnaryPredicate pred) {
    while (first != last) {
        if (pred(*first)) {
            return false;
        }
        ++first;
    }

    return true;
}

template <typename InputIt, typename UnaryFunction>
UnaryFunction for_each(InputIt first, InputIt last, UnaryFunction f) {
    while (first != last) {
        f(*first);
        ++first;
    }

    return f;
}

template <typename OutputIt, typename Size, typename T>
OutputIt fill_n(OutputIt first, Size n, const T& value) {
    while (n > 0) {
        *first = value;
        ++first;
        --n;
    }

    return first;
}

template <typename InputIt, typename Size, typename OutputIt>
OutputIt copy_n(InputIt first, Size n, OutputIt dest) {
    while (n > 0) {
        *dest = *first;
        ++first;
        ++dest;
        --n;
    }

    return dest;
}

template <typename ForwardIt1, typename ForwardIt2>
void iter_swap(ForwardIt1 a, ForwardIt2 b) {
    mystl::swap(*a, *b);
}

template <typename BidirIt>
void reverse(BidirIt first, BidirIt last) {
    while (first != last) {
        --last;
        if (first == last) {
            break;
        }
        mystl::iter_swap(first, last);
        ++first;
    }
}

template <typename ForwardIt, typename T>
ForwardIt remove(ForwardIt first, ForwardIt last, const T& value) {
    first = mystl::find(first, last, value);
    if (first == last) {
        return first;
    }

    ForwardIt result = first;
    ++first;
    while (first != last) {
        if (!(*first == value)) {
            *result = mystl::move(*first);
            ++result;
        }
        ++first;
    }

    return result;
}

template <typename ForwardIt, typename UnaryPredicate>
ForwardIt remove_if(ForwardIt first, ForwardIt last, UnaryPredicate pred) {
    first = mystl::find_if(first, last, pred);
    if (first == last) {
        return first;
    }

    ForwardIt result = first;
    ++first;
    while (first != last) {
        if (!pred(*first)) {
            *result = mystl::move(*first);
            ++result;
        }
        ++first;
    }

    return result;
}

template <typename RandomIt, typename Compare>
void sort(RandomIt first, RandomIt last, Compare comp) {
    for (RandomIt i = first; i != last; ++i) {
        RandomIt j = i;
        while (j != first) {
            RandomIt prev = j;
            --prev;
            if (comp(*j, *prev)) {
                mystl::swap(*j, *prev);
                j = prev;
            } else {
                break;
            }
        }
    }
}

template <typename RandomIt>
void sort(RandomIt first, RandomIt last) {
    using value_type = mystl::remove_reference_t<decltype(*first)>;
    mystl::sort(first, last, mystl::less<value_type>());
}

}
