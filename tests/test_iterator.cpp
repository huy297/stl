#include <iostream>
#include <cassert>

#include "mystl/iterator.hpp"
#include "mystl/array.hpp"
#include "mystl/vector.hpp"

int main() {
    // --- reverse_iterator over a raw int[5] ---
    int raw[5] = {1, 2, 3, 4, 5};

    mystl::reverse_iterator<int*> rfirst(raw + 5);
    mystl::reverse_iterator<int*> rlast(raw);

    // Iterate forward over the reverse range: should see 5, 4, 3, 2, 1.
    int expected_rev[5] = {5, 4, 3, 2, 1};
    int idx = 0;
    for (mystl::reverse_iterator<int*> it = rfirst; it != rlast; ++it) {
        assert(*it == expected_rev[idx]);
        ++idx;
    }
    assert(idx == 5);

    // deref first element of reverse range, and base() points one past it.
    assert(*rfirst == 5);
    assert(rfirst.base() == raw + 5);
    assert(rlast.base() == raw);

    // ++/-- and base() behavior.
    mystl::reverse_iterator<int*> rit = rfirst;
    assert(*rit == 5);
    ++rit;
    assert(*rit == 4);
    assert(rit.base() == raw + 4);
    --rit;
    assert(*rit == 5);
    assert(rit.base() == raw + 5);

    // distance over the whole reverse range.
    assert(mystl::distance(rfirst, rlast) == 5);

    std::cout << "raw reverse:";
    for (mystl::reverse_iterator<int*> it = rfirst; it != rlast; ++it) {
        std::cout << ' ' << *it;
    }
    std::cout << '\n';

    // --- reverse_iterator over a mystl::array ---
    mystl::array<int, 5> arr;
    for (int i = 0; i < 5; ++i) {
        arr[i] = (i + 1) * 10;  // 10, 20, 30, 40, 50
    }

    int expected_arr_rev[5] = {50, 40, 30, 20, 10};
    idx = 0;
    for (auto it = arr.rbegin(); it != arr.rend(); ++it) {
        assert(*it == expected_arr_rev[idx]);
        ++idx;
    }
    assert(idx == 5);

    // base() of rbegin() is end(); base() of rend() is begin().
    assert(arr.rbegin().base() == arr.end());
    assert(arr.rend().base() == arr.begin());

    // -- on an array reverse_iterator walks back toward rbegin.
    auto ait = arr.rbegin();
    assert(*ait == 50);
    ++ait;
    ++ait;
    assert(*ait == 30);
    --ait;
    assert(*ait == 40);

    std::cout << "array reverse:";
    for (auto it = arr.rbegin(); it != arr.rend(); ++it) {
        std::cout << ' ' << *it;
    }
    std::cout << '\n';

    // --- distance & advance on pointers ---
    int* p_begin = raw;
    int* p_end = raw + 5;
    assert(mystl::distance(p_begin, p_end) == 5);

    int* p = raw;
    mystl::advance(p, 2);
    assert(*p == 3);
    assert(p == raw + 2);
    mystl::advance(p, -1);
    assert(*p == 2);
    assert(p == raw + 1);

    // --- next / prev ---
    int* n = mystl::next(raw);          // default step of 1
    assert(*n == 2);
    int* n3 = mystl::next(raw, 3);
    assert(*n3 == 4);
    int* pv = mystl::prev(raw + 4);     // default step of 1
    assert(*pv == 4);
    int* pv2 = mystl::prev(raw + 4, 2);
    assert(*pv2 == 3);
    // next/prev must not mutate the source iterator.
    assert(raw[0] == 1);

    // --- back_inserter into a mystl::vector<int> ---
    mystl::vector<int> vec;
    auto out = mystl::back_inserter(vec);
    for (int i = 0; i < 5; ++i) {
        *out = raw[i];   // copies 1..5 into the vector
        ++out;
    }
    assert(vec.size() == 5);
    for (int i = 0; i < 5; ++i) {
        assert(vec[i] == raw[i]);
    }

    // back_inserter accepting an rvalue (uses the move overload).
    *mystl::back_inserter(vec) = 6;
    assert(vec.size() == 6);
    assert(vec[5] == 6);

    std::cout << "vector via back_inserter:";
    for (mystl::vector<int>::size_type i = 0; i < vec.size(); ++i) {
        std::cout << ' ' << vec[i];
    }
    std::cout << '\n';

    std::cout << "iterator ok\n";
    return 0;
}
