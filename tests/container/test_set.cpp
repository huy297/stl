#include <iostream>
#include <cassert>
#include <string>
#include <set>

#include "mystl/container/set.hpp"

int main() {

    mystl::set<int> s;
    assert(s.empty());
    assert(s.size() == 0);

    const int keys[] = {50, 10, 90, 30, 70, 20, 80, 40, 60, 0,
                        5, 95, 35, 65, 15, 85, 25, 75, 45, 55};
    const std::size_t N = sizeof(keys) / sizeof(keys[0]);

    std::set<int> oracle;
    for (std::size_t i = 0; i < N; ++i) {
        auto r = s.insert(keys[i]);
        assert(r.second);
        assert(*r.first == keys[i]);
        oracle.insert(keys[i]);
    }
    assert(s.size() == N);

    {
        auto r = s.insert(50);
        assert(!r.second);
        assert(*r.first == 50);
        assert(s.size() == N);
    }
    std::cout << "after insert size=" << s.size() << '\n';

    {
        auto it = s.begin();
        auto oit = oracle.begin();
        int prev = -1;
        bool first = true;
        for (; it != s.end(); ++it, ++oit) {
            if (!first) {
                assert(prev < *it);
            }
            first = false;
            prev = *it;
            assert(*it == *oit);
        }
        assert(oit == oracle.end());
    }
    std::cout << "ordered iteration ok\n";

    {
        auto it = s.find(70);
        assert(it != s.end());
        assert(*it == 70);
        assert(s.find(999) == s.end());

        assert(s.count(70) == 1);
        assert(s.count(999) == 0);
        assert(s.contains(70));
        assert(!s.contains(999));
    }
    std::cout << "find/count/contains ok\n";

    {

        auto lb = s.lower_bound(30);
        assert(lb != s.end());
        assert(*lb == 30);

        auto ub = s.upper_bound(30);
        assert(ub != s.end());
        assert(*ub == 35);

        auto er = s.equal_range(30);
        assert(er.first == lb);
        assert(er.second == ub);

        auto lb2 = s.lower_bound(31);
        auto ub2 = s.upper_bound(31);
        assert(lb2 == ub2);
        assert(*lb2 == 35);

        assert(s.lower_bound(100000) == s.end());
        assert(s.upper_bound(100000) == s.end());
    }
    std::cout << "lower/upper ok\n";

    {
        std::size_t before = s.size();
        assert(s.erase(90) == 1);
        assert(s.size() == before - 1);
        assert(!s.contains(90));
        assert(s.erase(90) == 0);

        auto it = s.find(10);
        assert(it != s.end());
        auto next = s.erase(it);
        assert(!s.contains(10));
        assert(next != s.end());
        assert(*next == 15);

        oracle.erase(90);
        oracle.erase(10);
    }
    std::cout << "erase ok\n";

    {
        assert(s.size() == oracle.size());
        auto it = s.begin();
        auto oit = oracle.begin();
        for (; it != s.end(); ++it, ++oit) {
            assert(*it == *oit);
        }
        assert(oit == oracle.end());
    }

    {
        auto rit = s.rbegin();
        auto orit = oracle.rbegin();
        int prev = 1000000;
        bool first = true;
        for (; rit != s.rend(); ++rit, ++orit) {
            if (!first) {
                assert(prev > *rit);
            }
            first = false;
            prev = *rit;
            assert(*rit == *orit);
        }
        assert(orit == oracle.rend());
    }
    std::cout << "reverse iteration ok\n";

    {
        mystl::set<int> copy(s);
        assert(copy.size() == s.size());
        auto a = s.begin();
        auto b = copy.begin();
        for (; a != s.end(); ++a, ++b) {
            assert(*a == *b);
        }
        assert(b == copy.end());

        copy.insert(777);
        assert(copy.contains(777));
        assert(!s.contains(777));
    }

    {
        mystl::set<int> ca;
        ca.insert(-1);
        ca = s;
        assert(ca.size() == s.size());
        assert(!ca.contains(-1));
        assert(ca.contains(30));
    }
    std::cout << "copy ok\n";

    {
        mystl::set<int> src(s);
        std::size_t n = src.size();
        mystl::set<int> moved(mystl::move(src));
        assert(moved.size() == n);
        assert(moved.contains(30));
        assert(src.size() == 0);
    }

    {
        mystl::set<int> src(s);
        std::size_t n = src.size();
        mystl::set<int> dst;
        dst.insert(42);
        dst = mystl::move(src);
        assert(dst.size() == n);
        assert(dst.contains(30));
        assert(!dst.contains(42));
    }
    std::cout << "move ok\n";

    std::cout << "set ok" << '\n';
    return 0;
}
