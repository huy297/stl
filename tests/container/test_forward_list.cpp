#include <iostream>
#include <cassert>
#include <string>

#include "mystl/container/forward_list.hpp"

int main() {

    mystl::forward_list<int> l;
    assert(l.empty());

    l.push_front(3);
    l.push_front(2);
    l.emplace_front(1);
    assert(!l.empty());
    assert(l.front() == 1);
    {
        int expected[] = {1, 2, 3};
        int i = 0;
        for (int x : l) assert(x == expected[i++]);
        assert(i == 3);
    }
    std::cout << "after push/emplace front=" << l.front() << '\n';

    l.pop_front();
    assert(l.front() == 2);
    std::cout << "after pop_front front=" << l.front() << '\n';

    {
        mystl::forward_list<int> m{10, 20, 30};

        auto ins = m.insert_after(m.before_begin(), 5);
        assert(*ins == 5);
        assert(m.front() == 5);

        auto it = m.begin();
        m.insert_after(it, 7);
        {
            int expected[] = {5, 7, 10, 20, 30};
            int i = 0;
            for (int x : m) assert(x == expected[i++]);
            assert(i == 5);
        }

        m.emplace_after(m.before_begin(), 1);
        assert(m.front() == 1);

        auto er = m.erase_after(m.before_begin());
        assert(*er == 5);
        assert(m.front() == 5);

        auto first = m.begin();
        auto last = m.begin();
        ++last; ++last; ++last;
        auto re = m.erase_after(first, last);
        assert(*re == 20);
        {
            int expected[] = {5, 20, 30};
            int i = 0;
            for (int x : m) assert(x == expected[i++]);
            assert(i == 3);
        }
        std::cout << "after insert/erase_after front=" << m.front() << '\n';
    }

    {
        mystl::forward_list<int> seq{1, 2, 3, 4};
        int sum = 0;
        for (auto it = seq.cbegin(); it != seq.cend(); ++it) {
            sum += *it;
        }
        assert(sum == 10);

        const mystl::forward_list<int>& cref = seq;
        int csum = 0;
        for (int x : cref) csum += x;
        assert(csum == 10);
        std::cout << "iteration sum=" << sum << '\n';
    }

    {
        mystl::forward_list<int> m{5, 3, 3, 1, 4, 1, 2, 5};
        m.sort();
        {
            int expected[] = {1, 1, 2, 3, 3, 4, 5, 5};
            int i = 0;
            for (int x : m) assert(x == expected[i++]);
            assert(i == 8);
        }
        m.unique();
        {
            int expected[] = {1, 2, 3, 4, 5};
            int i = 0;
            for (int x : m) assert(x == expected[i++]);
            assert(i == 5);
        }
        m.reverse();
        {
            int expected[] = {5, 4, 3, 2, 1};
            int i = 0;
            for (int x : m) assert(x == expected[i++]);
            assert(i == 5);
        }
        assert(m.front() == 5);

        m.sort([](int a, int b) { return a > b; });
        assert(m.front() == 5);
        std::cout << "after sort/unique/reverse front=" << m.front() << '\n';
    }

    {
        mystl::forward_list<int> m{1, 2, 3, 4, 5, 6};
        m.remove_if([](int x) { return x % 2 == 0; });
        {
            int expected[] = {1, 3, 5};
            int i = 0;
            for (int x : m) assert(x == expected[i++]);
            assert(i == 3);
        }

        m.remove(3);
        {
            int expected[] = {1, 5};
            int i = 0;
            for (int x : m) assert(x == expected[i++]);
            assert(i == 2);
        }
        std::cout << "after remove_if/remove front=" << m.front() << '\n';
    }

    {
        mystl::forward_list<int> a{1, 3, 5};
        mystl::forward_list<int> b{2, 4, 6};
        a.merge(b);
        assert(b.empty());
        int expected[] = {1, 2, 3, 4, 5, 6};
        int i = 0;
        for (int x : a) assert(x == expected[i++]);
        assert(i == 6);
        std::cout << "after merge front=" << a.front() << '\n';
    }

    {
        mystl::forward_list<int> a{1, 2, 5};
        mystl::forward_list<int> b{3, 4};
        auto pos = a.begin();
        ++pos;
        a.splice_after(pos, b);
        assert(b.empty());
        int expected[] = {1, 2, 3, 4, 5};
        int i = 0;
        for (int x : a) assert(x == expected[i++]);
        assert(i == 5);
        std::cout << "after splice_after front=" << a.front() << '\n';
    }

    {
        mystl::forward_list<int> orig{7, 8, 9};
        mystl::forward_list<int> copy(orig);
        assert(copy == orig);
        copy.push_front(6);
        assert(copy.front() == 6);
        assert(orig.front() == 7);
        assert(copy != orig);
        std::cout << "after copy orig_front=" << orig.front()
                  << " copy_front=" << copy.front() << '\n';
    }

    {
        mystl::forward_list<int> orig{7, 8, 9};
        mystl::forward_list<int> moved(mystl::move(orig));
        int expected[] = {7, 8, 9};
        int i = 0;
        for (int x : moved) assert(x == expected[i++]);
        assert(i == 3);

        std::cout << "after move moved_front=" << moved.front() << '\n';
    }

    {
        mystl::forward_list<int> src{1, 2, 3};
        mystl::forward_list<int> dst{9, 9};
        dst = mystl::move(src);
        int expected[] = {1, 2, 3};
        int i = 0;
        for (int x : dst) assert(x == expected[i++]);
        assert(i == 3);
    }

    {
        mystl::forward_list<std::string> s;
        s.push_front("world");
        s.push_front("hello");
        s.emplace_front("say");
        assert(s.front() == "say");

        s.sort();
        assert(s.front() == "hello");

        std::string joined;
        for (const std::string& w : s) {
            joined += w;
            joined += ' ';
        }
        assert(joined == "hello say world ");
        std::cout << "string forward_list=" << joined << '\n';
    }

    std::cout << "forward_list ok" << '\n';
    return 0;
}
