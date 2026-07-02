#include <iostream>
#include <cassert>
#include <string>

#include "mystl/container/deque.hpp"

int main() {

    mystl::deque<int> d;
    assert(d.empty());
    assert(d.size() == 0);

    d.push_back(1);
    d.push_back(2);
    d.push_front(0);
    d.emplace_back(3);
    d.emplace_front(-1);
    assert(d.size() == 5);
    assert(d.front() == -1);
    assert(d.back() == 3);

    assert(d[0] == -1);
    assert(d[4] == 3);
    assert(d.at(2) == 1);

    bool threw = false;
    try {
        (void)d.at(99);
    } catch (const std::out_of_range&) {
        threw = true;
    }
    assert(threw);
    std::cout << "at out_of_range threw=" << threw << '\n';

    d.pop_front();
    d.pop_back();
    assert(d.size() == 3);
    assert(d.front() == 0);
    assert(d.back() == 2);
    {
        int expected[] = {0, 1, 2};
        for (std::size_t i = 0; i < d.size(); ++i) {
            assert(d[i] == expected[i]);
        }
    }
    std::cout << "after basic ops size=" << d.size()
              << " front=" << d.front() << " back=" << d.back() << '\n';

    mystl::deque<int> big;
    const int N = 3000;

    for (int i = 0; i < N; ++i) {
        big.push_back(i);
    }
    for (int i = 1; i <= N; ++i) {
        big.push_front(-i);
    }
    assert(big.size() == static_cast<std::size_t>(2 * N));

    assert(big.front() == -N);
    assert(big.back() == N - 1);

    assert(big[0] == -N);
    assert(big[N] == 0);
    assert(big[2 * N - 1] == N - 1);

    for (int i = 0; i < 2 * N; ++i) {
        int expected = i - N;
        assert(big[static_cast<std::size_t>(i)] == expected);
    }
    std::cout << "multi-block size=" << big.size()
              << " front=" << big.front() << " back=" << big.back() << '\n';

    long long fwd_sum = 0;
    long long count_fwd = 0;
    for (auto it = big.begin(); it != big.end(); ++it) {
        fwd_sum += *it;
        ++count_fwd;
    }
    assert(count_fwd == 2 * N);
    long long expected_sum = -static_cast<long long>(N);
    assert(fwd_sum == expected_sum);

    long long rev_sum = 0;
    long long count_rev = 0;
    int prev = 0;
    bool first = true;
    for (auto it = big.rbegin(); it != big.rend(); ++it) {
        rev_sum += *it;

        if (!first) {
            assert(*it == prev - 1);
        }
        prev = *it;
        first = false;
        ++count_rev;
    }
    assert(count_rev == 2 * N);
    assert(rev_sum == fwd_sum);
    std::cout << "sum fwd=" << fwd_sum << " rev=" << rev_sum << '\n';

    {
        auto it = big.begin();

        it += N;
        assert(*it == 0);

        assert(it - big.begin() == N);

        assert(it[0] == 0);
        assert(it[5] == 5);
        assert(it[-1] == -1);

        auto it2 = it + 10;
        assert(*it2 == 10);
        auto it3 = it2 - 3;
        assert(*it3 == 7);

        assert(it2 - it == 10);

        assert(big.begin() < it);
        assert(it <= it);
        assert(it2 > it);
        assert(it >= big.begin());

        assert(big.end() - big.begin() == static_cast<std::ptrdiff_t>(big.size()));
    }
    std::cout << "iterator arithmetic ok\n";

    mystl::deque<int> m{10, 20, 30, 40, 50};
    assert(m.size() == 5);

    auto pit = m.insert(m.begin(), 5);
    assert(*pit == 5);
    assert(m.front() == 5);
    assert(m.size() == 6);

    pit = m.insert(m.begin() + 3, 25);
    assert(*pit == 25);
    assert(m[3] == 25);
    assert(m.size() == 7);

    pit = m.insert(m.end(), 60);
    assert(*pit == 60);
    assert(m.back() == 60);
    assert(m.size() == 8);
    {
        int expected[] = {5, 10, 20, 25, 30, 40, 50, 60};
        for (std::size_t i = 0; i < m.size(); ++i) {
            assert(m[i] == expected[i]);
        }
    }
    std::cout << "after inserts size=" << m.size() << '\n';

    auto eit = m.erase(m.begin() + 3);
    assert(*eit == 30);
    assert(m.size() == 7);

    eit = m.erase(m.begin() + 1, m.begin() + 4);
    assert(*eit == 40);
    assert(m.size() == 4);
    {
        int expected[] = {5, 40, 50, 60};
        for (std::size_t i = 0; i < m.size(); ++i) {
            assert(m[i] == expected[i]);
        }
    }
    std::cout << "after erase size=" << m.size() << '\n';

    m.resize(6);
    assert(m.size() == 6);
    assert(m[4] == 0 && m[5] == 0);
    m.resize(8, 9);
    assert(m.size() == 8);
    assert(m[6] == 9 && m[7] == 9);
    m.resize(3);
    assert(m.size() == 3);
    assert(m.back() == 50);
    std::cout << "after resize size=" << m.size() << " back=" << m.back() << '\n';

    mystl::deque<int> copy_src{1, 2, 3, 4, 5};
    mystl::deque<int> copy_c(copy_src);
    assert(copy_c.size() == copy_src.size());
    assert(copy_c == copy_src);

    copy_c[0] = 100;
    assert(copy_src[0] == 1);
    assert(copy_c[0] == 100);

    mystl::deque<int> copy_a;
    copy_a = copy_src;
    assert(copy_a == copy_src);
    std::cout << "copy ok size=" << copy_c.size() << '\n';

    mystl::deque<int> move_src{7, 8, 9};
    mystl::deque<int> move_c(std::move(move_src));
    assert(move_c.size() == 3);
    assert(move_c[0] == 7 && move_c[2] == 9);

    mystl::deque<int> move_a;
    mystl::deque<int> move_src2{11, 12};
    move_a = std::move(move_src2);
    assert(move_a.size() == 2);
    assert(move_a[0] == 11 && move_a[1] == 12);
    std::cout << "move ok size=" << move_c.size() << '\n';

    mystl::deque<std::string> ds;
    ds.push_back("world");
    ds.push_front("hello");
    ds.emplace_back("!");
    ds.emplace_front(">>");

    assert(ds.size() == 4);
    assert(ds.front() == ">>");
    assert(ds.back() == "!");
    assert(ds[1] == "hello");
    assert(ds.at(2) == "world");

    mystl::deque<std::string> dsn;
    const int SN = 400;
    for (int i = 0; i < SN; ++i) {
        dsn.push_back(std::string("b") + std::to_string(i));
    }
    for (int i = 0; i < SN; ++i) {
        dsn.push_front(std::string("f") + std::to_string(i));
    }
    assert(dsn.size() == static_cast<std::size_t>(2 * SN));

    assert(dsn.front() == std::string("f") + std::to_string(SN - 1));
    assert(dsn.back() == std::string("b") + std::to_string(SN - 1));
    assert(dsn[SN] == "b0");

    std::size_t str_iter_count = 0;
    for (const auto& s : dsn) {
        assert(!s.empty());
        ++str_iter_count;
    }
    assert(str_iter_count == static_cast<std::size_t>(2 * SN));

    mystl::deque<std::string> ds_copy(ds);
    assert(ds_copy == ds);
    ds_copy[0] = "changed";
    assert(ds.front() == ">>");
    mystl::deque<std::string> ds_move(std::move(ds_copy));
    assert(ds_move.size() == 4);
    assert(ds_move.front() == "changed");
    std::cout << "string deque size=" << ds.size()
              << " big=" << dsn.size() << '\n';

    mystl::deque<int> s1{1, 2, 3};
    mystl::deque<int> s2{9, 8};
    mystl::swap(s1, s2);
    assert(s1.size() == 2 && s1[0] == 9 && s1[1] == 8);
    assert(s2.size() == 3 && s2[0] == 1 && s2[2] == 3);

    mystl::deque<int> ca{1, 2, 3};
    mystl::deque<int> cb{1, 2, 3};
    mystl::deque<int> cc{1, 2, 4};
    mystl::deque<int> cd{1, 2};
    assert(ca == cb);
    assert(ca != cc);
    assert(ca < cc);
    assert(cd < ca);
    assert(!(cc < ca));
    std::cout << "compare (ca==cb)=" << (ca == cb)
              << " (ca<cc)=" << (ca < cc) << '\n';

    ca.clear();
    assert(ca.empty());
    assert(ca.size() == 0);

    ca.push_back(42);
    assert(ca.size() == 1 && ca.front() == 42);

    std::cout << "deque ok" << '\n';
    return 0;
}
