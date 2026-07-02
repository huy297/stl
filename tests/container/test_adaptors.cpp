#include <iostream>
#include <cassert>
#include <string>

#include "mystl/container/stack.hpp"
#include "mystl/container/queue.hpp"
#include "mystl/container/priority_queue.hpp"

int main() {

    {
        mystl::stack<int> s;
        assert(s.empty());
        assert(s.size() == 0);

        s.push(1);
        s.push(2);
        s.emplace(3);
        assert(s.size() == 3);
        assert(s.top() == 3);

        s.top() = 30;
        assert(s.top() == 30);

        int expected[] = {30, 2, 1};
        int k = 0;
        while (!s.empty()) {
            assert(s.top() == expected[k]);
            s.pop();
            ++k;
        }
        assert(k == 3);
        assert(s.empty());
        assert(s.size() == 0);
        std::cout << "stack<int> LIFO ok\n";
    }

    {
        mystl::stack<int, mystl::vector<int>> s;
        for (int i = 0; i < 5; ++i) {
            s.push(i);
        }
        assert(s.size() == 5);
        assert(s.top() == 4);

        int expect = 4;
        while (!s.empty()) {
            assert(s.top() == expect);
            s.pop();
            --expect;
        }
        assert(expect == -1);
        assert(s.empty());
        std::cout << "stack<int, vector> LIFO ok\n";
    }

    {
        mystl::stack<std::string> s;
        s.push("a");
        s.push(std::string("b"));
        s.emplace(3, 'c');
        assert(s.size() == 3);
        assert(s.top() == "ccc");

        const char* expected[] = {"ccc", "b", "a"};
        int k = 0;
        while (!s.empty()) {
            assert(s.top() == expected[k]);
            s.pop();
            ++k;
        }
        assert(k == 3);
        std::cout << "stack<string> LIFO ok\n";
    }

    {
        mystl::queue<int> q;
        assert(q.empty());
        assert(q.size() == 0);

        q.push(10);
        q.push(20);
        q.emplace(30);
        assert(q.size() == 3);
        assert(q.front() == 10);
        assert(q.back() == 30);

        q.front() = 11;
        q.back() = 33;
        assert(q.front() == 11);
        assert(q.back() == 33);

        int expected[] = {11, 20, 33};
        int k = 0;
        while (!q.empty()) {
            assert(q.front() == expected[k]);
            q.pop();
            ++k;
        }
        assert(k == 3);
        assert(q.empty());
        std::cout << "queue<int> FIFO ok\n";
    }

    {
        mystl::queue<std::string> q;
        q.push("first");
        q.push(std::string("second"));
        q.emplace(3, 'z');
        assert(q.size() == 3);
        assert(q.front() == "first");
        assert(q.back() == "zzz");

        const char* expected[] = {"first", "second", "zzz"};
        int k = 0;
        while (!q.empty()) {
            assert(q.front() == expected[k]);
            q.pop();
            ++k;
        }
        assert(k == 3);
        std::cout << "queue<string> FIFO ok\n";
    }

    {
        mystl::priority_queue<int> pq;
        int input[] = {5, 1, 8, 3, 9, 2, 7, 0, 6, 4};
        for (int x : input) {
            pq.push(x);
        }
        pq.emplace(10);
        assert(pq.size() == 11);
        assert(pq.top() == 10);

        int prev = pq.top();
        pq.pop();
        int count = 1;
        while (!pq.empty()) {
            int cur = pq.top();
            assert(cur <= prev);
            prev = cur;
            pq.pop();
            ++count;
        }
        assert(count == 11);
        assert(pq.empty());
        std::cout << "priority_queue<int> max-heap descending ok\n";
    }

    {
        mystl::priority_queue<int, mystl::vector<int>, mystl::greater<int>> pq;
        int input[] = {5, 1, 8, 3, 9, 2, 7, 0, 6, 4};
        for (int x : input) {
            pq.push(x);
        }
        assert(pq.size() == 10);
        assert(pq.top() == 0);

        int prev = pq.top();
        pq.pop();
        int count = 1;
        while (!pq.empty()) {
            int cur = pq.top();
            assert(cur >= prev);
            prev = cur;
            pq.pop();
            ++count;
        }
        assert(count == 10);
        assert(pq.empty());
        std::cout << "priority_queue<int, greater> min-heap ascending ok\n";
    }

    {
        mystl::priority_queue<int> pq;
        unsigned int state = 123456789u;
        const int N = 1000;
        for (int i = 0; i < N; ++i) {

            state = 1664525u * state + 1013904223u;
            int value = static_cast<int>(state >> 16) & 0x7fff;
            pq.push(value);
        }
        assert(pq.size() == static_cast<mystl::priority_queue<int>::size_type>(N));

        int prev = pq.top();
        pq.pop();
        int count = 1;
        while (!pq.empty()) {
            int cur = pq.top();
            assert(cur <= prev);
            prev = cur;
            pq.pop();
            ++count;
        }
        assert(count == N);
        assert(pq.empty());
        std::cout << "priority_queue LCG drain non-increasing ok (n=" << N << ")\n";
    }

    std::cout << "adaptors ok" << '\n';
    return 0;
}
