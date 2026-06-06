#include <iostream>
#include <string>

#include "mystl/unique_ptr.hpp"

struct User {
    std::string name;

    User(std::string name)
        : name(mystl::move(name)) {
        std::cout << "ctor " << this->name << '\n';
    }

    ~User() {
        std::cout << "dtor " << name << '\n';
    }

    void hello() {
        std::cout << "hello " << name << '\n';
    }
};

int main() {
    auto p = mystl::make_unique<User>("Alice");

    p->hello();

    auto q = mystl::move(p);

    std::cout << bool(p) << '\n';
    std::cout << bool(q) << '\n';

    q.reset(new User("Bob"));

    User* raw = q.release();

    std::cout << q.get() << '\n';

    delete raw;
}