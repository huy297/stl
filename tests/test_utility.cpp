#include <iostream>
#include "mystl/utility.hpp"

struct X {
    X() = default;

    X(const X&) {
        std::cout << "copy\n";
    }

    X(X&&) noexcept {
        std::cout << "move\n";
    }
};

int main() {
    X a;
    X b = mystl::move(a);

    int x = 1;
    int y = 2;

    mystl::swap(x, y);

    static_assert(mystl::is_same_v<decltype(mystl::move(x)), int&&>);

    std::cout << x << " " << y << '\n';
}