#pragma once

#include <cstddef>
#include <new>
#include <exception>

#include "mystl/utility/utility.hpp"
#include "mystl/type_traits/type_traits.hpp"

namespace mystl {

struct monostate {};

class bad_variant_access : public std::exception {
public:
    const char* what() const noexcept override {
        return "bad variant access";
    }
};

inline constexpr std::size_t variant_npos = static_cast<std::size_t>(-1);

template<bool B, typename T = void>
struct variant_enable_if {};

template<typename T>
struct variant_enable_if<true, T> {
    using type = T;
};

template<bool B, typename T = void>
using variant_enable_if_t = typename variant_enable_if<B, T>::type;

constexpr std::size_t variant_max(std::size_t a) {
    return a;
}

template<typename... Rest>
constexpr std::size_t variant_max(std::size_t a, std::size_t b, Rest... rest) {
    return variant_max(a > b ? a : b, rest...);
}

template<typename... Ts>
struct variant_max_size {
    static constexpr std::size_t value = variant_max(sizeof(Ts)...);
};

template<typename... Ts>
struct variant_max_align {
    static constexpr std::size_t value = variant_max(alignof(Ts)...);
};

template<std::size_t I, typename... Ts>
struct type_at;

template<typename T0, typename... Ts>
struct type_at<0, T0, Ts...> {
    using type = T0;
};

template<std::size_t I, typename T0, typename... Ts>
struct type_at<I, T0, Ts...> {
    using type = typename type_at<I - 1, Ts...>::type;
};

template<std::size_t I, typename... Ts>
using type_at_t = typename type_at<I, Ts...>::type;

template<typename T, typename... Ts>
struct index_of;

template<typename T, typename T0, typename... Ts>
struct index_of<T, T0, Ts...> {
    static constexpr std::size_t value =
        is_same_v<remove_cvref_t<T>, T0> ? 0 : 1 + index_of<T, Ts...>::value;
};

template<typename T>
struct index_of<T> {
    static constexpr std::size_t value = 0;
};

template<typename T, typename... Ts>
inline constexpr std::size_t index_of_v = index_of<T, Ts...>::value;

template<std::size_t I, std::size_t N>
struct variant_dispatcher {
    template<typename F>
    static void apply(std::size_t idx, F&& f) {
        if (idx == I) {
            mystl::forward<F>(f).template operator()<I>();
            return;
        }
        variant_dispatcher<I + 1, N>::apply(idx, mystl::forward<F>(f));
    }
};

template<std::size_t N>
struct variant_dispatcher<N, N> {
    template<typename F>
    static void apply(std::size_t, F&&) {}
};

template<typename... Ts>
class variant {
private:
    static constexpr std::size_t kSize = variant_max_size<Ts...>::value;
    static constexpr std::size_t kAlign = variant_max_align<Ts...>::value;

    alignas(kAlign) unsigned char storage_[kSize];
    std::size_t index_;

    template<std::size_t I>
    type_at_t<I, Ts...>* ptr() {
        return reinterpret_cast<type_at_t<I, Ts...>*>(storage_);
    }

    template<std::size_t I>
    const type_at_t<I, Ts...>* ptr() const {
        return reinterpret_cast<const type_at_t<I, Ts...>*>(storage_);
    }

    template<std::size_t I, typename F>
    static void dispatch(std::size_t idx, F&& f) {
        variant_dispatcher<I, sizeof...(Ts)>::apply(idx, mystl::forward<F>(f));
    }

    void destroy() {
        dispatch<0>(index_, [this]<std::size_t I>() {
            using T = type_at_t<I, Ts...>;
            ptr<I>()->~T();
        });
    }

    void copy_construct_from(const variant& other) {
        dispatch<0>(other.index_, [this, &other]<std::size_t I>() {
            using T = type_at_t<I, Ts...>;
            new (storage_) T(*other.template ptr<I>());
        });
        index_ = other.index_;
    }

    void move_construct_from(variant& other) {
        dispatch<0>(other.index_, [this, &other]<std::size_t I>() {
            using T = type_at_t<I, Ts...>;
            new (storage_) T(mystl::move(*other.template ptr<I>()));
        });
        index_ = other.index_;
    }

public:
    variant() {
        using T0 = type_at_t<0, Ts...>;
        new (storage_) T0();
        index_ = 0;
    }

    template<typename U,
             typename = variant_enable_if_t<!is_same_v<remove_cvref_t<U>, variant>>>
    variant(U&& u) {
        constexpr std::size_t I = index_of_v<remove_cvref_t<U>, Ts...>;
        using T = type_at_t<I, Ts...>;
        new (storage_) T(mystl::forward<U>(u));
        index_ = I;
    }

    variant(const variant& other) {
        copy_construct_from(other);
    }

    variant(variant&& other) {
        move_construct_from(other);
    }

    variant& operator=(const variant& other) {
        if (this == &other) return *this;
        destroy();
        copy_construct_from(other);
        return *this;
    }

    variant& operator=(variant&& other) {
        if (this == &other) return *this;
        destroy();
        move_construct_from(other);
        return *this;
    }

    ~variant() {
        destroy();
    }

    template<typename T, typename... Args>
    T& emplace(Args&&... args) {
        destroy();
        constexpr std::size_t I = index_of_v<T, Ts...>;
        new (storage_) T(mystl::forward<Args>(args)...);
        index_ = I;
        return *ptr<I>();
    }

