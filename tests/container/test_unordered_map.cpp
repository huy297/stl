#include <iostream>
#include <cassert>
#include <stdexcept>

#include "mystl/container/unordered_map.hpp"
#include "mystl/container/string.hpp"

int main() {

    const int N = 1000;
    mystl::unordered_map<int, int> m;
    assert(m.empty());
    assert(m.size() == 0);

    for (int i = 0; i < N; ++i) {
        auto r = m.insert({i, i * 2});
        assert(r.second == true);
        assert(r.first->first == i);
        assert(r.first->second == i * 2);
    }
    assert(m.size() == static_cast<std::size_t>(N));
    assert(!m.empty());
    std::cout << "after bulk insert size=" << m.size()
              << " bucket_count=" << m.bucket_count() << '\n';

    {
        auto r = m.insert({500, -1});
        assert(r.second == false);
        assert(r.first->second == 1000);
    }
    assert(m.size() == static_cast<std::size_t>(N));

    for (int i = 0; i < N; ++i) {
        auto it = m.find(i);
        assert(it != m.end());
        assert(it->first == i);
        assert(it->second == i * 2);
        assert(m.count(i) == 1);
        assert(m.contains(i));
    }

    for (int i = N; i < N + 50; ++i) {
        assert(m.find(i) == m.end());
        assert(m.count(i) == 0);
        assert(!m.contains(i));
    }
    std::cout << "find/count/contains verified for present and absent keys\n";

    m[0] = 12345;
    assert(m[0] == 12345);
    assert(m.size() == static_cast<std::size_t>(N));

    int newKey = N + 1000;
    assert(!m.contains(newKey));
    int defVal = m[newKey];
    assert(defVal == 0);
    assert(m.contains(newKey));
    assert(m.size() == static_cast<std::size_t>(N) + 1);
    m[newKey] = 777;
    assert(m[newKey] == 777);
    std::cout << "operator[] update + insert-default verified\n";

    m[0] = 0;
    m.erase(newKey);
    assert(!m.contains(newKey));
    assert(m.size() == static_cast<std::size_t>(N));

    assert(m.at(3) == 6);
    m.at(3) = 99;
    assert(m.at(3) == 99);
    m.at(3) = 6;

    bool threw = false;
    try {
        (void)m.at(N + 5000);
    } catch (const std::out_of_range&) {
        threw = true;
    }
    assert(threw);
    std::cout << "at() out_of_range threw=" << threw << '\n';

    {
        const mystl::unordered_map<int, int>& cm = m;
        assert(cm.at(7) == 14);
        bool cthrew = false;
        try {
            (void)cm.at(N + 6000);
        } catch (const std::out_of_range&) {
            cthrew = true;
        }
        assert(cthrew);
    }

    std::size_t erased = 0;
    for (int i = 0; i < N; i += 2) {
        std::size_t c = m.erase(i);
        assert(c == 1);
        ++erased;
    }

    assert(m.erase(0) == 0);
    assert(m.erase(N + 9999) == 0);

    assert(m.size() == static_cast<std::size_t>(N) - erased);
    std::cout << "after erasing " << erased << " keys size=" << m.size() << '\n';

    for (int i = 0; i < N; ++i) {
        if (i % 2 == 0) {
            assert(!m.contains(i));
            assert(m.find(i) == m.end());
            assert(m.count(i) == 0);
        } else {
            assert(m.contains(i));
            auto it = m.find(i);
            assert(it != m.end());
            assert(it->second == i * 2);
        }
    }

    {
        std::size_t counted = 0;
        for (auto it = m.begin(); it != m.end(); ++it) {

            assert(it->first % 2 == 1);
            assert(it->second == it->first * 2);
            ++counted;
        }
        assert(counted == m.size());

        std::size_t counted2 = 0;
        for (const auto& kv : m) {
            (void)kv;
            ++counted2;
        }
        assert(counted2 == m.size());
        std::cout << "iteration counted=" << counted << " size=" << m.size() << '\n';
    }

    mystl::unordered_map<mystl::string, int> sm;
    sm.insert({mystl::string("alpha"), 1});
    sm.insert({mystl::string("beta"), 2});
    sm.insert({mystl::string("gamma"), 3});
    assert(sm.size() == 3);

    assert(sm.contains(mystl::string("alpha")));
    assert(sm.contains(mystl::string("beta")));
    assert(sm.contains(mystl::string("gamma")));
    assert(!sm.contains(mystl::string("delta")));

    assert(sm.at(mystl::string("alpha")) == 1);
    assert(sm.find(mystl::string("beta"))->second == 2);
    assert(sm.count(mystl::string("gamma")) == 1);
    assert(sm.count(mystl::string("delta")) == 0);

    sm[mystl::string("alpha")] = 100;
    assert(sm[mystl::string("alpha")] == 100);

    assert(sm[mystl::string("delta")] == 0);
    assert(sm.contains(mystl::string("delta")));
    assert(sm.size() == 4);
    std::cout << "string-keyed map verified size=" << sm.size() << '\n';

    mystl::unordered_map<int, int> mcopy(m);
    assert(mcopy.size() == m.size());
    for (int i = 1; i < N; i += 2) {
        assert(mcopy.contains(i));
        assert(mcopy.at(i) == i * 2);
    }

    mcopy[1] = -1;
    assert(mcopy.at(1) == -1);
    assert(m.at(1) == 2);

    mystl::unordered_map<int, int> massign;
    massign.insert({999999, 1});
    massign = m;
    assert(massign.size() == m.size());
    assert(!massign.contains(999999));
    for (int i = 1; i < N; i += 2) {
        assert(massign.contains(i));
    }
    std::cout << "copy verified copy_size=" << mcopy.size() << '\n';

    std::size_t origSize = m.size();
    mystl::unordered_map<int, int> mmove(mystl::move(m));
    assert(mmove.size() == origSize);
    for (int i = 1; i < N; i += 2) {
        assert(mmove.contains(i));
        assert(mmove.at(i) == i * 2);
    }

    mystl::unordered_map<int, int> mmove2;
    mmove2.insert({42, 42});
    mmove2 = mystl::move(mmove);
    assert(mmove2.size() == origSize);
    for (int i = 1; i < N; i += 2) {
        assert(mmove2.contains(i));
    }
    std::cout << "move verified move_size=" << mmove2.size() << '\n';

    std::cout << "unordered_map ok" << '\n';
    return 0;
}
