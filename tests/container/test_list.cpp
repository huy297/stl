#include <iostream>
#include <cassert>
#include <string>

#include "mystl/container/list.hpp"

int main() {

    mystl::list<int> l;
    assert(l.empty());
    assert(l.size() == 0);

    l.push_back(2);
    l.push_front(1);
    l.emplace_back(3);
    l.emplace_front(0);
    assert(l.size() == 4);
    assert(l.front() == 0);
    assert(l.back() == 3);
    std::cout << "after push/emplace size=" << l.size()
              << " front=" << l.front() << " back=" << l.back() << '\n';

    l.pop_front();
    l.pop_back();
    assert(l.size() == 2);
    assert(l.front() == 1);
    assert(l.back() == 2);
    std::cout << "after pops size=" << l.size()
              << " front=" << l.front() << " back=" << l.back() << '\n';

    {
        mystl::list<int> seq{10, 20, 30, 40};
        int expected[] = {10, 20, 30, 40};
        int i = 0;
        for (auto it = seq.begin(); it != seq.end(); ++it) {
            assert(*it == expected[i++]);
        }
        assert(i == 4);

        int sum = 0;
        for (int x : seq) {
            sum += x;
        }
        assert(sum == 100);

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
        std::cout << "reverse iter=" << collected[0] << ' ' << collected[1]
                  << ' ' << collected[2] << ' ' << collected[3] << '\n';
    }

    {
        mystl::list<int> m{1, 2, 3};
        auto it = m.begin();
        ++it;
        auto ins = m.insert(it, 99);
        assert(*ins == 99);
        assert(m.size() == 4);
        {
            int expected[] = {1, 99, 2, 3};
            int i = 0;
            for (int x : m) assert(x == expected[i++]);
        }

        auto er = m.erase(ins);
        assert(*er == 2);
        assert(m.size() == 3);

        auto last = m.begin();
        ++last; ++last;
        auto re = m.erase(m.begin(), last);
        assert(*re == 3);
        assert(m.size() == 1);
        assert(m.front() == 3);
        std::cout << "after insert/erase size=" << m.size()
                  << " front=" << m.front() << '\n';
    }

    {
        mystl::list<int> m{1, 2, 2, 3, 4, 2, 5};
        m.remove(2);
        assert(m.size() == 4);
        {
            int expected[] = {1, 3, 4, 5};
            int i = 0;
            for (int x : m) assert(x == expected[i++]);
        }

        m.remove_if([](int x) { return x % 2 == 0; });
        assert(m.size() == 3);
        {
            int expected[] = {1, 3, 5};
            int i = 0;
            for (int x : m) assert(x == expected[i++]);
        }
        std::cout << "after remove/remove_if size=" << m.size() << '\n';
    }

    {
        mystl::list<int> m{5, 3, 3, 1, 4, 1, 2, 5};
        m.sort();
        {
            int expected[] = {1, 1, 2, 3, 3, 4, 5, 5};
            int i = 0;
            for (int x : m) assert(x == expected[i++]);
            assert(i == 8);
        }
        m.unique();
        assert(m.size() == 5);
        {
            int expected[] = {1, 2, 3, 4, 5};
            int i = 0;
            for (int x : m) assert(x == expected[i++]);
        }

        m.sort([](int a, int b) { return a > b; });
        assert(m.front() == 5);
        assert(m.back() == 1);
        std::cout << "after sort/unique size=" << m.size()
                  << " front=" << m.front() << " back=" << m.back() << '\n';
    }

    {
        mystl::list<int> a{1, 3, 5};
        mystl::list<int> b{2, 4, 6};
        a.merge(b);
        assert(a.size() == 6);
        assert(b.empty());
        int expected[] = {1, 2, 3, 4, 5, 6};
        int i = 0;
        for (int x : a) assert(x == expected[i++]);
        assert(i == 6);
        std::cout << "after merge size=" << a.size() << '\n';
    }

    {
        mystl::list<int> a{1, 2, 5};
        mystl::list<int> b{3, 4};
        auto pos = a.begin();
        ++pos; ++pos;
        a.splice(pos, b);
        assert(a.size() == 5);
        assert(b.empty());
        int expected[] = {1, 2, 3, 4, 5};
        int i = 0;
        for (int x : a) assert(x == expected[i++]);
        assert(i == 5);
        std::cout << "after splice size=" << a.size() << '\n';
    }

    {
        mystl::list<int> m{1, 2, 3, 4};
        m.reverse();
        int expected[] = {4, 3, 2, 1};
        int i = 0;
        for (int x : m) assert(x == expected[i++]);
        assert(m.front() == 4 && m.back() == 1);
    }

    {
        mystl::list<int> orig{7, 8, 9};
        mystl::list<int> copy(orig);
        assert(copy.size() == 3);
        assert(copy == orig);
        copy.push_back(10);
        assert(copy.size() == 4);
        assert(orig.size() == 3);
        assert(copy != orig);
        std::cout << "after copy orig=" << orig.size()
                  << " copy=" << copy.size() << '\n';
    }

    {
        mystl::list<int> orig{7, 8, 9};
        mystl::list<int> moved(mystl::move(orig));
        assert(moved.size() == 3);
        int expected[] = {7, 8, 9};
        int i = 0;
        for (int x : moved) assert(x == expected[i++]);
        assert(i == 3);

        std::cout << "after move moved=" << moved.size() << '\n';
    }

    {
        mystl::list<int> src{1, 2, 3};
        mystl::list<int> dst{9, 9};
        dst = mystl::move(src);
        assert(dst.size() == 3);
        assert(dst.front() == 1 && dst.back() == 3);
    }

    {
        mystl::list<std::string> s;
        s.push_back("hello");
        s.push_front("hi");
        s.emplace_back("world");
        assert(s.size() == 3);
        assert(s.front() == "hi");
        assert(s.back() == "world");

        s.sort();
        assert(s.front() == "hello");
        assert(s.back() == "world");

        std::string joined;
        for (const std::string& w : s) {
            joined += w;
            joined += ' ';
        }
        assert(joined == "hello hi world ");
        std::cout << "string list=" << joined << '\n';
    }

    std::cout << "list ok" << '\n';
    return 0;
}
