#include <iostream>
#include <cassert>
#include "mystl/utility/functional.hpp"

int main() {

    mystl::less<int> lt;
    mystl::greater<int> gt;
    assert(lt(2, 5));
    assert(!lt(5, 2));
    assert(!lt(3, 3));
    assert(gt(5, 2));
    assert(!gt(2, 5));

    mystl::plus<int> add;
    mystl::minus<int> sub;
    mystl::multiplies<int> mul;
    mystl::modulus<int> mod;
    mystl::negate<int> neg;
    assert(add(2, 3) == 5);
    assert(sub(7, 4) == 3);
    assert(mul(6, 7) == 42);
    assert(mod(17, 5) == 2);
    assert(neg(8) == -8);
    assert(neg(-4) == 4);

    mystl::logical_and<bool> land;
    mystl::logical_not<bool> lnot;
    assert(land(true, true));
    assert(!land(true, false));
    assert(!land(false, false));
    assert(lnot(false));
    assert(!lnot(true));

    mystl::bit_xor<int> bxor;
    assert(bxor(0b1100, 0b1010) == 0b0110);
    assert(bxor(5, 5) == 0);

    static_assert(2 + 3 == 5);
    static_assert(9 - 4 == 5);
    static_assert(3 * 4 == 12);
    static_assert(10 % 3 == 1);
    static_assert(-(7) == -7);
    static_assert((6 ^ 3) == 5);

    std::cout << "functional ok\n";
}
