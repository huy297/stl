# mystl

A reimplementation of parts of the C++ Standard Library, written to understand STL library.

## What's implemented

- Containers: array, vector, list, forward_list, deque, string, map, set, unordered_map, unordered_set, stack, queue, priority_queue
- Memory: allocator, unique_ptr
- Utilities: pair, optional, variant, functional, hash, type_traits, iterator, algorithm

## Layout

```
include/mystl/
├── type_traits/
├── utility/       utility, pair, optional, variant, functional, hash
├── iterator/
├── memory/        allocator, memory, unique_ptr
├── algorithm/
├── container/     array, vector, list, forward_list, deque, string,
│                  map, set, unordered_map, unordered_set,
│                  stack, queue, priority_queue
├── detail/        rb_tree, hash_table
└── mystl.hpp      umbrella header

tests/             mirrors include/mystl/
```

## Build

Header-only, so just add `include/` to the include path:

```sh
g++ -std=c++17 -Iinclude your_file.cpp
```

```cpp
#include "mystl/container/vector.hpp"

mystl::vector<int> v{1, 2, 3};
v.push_back(4);
```

## Tests

```sh
make test     # build and run all tests
make clean
```
