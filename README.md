# mystl

A from-scratch reimplementation of parts of the C++ Standard Library, written to
understand how the STL actually works under the hood. Header-only, C++17, no
dependency on the real `<vector>`, `<algorithm>`, `<memory>`, etc. — everything
lives in `namespace mystl`.

## What's implemented

**Utilities & traits**
- `type_traits` — `integral_constant`, `is_same`, `remove_*` / `add_*`,
  `conditional`, `enable_if`, `void_t`, `is_pointer/reference/const/void/array`,
  `declval`, `type_identity`
- `utility` — `move`, `forward`, `swap`, `exchange`
- `functional` — comparison, arithmetic, logical and bitwise function objects
- `iterator` — iterator category tags, `iterator_traits`, `advance` / `distance`
  / `next` / `prev`, `reverse_iterator`, `back_insert_iterator`, `begin`/`end`

**Memory**
- `memory` — `construct_at` / `destroy_at`, `addressof`, `destroy(_n)`,
  `uninitialized_copy` / `_move` / `_fill` / `_value_construct`
- `allocator` — `allocator<T>`
- `unique_ptr` — `unique_ptr<T>`, `make_unique`

**Containers**
- `array` — `array<T, N>`
- `vector` — `vector<T>` (growth, `insert`/`erase`/`assign`, exception safety)
- `string` — `string` with small-string optimization (SSO)

**Vocabulary types**
- `pair` — `pair`, `make_pair`, `get`
- `optional` — `optional<T>`, `nullopt`, `value_or`
- `variant` — `variant<Ts...>`, `get` / `get_if` / `holds_alternative` / `visit`,
  `monostate`

**Algorithms**
- `algorithm` — `sort`, `min`/`max`, `*_element`, `find*`, `count*`, `*_of`,
  `for_each`, `copy`/`move`/`fill`, `reverse`, `remove*`, `equal`,
  `lexicographical_compare`, `swap_ranges`

`mystl/mystl.hpp` is an umbrella header that pulls in everything.

## Layout

```
include/mystl/   the library (header-only)
tests/           one test program per component
Makefile         build & run the tests
```

## Build & test

```sh
make test     # build every test in tests/ and run them
make          # just build into build/
make clean
```

Or use a single header directly:

```cpp
#include "mystl/vector.hpp"

mystl::vector<int> v{1, 2, 3};
v.push_back(4);
```

Compile with `g++ -std=c++17 -Iinclude`.

## Note

This is a learning project, not a drop-in replacement for the standard library —
the goal is clarity over completeness.
