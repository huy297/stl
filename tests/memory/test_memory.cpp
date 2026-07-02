#include <iostream>
#include <cassert>
#include <string>
#include <new>

#include "mystl/memory/memory.hpp"

struct Item {
    std::string name;
    int id;

    Item()
        : name("default"), id(0) {}

    Item(std::string n, int i)
        : name(mystl::move(n)), id(i) {}
};

int main() {
    constexpr int N = 4;

    {
        void* mem = ::operator new(sizeof(Item));
        Item* p = static_cast<Item*>(mem);

        Item* built = mystl::construct_at(p, std::string("solo"), 42);
        assert(built == p);
        assert(p->name == "solo");
        assert(p->id == 42);

        mystl::destroy_at(p);
        ::operator delete(mem);
    }

    {
        Item src[N] = {
            Item("alpha", 1),
            Item("beta", 2),
            Item("gamma", 3),
            Item("delta", 4),
        };

        void* mem = ::operator new(sizeof(Item) * N);
        Item* buf = static_cast<Item*>(mem);

        Item* end = mystl::uninitialized_copy(src, src + N, buf);
        assert(end == buf + N);

        for (int i = 0; i < N; ++i) {
            assert(buf[i].name == src[i].name);
            assert(buf[i].id == src[i].id);
        }
        std::cout << "uninitialized_copy: "
                  << buf[0].name << " " << buf[1].name << " "
                  << buf[2].name << " " << buf[3].name << '\n';

        assert(src[0].name == "alpha");
        assert(src[3].id == 4);

        mystl::destroy(buf, buf + N);
        ::operator delete(mem);
    }

    {
        Item fillValue("filled", 7);

        void* mem = ::operator new(sizeof(Item) * N);
        Item* buf = static_cast<Item*>(mem);

        mystl::uninitialized_fill(buf, buf + N, fillValue);

        for (int i = 0; i < N; ++i) {
            assert(buf[i].name == "filled");
            assert(buf[i].id == 7);
        }
        std::cout << "uninitialized_fill: " << buf[0].name
                  << " x" << N << '\n';

        assert(fillValue.name == "filled");
        assert(fillValue.id == 7);

        mystl::destroy(buf, buf + N);
        ::operator delete(mem);
    }

    {
        Item x("addr", 9);
        Item* a = mystl::addressof(x);
        assert(a == &x);
        assert(a->id == 9);
    }

    std::cout << "memory ok\n";
    return 0;
}
