#pragma once

#include <cstddef>
#include <cstdint>

#include "mystl/container/string.hpp"

namespace mystl {

inline std::size_t fnv1a(const void* data, std::size_t n) noexcept {
    const unsigned char* p = static_cast<const unsigned char*>(data);
#if SIZE_MAX > 0xffffffffu
    std::size_t hash = 14695981039346656037ull;
    const std::size_t prime = 1099511628211ull;
#else
    std::size_t hash = 2166136261u;
    const std::size_t prime = 16777619u;
#endif
    for (std::size_t i = 0; i < n; ++i) {
        hash ^= static_cast<std::size_t>(p[i]);
        hash *= prime;
    }
    return hash;
}

template <class T>
struct hash;

template <>
struct hash<bool> {
    std::size_t operator()(bool v) const noexcept {
        return static_cast<std::size_t>(v);
    }
};

template <>
struct hash<char> {
    std::size_t operator()(char v) const noexcept {
        return static_cast<std::size_t>(v);
    }
};

template <>
struct hash<signed char> {
    std::size_t operator()(signed char v) const noexcept {
        return static_cast<std::size_t>(v);
    }
};

template <>
struct hash<unsigned char> {
    std::size_t operator()(unsigned char v) const noexcept {
        return static_cast<std::size_t>(v);
    }
};

template <>
struct hash<wchar_t> {
    std::size_t operator()(wchar_t v) const noexcept {
        return static_cast<std::size_t>(v);
    }
};

template <>
struct hash<char16_t> {
    std::size_t operator()(char16_t v) const noexcept {
        return static_cast<std::size_t>(v);
    }
};

template <>
struct hash<char32_t> {
    std::size_t operator()(char32_t v) const noexcept {
        return static_cast<std::size_t>(v);
    }
};

template <>
struct hash<short> {
    std::size_t operator()(short v) const noexcept {
        return static_cast<std::size_t>(v);
    }
};

template <>
struct hash<unsigned short> {
    std::size_t operator()(unsigned short v) const noexcept {
        return static_cast<std::size_t>(v);
    }
};

template <>
struct hash<int> {
    std::size_t operator()(int v) const noexcept {
        return static_cast<std::size_t>(v);
    }
};

template <>
struct hash<unsigned int> {
    std::size_t operator()(unsigned int v) const noexcept {
        return static_cast<std::size_t>(v);
    }
};

template <>
struct hash<long> {
    std::size_t operator()(long v) const noexcept {
        return static_cast<std::size_t>(v);
    }
};

template <>
struct hash<unsigned long> {
    std::size_t operator()(unsigned long v) const noexcept {
        return static_cast<std::size_t>(v);
    }
};

template <>
struct hash<long long> {
    std::size_t operator()(long long v) const noexcept {
        return static_cast<std::size_t>(v);
    }
};

template <>
struct hash<unsigned long long> {
    std::size_t operator()(unsigned long long v) const noexcept {
        return static_cast<std::size_t>(v);
    }
};

template <>
struct hash<float> {
    std::size_t operator()(float v) const noexcept {

        if (v == 0.0f) {
            return 0;
        }
        return fnv1a(&v, sizeof(v));
    }
};

template <>
struct hash<double> {
    std::size_t operator()(double v) const noexcept {

        if (v == 0.0) {
            return 0;
        }
        return fnv1a(&v, sizeof(v));
    }
};

template <class T>
struct hash<T*> {
    std::size_t operator()(T* v) const noexcept {
        return static_cast<std::size_t>(reinterpret_cast<std::uintptr_t>(v));
    }
};

template <>
struct hash<mystl::string> {
    std::size_t operator()(const mystl::string& v) const noexcept {
        return fnv1a(v.data(), v.size());
    }
};

}
