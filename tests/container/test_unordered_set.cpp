#include <iostream>
#include <cassert>

#include "mystl/container/unordered_set.hpp"
#include "mystl/container/string.hpp"

int main() {

    mystl::unordered_set<int> s;
    assert(s.empty());
    assert(s.size() == 0);

    {
        auto r = s.insert(10);
        assert(r.second == true);
        assert(*r.first == 10);
    }
    assert(s.size() == 1);

    {
        auto r = s.insert(10);
        assert(r.second == false);
        assert(*r.first == 10);
    }
    assert(s.size() == 1);

    int seed[] = {1, 2, 3, 2, 1, 4, 5, 5, 10, 3};
    for (int x : seed) {
        s.insert(x);
    }

    assert(s.size() == 6);
    assert(s.contains(1) && s.contains(2) && s.contains(3));
    assert(s.contains(4) && s.contains(5) && s.contains(10));
    assert(!s.contains(999));
    std::cout << "after dup insert size=" << s.size() << '\n';

    assert(s.count(1) == 1);
    assert(s.count(999) == 0);
    assert(s.contains(5));
    assert(!s.contains(6));

    assert(s.erase(2) == 1);
    assert(!s.contains(2));
    assert(s.count(2) == 0);
    assert(s.erase(2) == 0);
    assert(s.erase(12345) == 0);
    assert(s.size() == 5);
    std::cout << "after erase size=" << s.size() << '\n';

    const int M = 500;
    mystl::unordered_set<int> big;
    for (int i = 0; i < M; ++i) {
        auto r = big.insert(i);
        assert(r.second == true);
    }
    assert(big.size() == static_cast<std::size_t>(M));

    for (int i = 0; i < M; ++i) {
        auto r = big.insert(i);
        assert(r.second == false);
    }
    assert(big.size() == static_cast<std::size_t>(M));
    std::cout << "stress size=" << big.size()
              << " bucket_count=" << big.bucket_count() << '\n';

    for (int i = 0; i < M; ++i) {
        assert(big.contains(i));
        assert(big.count(i) == 1);
        assert(*big.find(i) == i);
    }
    for (int i = M; i < M + 50; ++i) {
        assert(!big.contains(i));
        assert(big.count(i) == 0);
        assert(big.find(i) == big.end());
    }

    std::size_t erased = 0;
    for (int i = 0; i < M; i += 2) {
        assert(big.erase(i) == 1);
        ++erased;
    }
    assert(big.size() == static_cast<std::size_t>(M) - erased);
    for (int i = 0; i < M; ++i) {
        if (i % 2 == 0) {
            assert(!big.contains(i));
        } else {
            assert(big.contains(i));
        }
    }
    std::cout << "after erasing " << erased << " size=" << big.size() << '\n';

    {
        std::size_t counted = 0;
        for (auto it = big.begin(); it != big.end(); ++it) {
            assert(*it % 2 == 1);
            ++counted;
        }
        assert(counted == big.size());

        std::size_t counted2 = 0;
        for (const auto& v : big) {
            (void)v;
            ++counted2;
        }
        assert(counted2 == big.size());
        std::cout << "iteration counted=" << counted << " size=" << big.size() << '\n';
    }

    mystl::unordered_set<int> scopy(big);
    assert(scopy.size() == big.size());
    for (int i = 1; i < M; i += 2) {
        assert(scopy.contains(i));
    }

    scopy.insert(0);
    assert(scopy.contains(0));
    assert(!big.contains(0));
    assert(scopy.size() == big.size() + 1);

    mystl::unordered_set<int> sassign;
    sassign.insert(-1);
    sassign = big;
    assert(sassign.size() == big.size());
    assert(!sassign.contains(-1));
    for (int i = 1; i < M; i += 2) {
        assert(sassign.contains(i));
    }
    std::cout << "copy verified copy_size=" << scopy.size() << '\n';

    std::size_t origSize = big.size();
    mystl::unordered_set<int> smove(mystl::move(big));
    assert(smove.size() == origSize);
    for (int i = 1; i < M; i += 2) {
        assert(smove.contains(i));
    }

    mystl::unordered_set<int> smove2;
    smove2.insert(7);
    smove2 = mystl::move(smove);
    assert(smove2.size() == origSize);
    for (int i = 1; i < M; i += 2) {
        assert(smove2.contains(i));
    }
    std::cout << "move verified move_size=" << smove2.size() << '\n';

    mystl::unordered_set<mystl::string> ss;
    assert(ss.insert(mystl::string("red")).second == true);
    assert(ss.insert(mystl::string("green")).second == true);
    assert(ss.insert(mystl::string("blue")).second == true);
    assert(ss.insert(mystl::string("red")).second == false);
    assert(ss.size() == 3);
    assert(ss.contains(mystl::string("green")));
    assert(!ss.contains(mystl::string("yellow")));
    assert(ss.erase(mystl::string("green")) == 1);
    assert(ss.size() == 2);
    std::cout << "string-set verified size=" << ss.size() << '\n';

    std::cout << "unordered_set ok" << '\n';
    return 0;
}