    std::size_t index() const noexcept {
        return index_;
    }

    bool valueless_by_exception() const noexcept {
        return false;
    }

    template<std::size_t I>
    type_at_t<I, Ts...>* raw_ptr() {
        return ptr<I>();
    }

    template<std::size_t I>
    const type_at_t<I, Ts...>* raw_ptr() const {
        return ptr<I>();
    }
};

template<typename V>
struct variant_size;

template<typename... Ts>
struct variant_size<variant<Ts...>> {
    static constexpr std::size_t value = sizeof...(Ts);
};

template<typename V>
struct variant_size<const V> {
    static constexpr std::size_t value = variant_size<V>::value;
};

template<typename V>
inline constexpr std::size_t variant_size_v = variant_size<V>::value;

template<std::size_t I, typename V>
struct variant_alternative;

template<std::size_t I, typename... Ts>
struct variant_alternative<I, variant<Ts...>> {
    using type = type_at_t<I, Ts...>;
};

template<std::size_t I, typename V>
struct variant_alternative<I, const V> {
    using type = add_const_t<typename variant_alternative<I, V>::type>;
};

template<std::size_t I, typename V>
using variant_alternative_t = typename variant_alternative<I, V>::type;

template<typename T, typename... Ts>
bool holds_alternative(const variant<Ts...>& v) noexcept {
    return v.index() == index_of_v<T, Ts...>;
}

template<std::size_t I, typename... Ts>
type_at_t<I, Ts...>* get_if(variant<Ts...>* v) noexcept {
    if (v == nullptr || v->index() != I) return nullptr;
    return v->template raw_ptr<I>();
}

template<std::size_t I, typename... Ts>
const type_at_t<I, Ts...>* get_if(const variant<Ts...>* v) noexcept {
    if (v == nullptr || v->index() != I) return nullptr;
    return v->template raw_ptr<I>();
}

template<typename T, typename... Ts>
T* get_if(variant<Ts...>* v) noexcept {
    return get_if<index_of_v<T, Ts...>>(v);
}

template<typename T, typename... Ts>
const T* get_if(const variant<Ts...>* v) noexcept {
    return get_if<index_of_v<T, Ts...>>(v);
}

template<std::size_t I, typename... Ts>
type_at_t<I, Ts...>& get(variant<Ts...>& v) {
    if (v.index() != I) throw bad_variant_access();
    return *v.template raw_ptr<I>();
}

template<std::size_t I, typename... Ts>
const type_at_t<I, Ts...>& get(const variant<Ts...>& v) {
    if (v.index() != I) throw bad_variant_access();
    return *v.template raw_ptr<I>();
}

template<std::size_t I, typename... Ts>
type_at_t<I, Ts...>&& get(variant<Ts...>&& v) {
    if (v.index() != I) throw bad_variant_access();
    return mystl::move(*v.template raw_ptr<I>());
}

template<typename T, typename... Ts>
T& get(variant<Ts...>& v) {
    return get<index_of_v<T, Ts...>>(v);
}

template<typename T, typename... Ts>
const T& get(const variant<Ts...>& v) {
    return get<index_of_v<T, Ts...>>(v);
}

template<typename T, typename... Ts>
T&& get(variant<Ts...>&& v) {
    return get<index_of_v<T, Ts...>>(mystl::move(v));
}

template<std::size_t I, std::size_t N>
struct variant_visit_impl {
    template<typename Visitor, typename Variant, typename R>
    static R apply(Visitor&& vis, Variant&& var) {
        if (var.index() == I) {
            return mystl::forward<Visitor>(vis)(get<I>(mystl::forward<Variant>(var)));
        }
        return variant_visit_impl<I + 1, N>::template apply<Visitor, Variant, R>(
            mystl::forward<Visitor>(vis), mystl::forward<Variant>(var));
    }
};

template<std::size_t N>
struct variant_visit_impl<N, N> {
    template<typename Visitor, typename Variant, typename R>
    static R apply(Visitor&&, Variant&&) {
        throw bad_variant_access();
    }
};

template<typename Visitor, typename... Ts>
decltype(auto) visit(Visitor&& vis, variant<Ts...>& var) {
    using R = decltype(mystl::forward<Visitor>(vis)(get<0>(var)));
    return variant_visit_impl<0, sizeof...(Ts)>::template apply<Visitor, variant<Ts...>&, R>(
        mystl::forward<Visitor>(vis), var);
}

template<typename Visitor, typename... Ts>
decltype(auto) visit(Visitor&& vis, const variant<Ts...>& var) {
    using R = decltype(mystl::forward<Visitor>(vis)(get<0>(var)));
    return variant_visit_impl<0, sizeof...(Ts)>::template apply<Visitor, const variant<Ts...>&, R>(
        mystl::forward<Visitor>(vis), var);
}

template<typename Visitor, typename... Ts>
decltype(auto) visit(Visitor&& vis, variant<Ts...>&& var) {
    using R = decltype(mystl::forward<Visitor>(vis)(get<0>(mystl::move(var))));
    return variant_visit_impl<0, sizeof...(Ts)>::template apply<Visitor, variant<Ts...>&&, R>(
        mystl::forward<Visitor>(vis), mystl::move(var));
}

}
