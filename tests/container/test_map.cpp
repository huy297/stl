#include <iostream>
#include <cassert>
#include <string>
#include <map>

#include "mystl/container/map.hpp"

int main() {

    mystl::map<int, std::string> m;
    assert(m.empty());
    assert(m.size() == 0);

    const int keys[] = {50, 10, 90, 30, 70, 20, 80, 40, 60, 0,
                        5, 95, 35, 65, 15, 85, 25, 75, 45, 55};
    const std::size_t N = sizeof(keys) / sizeof(keys[0]);

    std::map<int, std::string> oracle;
    for (std::size_t i = 0; i < N; ++i) {
        std::string val = "v" + std::to_string(keys[i]);
        auto r = m.insert(mystl::pair<const int, std::string>(keys[i], val));
        assert(r.second);
        assert(r.first->first == keys[i]);
        oracle.insert({keys[i], val});
    }
    assert(m.size() == N);

    {
        auto r = m.insert(mystl::pair<const int, std::string>(50, std::string("dup")));
        assert(!r.second);
        assert(r.first->first == 50);
        assert(r.first->second == "v50");
    }
    std::cout << "after insert size=" << m.size() << '\n';

    {
        auto it = m.begin();
        auto oit = oracle.begin();
        int prev = -1;
        bool first = true;
        for (; it != m.end(); ++it, ++oit) {
            if (!first) {
                assert(prev < it->first);
            }
            first = false;
            prev = it->first;
            assert(it->first == oit->first);
            assert(it->second == oit->second);
        }
        assert(oit == oracle.end());
    }
    std::cout << "ordered iteration ok\n";

    {
        auto it = m.find(70);
        assert(it != m.end());
        assert(it->first == 70);
        assert(it->second == "v70");

        assert(m.find(999) == m.end());
        assert(m.count(70) == 1);
        assert(m.count(999) == 0);
        assert(m.contains(70));
        assert(!m.contains(999));
    }
    std::cout << "find/count/contains ok\n";

    {
        std::size_t before = m.size();
        std::string& ref = m[1000];
        assert(ref.empty());
        assert(m.size() == before + 1);
        assert(m.contains(1000));

        m[1000] = "hello";
        assert(m.at(1000) == "hello");
        assert(m.size() == before + 1);

        m[50] = "updated50";
        assert(m.at(50) == "updated50");

        m.erase(1000);
        m[50] = "v50";
    }
    std::cout << "operator[] ok\n";

    {
        assert(m.at(30) == "v30");

        bool threw = false;
        try {
            (void)m.at(123456);
        } catch (const std::out_of_range&) {
            threw = true;
        }
        assert(threw);

        const mystl::map<int, std::string>& cm = m;
        assert(cm.at(30) == "v30");
        std::cout << "at out_of_range threw=" << threw << '\n';
    }

    {

        auto lb = m.lower_bound(30);
        assert(lb != m.end());
        assert(lb->first == 30);

        auto ub = m.upper_bound(30);
        assert(ub != m.end());
        assert(ub->first == 35);

        auto er = m.equal_range(30);
        assert(er.first == lb);
        assert(er.second == ub);
        std::size_t span = 0;
        for (auto it = er.first; it != er.second; ++it) {
            ++span;
        }
        assert(span == 1);

        auto lb2 = m.lower_bound(31);
        auto ub2 = m.upper_bound(31);
        assert(lb2 == ub2);
        assert(lb2->first == 35);
        auto er2 = m.equal_range(31);
        assert(er2.first == er2.second);

        assert(m.lower_bound(100000) == m.end());
        assert(m.upper_bound(100000) == m.end());
    }
    std::cout << "lower/upper/equal_range ok\n";

    {
        std::size_t before = m.size();
        std::size_t removed = m.erase(90);
        assert(removed == 1);
        assert(m.size() == before - 1);
        assert(!m.contains(90));

        assert(m.erase(90) == 0);
    }

    {
        auto it = m.find(10);
        assert(it != m.end());
        auto next = m.erase(it);
        assert(!m.contains(10));
        assert(next != m.end());
        assert(next->first == 15);
    }
    std::cout << "erase ok\n";

    oracle.erase(90);
    oracle.erase(10);
    {
        assert(m.size() == oracle.size());
        auto it = m.begin();
        auto oit = oracle.begin();
        int prev = -1;
        bool first = true;
        for (; it != m.end(); ++it, ++oit) {
            if (!first) {
                assert(prev < it->first);
            }
            first = false;
            prev = it->first;
            assert(it->first == oit->first);

            assert(it->second == oit->second || it->first == 50);
        }
        assert(oit == oracle.end());
    }
    std::cout << "re-verify ok\n";

    {
        mystl::map<int, std::string> copy(m);
        assert(copy.size() == m.size());

        auto a = m.begin();
        auto b = copy.begin();
        for (; a != m.end(); ++a, ++b) {
            assert(a->first == b->first);
            assert(a->second == b->second);
        }
        assert(b == copy.end());

        copy[777] = "only in copy";
        assert(copy.contains(777));
        assert(!m.contains(777));
    }

    {
        mystl::map<int, std::string> ca;
        ca[1] = "temp";
        ca = m;
        assert(ca.size() == m.size());
        assert(!ca.contains(1));
        assert(ca.contains(30));
    }
    std::cout << "copy ok\n";

    {
        mystl::map<int, std::string> src(m);
        std::size_t n = src.size();
        mystl::map<int, std::string> moved(mystl::move(src));
        assert(moved.size() == n);
        assert(moved.contains(30));
        assert(src.size() == 0);
    }

    {
        mystl::map<int, std::string> src(m);
        std::size_t n = src.size();
        mystl::map<int, std::string> dst;
        dst[42] = "temp";
        dst = mystl::move(src);
        assert(dst.size() == n);
        assert(dst.contains(30));
        assert(!dst.contains(42));
    }
    std::cout << "move ok\n";

    {
        mystl::map<std::string, int> sm;
        sm["banana"] = 2;
        sm["apple"] = 1;
        sm["cherry"] = 3;
        assert(sm.size() == 3);

        auto it = sm.begin();
        assert(it->first == "apple");  ++it;
        assert(it->first == "banana"); ++it;
        assert(it->first == "cherry"); ++it;
        assert(it == sm.end());
        assert(sm.at("banana") == 2);
    }

    std::cout << "map ok" << '\n';
    return 0;
}
