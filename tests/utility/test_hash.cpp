#include <iostream>
#include <cassert>
#include "mystl/utility/hash.hpp"

int main() {

    mystl::hash<int> hi;
    assert(hi(42) == hi(42));
    assert(hi(0) == hi(0));
    assert(hi(-7) == hi(-7));

    mystl::hash<unsigned int> hu;
    assert(hu(123u) == hu(123u));

    mystl::hash<long long> hll;
    assert(hll(1234567890123LL) == hll(1234567890123LL));

    mystl::hash<char> hc;
    assert(hc('a') == hc('a'));

    mystl::hash<double> hd;
    assert(hd(3.14) == hd(3.14));
    assert(hd(0.0) == hd(-0.0));

    int obj = 5;
    int* p = &obj;
    mystl::hash<int*> hp;
    assert(hp(p) == hp(p));
    assert(hp(nullptr) == hp(nullptr));

    mystl::hash<mystl::string> hs;
    mystl::string a("hello world");
    mystl::string b("hello world");
    assert(a == b);
    assert(hs(a) == hs(b));

    mystl::string c("foo");
    mystl::string d("bar");
    mystl::string e("");
    assert(hs(c) != hs(d));
    assert(hs(c) != hs(e));
    assert(hs(mystl::string("abc")) != hs(mystl::string("abd")));

    std::cout << "hash ok\n";
}
