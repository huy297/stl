#include <iostream>
#include <cassert>
#include "mystl/array.hpp"

int main() {
    mystl::array<int, 5> a;

    // operator[] fill
    for (mystl::array<int, 5>::size_type i = 0; i < a.size(); ++i) {
        a[i] = static_cast<int>(i) * 10;  // 0 10 20 30 40
    }

    assert(a.size() == 5);
    assert(a[0] == 0);
    assert(a[4] == 40);

    // at() / front() / back()
    assert(a.at(2) == 20);
    assert(a.front() == 0);
    assert(a.back() == 40);

    bool threw = false;
    try {
        a.at(5);
    } catch (const std::out_of_range&) {
        threw = true;
    }
    assert(threw);

    // range-for sum
    int sum = 0;
    for (int v : a) {
        sum += v;
    }
    assert(sum == 0 + 10 + 20 + 30 + 40);
    std::cout << "sum = " << sum << '\n';

    // rbegin / rend -> reverse order
    int reversed[5];
    int idx = 0;
    for (auto it = a.rbegin(); it != a.rend(); ++it) {
        reversed[idx++] = *it;
    }
    assert(idx == 5);
    assert(reversed[0] == 40);
    assert(reversed[1] == 30);
    assert(reversed[2] == 20);
    assert(reversed[3] == 10);
    assert(reversed[4] == 0);
    std::cout << "reversed front = " << reversed[0] << '\n';

    // comparison == and <
    mystl::array<int, 5> b = a;  // copy
    assert(b == a);
    assert(!(b < a));
    assert(!(a < b));

    b[4] = 41;  // make b greater at last position
    assert(!(b == a));
    assert(a < b);
    assert(!(b < a));

    // mystl::get<0>
    assert(mystl::get<0>(a) == 0);
    const mystl::array<int, 5>& ca = a;
    assert(mystl::get<0>(ca) == 0);

    mystl::get<0>(b) = 99;
    assert(b.front() == 99);
    std::cout << "get<0>(b) = " << mystl::get<0>(b) << '\n';

    std::cout << "array ok\n";
    return 0;
}
