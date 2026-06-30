#include <iostream>
#include <sstream>
#include <cassert>

#include "mystl/string.hpp"

int main() {
    // --- small string (SSO): fits in the 15-char inline buffer ---
    mystl::string small("hello");
    assert(small.size() == 5);
    assert(small.capacity() <= 15);          // still inline
    assert(small == "hello");
    assert(std::strcmp(small.c_str(), "hello") == 0);
    std::cout << "small: " << small << '\n';

    // --- large string (>15 chars => heap) ---
    mystl::string large("this string is definitely longer than fifteen chars");
    assert(large.size() > 15);
    assert(large.capacity() > 15);           // promoted to heap
    assert(large == "this string is definitely longer than fifteen chars");
    std::cout << "large: " << large << '\n';

    // --- append / += / + concat ---
    mystl::string a("foo");
    a.append("bar");                         // append(const char*)
    assert(a == "foobar");
    a += "baz";                              // operator+=(const char*)
    assert(a == "foobarbaz");
    a += mystl::string("!");                 // operator+=(const string&)
    assert(a == "foobarbaz!");
    a += '?';                                // operator+=(char)
    assert(a == "foobarbaz!?");

    mystl::string lhs("ab");
    mystl::string rhs("cd");
    mystl::string cat = lhs + rhs;           // operator+(string, string)
    assert(cat == "abcd");
    assert((lhs + "XY") == "abXY");          // operator+(string, const char*)
    assert(("PQ" + rhs) == "PQcd");          // operator+(const char*, string)

    // concat that crosses into heap territory
    mystl::string grow("0123456789");
    grow = grow + "abcdefghij";              // 20 chars -> heap
    assert(grow.size() == 20);
    assert(grow.capacity() > 15);
    assert(grow == "0123456789abcdefghij");

    // --- substr ---
    mystl::string text("hello world");
    mystl::string hello = text.substr(0, 5);
    mystl::string world = text.substr(6);    // to end
    assert(hello == "hello");
    assert(world == "world");
    assert(text.substr(6, 100) == "world");  // count clamped to size
    std::cout << "substr: " << hello << "|" << world << '\n';

    // --- find / rfind ---
    mystl::string fr("abracadabra");
    assert(fr.find('a') == 0);
    assert(fr.find('a', 1) == 3);
    assert(fr.find('z') == mystl::string::npos);
    assert(fr.find("cad") == 4);
    assert(fr.find("xyz") == mystl::string::npos);
    assert(fr.rfind('a') == 10);
    assert(fr.rfind('b') == 8);
    assert(fr.rfind('z') == mystl::string::npos);

    // --- compare ---
    mystl::string c1("apple");
    mystl::string c2("apply");
    mystl::string c3("apple");
    assert(c1.compare(c3) == 0);
    assert(c1.compare(c2) < 0);
    assert(c2.compare(c1) > 0);
    mystl::string prefix("app");
    assert(prefix.compare(c1) < 0);          // shorter is "less" when prefix

    // --- swap ---
    mystl::string s1("short");
    mystl::string s2("a much longer heap-allocated string value");
    mystl::string s1_orig = s1;
    mystl::string s2_orig = s2;
    s1.swap(s2);
    assert(s1 == s2_orig);
    assert(s2 == s1_orig);
    std::cout << "swapped: " << s1.substr(0, 6) << "... | " << s2 << '\n';

    // --- == and < vs const char* (both orders) ---
    mystl::string cmp("banana");
    assert(cmp == "banana");
    assert("banana" == cmp);
    assert(cmp != "apple");
    assert("apple" != cmp);
    assert(cmp < "cherry");
    assert("apple" < cmp);
    assert(cmp > "apple");
    assert("cherry" > cmp);

    // --- push_back / pop_back ---
    mystl::string pb;
    for (char ch = 'a'; ch <= 'e'; ++ch) {
        pb.push_back(ch);
    }
    assert(pb == "abcde");
    assert(pb.size() == 5);
    pb.pop_back();
    assert(pb == "abcd");
    pb.pop_back();
    assert(pb == "abc");
    assert(pb.back() == 'c');
    assert(pb.front() == 'a');

    // push_back that forces growth from SSO into heap
    mystl::string grower("123456789012345");   // exactly 15 chars, still SSO
    assert(grower.capacity() <= 15);
    grower.push_back('X');                      // 16th char -> heap
    assert(grower.size() == 16);
    assert(grower.capacity() > 15);
    assert(grower == "123456789012345X");

    // --- c_str: null-terminated and matches contents ---
    mystl::string cs("nul-term");
    const char* p = cs.c_str();
    assert(std::strlen(p) == cs.size());
    assert(p[cs.size()] == '\0');
    assert(std::strcmp(p, "nul-term") == 0);

    // --- operator<< into a stream, verify exact bytes ---
    mystl::string out("stream me 1234567890 extra-long");
    std::ostringstream oss;
    oss << out;
    assert(oss.str() == "stream me 1234567890 extra-long");

    // empty string streams nothing
    mystl::string empty;
    std::ostringstream oss2;
    oss2 << empty;
    assert(oss2.str().empty());
    assert(empty.empty());

    std::cout << "string ok" << '\n';
    return 0;
}
