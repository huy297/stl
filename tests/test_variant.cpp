#include <iostream>
#include <cassert>
#include <string>

#include "mystl/variant.hpp"

// Visitor that handles each alternative of variant<int, double, monostate>.
// visit() deduces the return type from the first alternative (int), so every
// overload must return the same type. We return std::string for all of them.
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

    // ---- construct holding the int alternative (index 0) ----
    V v(42);
    assert(v.index() == 0);
    assert(mystl::holds_alternative<int>(v));
    assert(!mystl::holds_alternative<double>(v));
    assert(!mystl::holds_alternative<mystl::monostate>(v));
    std::cout << "index after int construct: " << v.index() << '\n';

    // ---- get<I> and get<T> ----
    assert(mystl::get<0>(v) == 42);
    assert(mystl::get<int>(v) == 42);
    std::cout << "get<int>(v) = " << mystl::get<int>(v) << '\n';

    // ---- get_if (matching and non-matching) ----
    int* pi = mystl::get_if<int>(&v);
    assert(pi != nullptr);
    assert(*pi == 42);
    double* pd = mystl::get_if<double>(&v);
    assert(pd == nullptr);
    std::cout << "get_if<int> ok, get_if<double> null\n";

    // ---- visit on the int alternative ----
    std::string desc = mystl::visit(Describe{}, v);
    assert(desc == "int:42");
    std::cout << "visit -> " << desc << '\n';

    // ---- copy construction ----
    V copy = v;
    assert(copy.index() == 0);
    assert(mystl::holds_alternative<int>(copy));
    assert(mystl::get<int>(copy) == 42);
    // mutate original; copy must be independent
    mystl::get<int>(v) = 7;
    assert(mystl::get<int>(copy) == 42);
    assert(mystl::get<int>(v) == 7);
    std::cout << "copy independent: copy=" << mystl::get<int>(copy)
              << " orig=" << mystl::get<int>(v) << '\n';

    // ---- assign a different alternative (double, index 1) ----
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

    // visit the double alternative
    std::string desc2 = mystl::visit(Describe{}, v);
    assert(desc2 == "double:3.500000");
    std::cout << "visit -> " << desc2 << '\n';

    // ---- assign the monostate alternative (index 2) ----
    v = mystl::monostate{};
    assert(v.index() == 2);
    assert(mystl::holds_alternative<mystl::monostate>(v));
    std::string desc3 = mystl::visit(Describe{}, v);
    assert(desc3 == "monostate");
    std::cout << "visit -> " << desc3 << '\n';

    // ---- bad access via get throws ----
    bool threw = false;
    try {
        (void)mystl::get<int>(v);  // v currently holds monostate
    } catch (const mystl::bad_variant_access&) {
        threw = true;
    }
    assert(threw);
    std::cout << "bad get<int> threw bad_variant_access\n";

    // copy is still independent of all the reassignments above
    assert(copy.index() == 0);
    assert(mystl::get<int>(copy) == 42);

    std::cout << "variant ok\n";
    return 0;
}
