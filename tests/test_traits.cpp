#include <iostream>
#include "mystl/type_traits.hpp"

int main() {
    static_assert(mystl::is_same_v<int, int>);
    static_assert(!mystl::is_same_v<int, double>);

    static_assert(mystl::is_same_v<
        mystl::remove_reference_t<int&>,
        int
    >);

    static_assert(mystl::is_same_v<
        mystl::remove_cvref_t<const int&>,
        int
    >);

    std::cout << "type_traits ok\n";
}