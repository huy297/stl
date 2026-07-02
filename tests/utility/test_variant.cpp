#include <iostream>
#include <cassert>
#include <string>

#include "mystl/utility/variant.hpp"

struct Describe {
    std::string operator()(int x) const {
        return std::string("int:") + std::to_string(x);
    }
    std::string operator()(double x) const {
        return std::string("double:") + std::to_string(x);
    }
    std::string operator()(mystl::monostate) const {
        return std::string("monostate");
    }
};

int main() {
    using V = mystl::variant<int, double, mystl::monostate>;

    V v(42);
    assert(v.index() == 0);
    assert(mystl::holds_alternative<int>(v));
    assert(!mystl::holds_alternative<double>(v));
    assert(!mystl::holds_alternative<mystl::monostate>(v));
    std::cout << "index after int construct: " << v.index() << '\n';

    assert(mystl::get<0>(v) == 42);
    assert(mystl::get<int>(v) == 42);
    std::cout << "get<int>(v) = " << mystl::get<int>(v) << '\n';

    int* pi = mystl::get_if<int>(&v);
    assert(pi != nullptr);
    assert(*pi == 42);
    double* pd = mystl::get_if<double>(&v);
    assert(pd == nullptr);
    std::cout << "get_if<int> ok, get_if<double> null\n";

    std::string desc = mystl::visit(Describe{}, v);
    assert(desc == "int:42");
    std::cout << "visit -> " << desc << '\n';

    V copy = v;
    assert(copy.index() == 0);
    assert(mystl::holds_alternative<int>(copy));
    assert(mystl::get<int>(copy) == 42);

    mystl::get<int>(v) = 7;
    assert(mystl::get<int>(copy) == 42);
    assert(mystl::get<int>(v) == 7);
    std::cout << "copy independent: copy=" << mystl::get<int>(copy)
              << " orig=" << mystl::get<int>(v) << '\n';

    v = 3.5;
    assert(v.index() == 1);
    assert(mystl::holds_alternative<double>(v));
    assert(!mystl::holds_alternative<int>(v));
    assert(mystl::get<1>(v) == 3.5);
    assert(mystl::get<double>(v) == 3.5);
    assert(mystl::get_if<int>(&v) == nullptr);
    {
        double* pd2 = mystl::get_if<double>(&v);
        assert(pd2 != nullptr && *pd2 == 3.5);
    }
    std::cout << "after assigning double: index=" << v.index()
              << " value=" << mystl::get<double>(v) << '\n';

    std::string desc2 = mystl::visit(Describe{}, v);
    assert(desc2 == "double:3.500000");
    std::cout << "visit -> " << desc2 << '\n';

    v = mystl::monostate{};
    assert(v.index() == 2);
    assert(mystl::holds_alternative<mystl::monostate>(v));
    std::string desc3 = mystl::visit(Describe{}, v);
    assert(desc3 == "monostate");
    std::cout << "visit -> " << desc3 << '\n';

    bool threw = false;
    try {
        (void)mystl::get<int>(v);
    } catch (const mystl::bad_variant_access&) {
        threw = true;
    }
    assert(threw);
    std::cout << "bad get<int> threw bad_variant_access\n";

    assert(copy.index() == 0);
    assert(mystl::get<int>(copy) == 42);

    std::cout << "variant ok\n";
    return 0;
}
