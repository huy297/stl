#include <iostream>
#include <cassert>

#include "mystl/algorithm/algorithm.hpp"
#include "mystl/container/vector.hpp"

int main() {

    mystl::vector<int> v{5, 3, 8, 1, 9, 2, 7, 4, 6, 0};
    mystl::sort(v.begin(), v.end());
    for (std::size_t i = 0; i < v.size(); ++i) {
        assert(v[i] == static_cast<int>(i));
    }
    std::cout << "sorted:";
    for (std::size_t i = 0; i < v.size(); ++i) {
        std::cout << ' ' << v[i];
    }
    std::cout << '\n';

    assert(mystl::min(3, 7) == 3);
    assert(mystl::max(3, 7) == 7);
    assert(mystl::min(7, 3) == 3);
    assert(mystl::max(7, 3) == 7);
    std::cout << "min(3,7)=" << mystl::min(3, 7)
              << " max(3,7)=" << mystl::max(3, 7) << '\n';

    auto minIt = mystl::min_element(v.begin(), v.end());
    auto maxIt = mystl::max_element(v.begin(), v.end());
    assert(minIt != v.end() && *minIt == 0);
    assert(maxIt != v.end() && *maxIt == 9);
    std::cout << "min_element=" << *minIt << " max_element=" << *maxIt << '\n';

    auto found = mystl::find(v.begin(), v.end(), 6);
    assert(found != v.end() && *found == 6);
    auto missing = mystl::find(v.begin(), v.end(), 42);
    assert(missing == v.end());
    auto firstEven = mystl::find_if(v.begin(), v.end(), [](int x) { return x % 2 == 0; });
    assert(firstEven != v.end() && *firstEven == 0);
    std::cout << "find(6)=" << *found
              << " first_even=" << *firstEven << '\n';

    auto cnt3 = mystl::count(v.begin(), v.end(), 3);
    assert(cnt3 == 1);
    auto evenCount = mystl::count_if(v.begin(), v.end(), [](int x) { return x % 2 == 0; });
    assert(evenCount == 5);
    std::cout << "count(3)=" << cnt3 << " even_count=" << evenCount << '\n';

    bool allNonNeg = mystl::all_of(v.begin(), v.end(), [](int x) { return x >= 0; });
    bool anyGt8 = mystl::any_of(v.begin(), v.end(), [](int x) { return x > 8; });
    bool noneNeg = mystl::none_of(v.begin(), v.end(), [](int x) { return x < 0; });
    assert(allNonNeg);
    assert(anyGt8);
    assert(noneNeg);
    std::cout << "all_of(>=0)=" << allNonNeg
              << " any_of(>8)=" << anyGt8
              << " none_of(<0)=" << noneNeg << '\n';

    long sum = 0;
    mystl::for_each(v.begin(), v.end(), [&sum](int x) { sum += x; });
    assert(sum == 45);
    std::cout << "for_each sum=" << sum << '\n';

    mystl::reverse(v.begin(), v.end());
    for (std::size_t i = 0; i < v.size(); ++i) {
        assert(v[i] == static_cast<int>(v.size() - 1 - i));
    }
    std::cout << "reversed:";
    for (std::size_t i = 0; i < v.size(); ++i) {
        std::cout << ' ' << v[i];
    }
    std::cout << '\n';

    auto newEnd = mystl::remove_if(v.begin(), v.end(), [](int x) { return x % 2 == 0; });
    v.erase(newEnd, v.end());
    assert(v.size() == 5);
    for (std::size_t i = 0; i < v.size(); ++i) {
        assert(v[i] % 2 != 0);
    }

    int expectedOdds[] = {9, 7, 5, 3, 1};
    for (std::size_t i = 0; i < v.size(); ++i) {
        assert(v[i] == expectedOdds[i]);
    }
    std::cout << "after remove_if(even)+erase:";
    for (std::size_t i = 0; i < v.size(); ++i) {
        std::cout << ' ' << v[i];
    }
    std::cout << '\n';

    std::cout << "algorithm ok" << '\n';
    return 0;
}
