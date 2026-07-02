#include <iostream>
#include <string>
#include "mystl/utility/pair.hpp"

int main() {
    mystl::pair<int, std::string> p(1, "hello");

    std::cout << p.first << " " << p.second << '\n';

    auto q = mystl::make_pair(2, std::string("world"));

    std::cout << q.first << " " << q.second << '\n';
}
