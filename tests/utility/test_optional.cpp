#include <iostream>
#include <string>

#include "mystl/utility/optional.hpp"

int main() {
    mystl::optional<std::string> op;

    std::cout << op.has_value() << '\n';

    op.emplace("hello");

    std::cout << op.has_value() << '\n';
    std::cout << op.value() << '\n';

    op.reset();

    std::cout << op.has_value() << '\n';

    op.emplace("world");

    std::cout << *op << '\n';
    std::cout << op->size() << '\n';

    mystl::optional<std::string> copy = op;
    std::cout << copy.value() << '\n';

    mystl::optional<std::string> moved = mystl::move(op);
    std::cout << moved.value() << '\n';
    std::cout << op.has_value() << '\n';
}
