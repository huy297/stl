#include <iostream>
#include <cassert>

#include "mystl/container/vector.hpp"

int main() {

    mystl::vector<int> v;
    assert(v.empty());
    assert(v.size() == 0);

    for (int i = 0; i < 5; ++i) {
        v.push_back(i * 10);
    }
    v.emplace_back(50);
    assert(v.size() == 6);
    std::cout << "after push/emplace size=" << v.size() << '\n';

    assert(v[0] == 0);
    assert(v[5] == 50);
    assert(v.at(2) == 20);
    assert(v.front() == 0);
    assert(v.back() == 50);

    bool threw = false;
    try {
        (void)v.at(99);
    } catch (const std::out_of_range&) {
        threw = true;
    }
    assert(threw);
    std::cout << "at out_of_range threw=" << threw << '\n';

    v.reserve(64);
    assert(v.capacity() >= 64);
    assert(v.size() == 6);
    std::cout << "after reserve cap>=64 cap=" << v.capacity()
              << " size=" << v.size() << '\n';

    v.resize(8);
    assert(v.size() == 8);
    assert(v[6] == 0 && v[7] == 0);
    v.resize(4);
    assert(v.size() == 4);
    assert(v.back() == 30);
    std::cout << "after resize size=" << v.size() << " back=" << v.back() << '\n';

    v.insert(v.begin(), -1);
    assert(v.front() == -1);
    assert(v.size() == 5);

    v.insert(v.begin() + 2, 5);
    assert(v[2] == 5);
    assert(v.size() == 6);

    v.insert(v.end(), 99);
    assert(v.back() == 99);
    assert(v.size() == 7);
    std::cout << "after inserts size=" << v.size()
              << " front=" << v.front() << " back=" << v.back() << '\n';

    {
        int expected[] = {-1, 0, 5, 10, 20, 30, 99};
        for (std::size_t i = 0; i < v.size(); ++i) {
            assert(v[i] == expected[i]);
        }
    }

    v.erase(v.begin());
    assert(v.front() == 0);
    assert(v.size() == 6);

    v.erase(v.begin() + 1, v.begin() + 4);
    assert(v.size() == 3);
    {
        int expected[] = {0, 30, 99};
        for (std::size_t i = 0; i < v.size(); ++i) {
            assert(v[i] == expected[i]);
        }
    }
    std::cout << "after erase size=" << v.size()
              << " contents=";
    for (std::size_t i = 0; i < v.size(); ++i) {
        std::cout << v[i] << (i + 1 < v.size() ? ' ' : '\n');
    }

    v.assign(4, 7);
    assert(v.size() == 4);
    for (std::size_t i = 0; i < v.size(); ++i) {
        assert(v[i] == 7);
    }
    std::cout << "after assign size=" << v.size() << " value=" << v[0] << '\n';

    mystl::vector<int> a{1, 2, 3};
    mystl::vector<int> b{1, 2, 3};
    mystl::vector<int> c{1, 2, 4};
    mystl::vector<int> d{1, 2};

    assert(a == b);
    assert(!(a == c));
    assert(a < c);
    assert(d < a);
    assert(!(c < a));
    std::cout << "compare (a==b)=" << (a == b)
              << " (a<c)=" << (a < c)
              << " (d<a)=" << (d < a) << '\n';

    mystl::vector<int> seq{10, 20, 30, 40};
    int collected[4];
    int k = 0;
    for (auto it = seq.rbegin(); it != seq.rend(); ++it) {
        collected[k++] = *it;
    }
    assert(k == 4);
    assert(collected[0] == 40);
    assert(collected[1] == 30);
    assert(collected[2] == 20);
    assert(collected[3] == 10);
    std::cout << "reverse iter=";
    for (int i = 0; i < 4; ++i) {
        std::cout << collected[i] << (i + 1 < 4 ? ' ' : '\n');
    }

    std::cout << "vector ok" << '\n';
    return 0;
}
