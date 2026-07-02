#pragma once

namespace mystl {

template <typename T, T v>
struct integral_constant {
    static constexpr T value = v;

    using value_type = T;
    using type = integral_constant;

    constexpr operator value_type() const noexcept {
        return value;
    }

    constexpr value_type operator()() const noexcept {
        return value;
    }
};

using true_type = integral_constant<bool, true>;
using false_type = integral_constant<bool, false>;

template <typename A, typename B>
struct is_same : false_type {};

template <typename A>
struct is_same<A, A> : true_type {};

template <typename A, typename B>
inline constexpr bool is_same_v = is_same<A, B>::value;

template <typename T>
struct is_lvalue_reference : false_type {};

template <typename T>
struct is_lvalue_reference<T&> : true_type {};

template <typename T>
inline constexpr bool is_lvalue_reference_v = is_lvalue_reference<T>::value;

template <typename T>
struct remove_reference {
    using type = T;
};

template <typename T>
struct remove_reference<T&> {
    using type = T;
};

template <typename T>
struct remove_reference<T&&> {
    using type = T;
};

template <typename T>
using remove_reference_t = typename remove_reference<T>::type;

template <typename T>
struct remove_const {
    using type = T;
};

template <typename T>
struct remove_const<const T> {
    using type = T;
};

template <typename T>
using remove_const_t = typename remove_const<T>::type;

template <typename T>
struct remove_volatile {
    using type = T;
};

template <typename T>
struct remove_volatile<volatile T> {
    using type = T;
};

template <typename T>
using remove_volatile_t = typename remove_volatile<T>::type;

template <typename T>
struct remove_cv {
    using type = remove_volatile_t<remove_const_t<T>>;
};

template <typename T>
using remove_cv_t = typename remove_cv<T>::type;

template <typename T>
struct remove_cvref {
    using type = remove_cv_t<remove_reference_t<T>>;
};

template <typename T>
using remove_cvref_t = typename remove_cvref<T>::type;

template<bool B, typename T, typename F>
struct conditional;

template<typename T, typename F>
struct conditional<true, T, F> {
    using type = T;
};

template<typename T, typename F>
struct conditional<false, T, F> {
    using type = F;
};

template<bool B, typename T, typename F>
using conditional_t = typename conditional<B, T, F>::type;

template <bool B, typename T = void>
struct enable_if {
    using type = void;
};

template <typename T>
struct enable_if<true, T> {
    using type = T;
};

template <bool B, typename T = void>
using enable_if_t = typename enable_if<B, T>::type;

template <typename...>
using void_t = void;

template <typename T>
struct is_void : false_type {};

template <>
struct is_void<void> : true_type {};

template <>
struct is_void<const void> : true_type {};

template <>
struct is_void<volatile void> : true_type {};

template <>
struct is_void<const volatile void> : true_type {};

template <typename T>
inline constexpr bool is_void_v = is_void<T>::value;

template <typename T>
struct is_pointer : false_type {};

template <typename T>
struct is_pointer<T*> : true_type {};

template <typename T>
struct is_pointer<T* const> : true_type {};

template <typename T>
struct is_pointer<T* volatile> : true_type {};

template <typename T>
struct is_pointer<T* const volatile> : true_type {};

template <typename T>
inline constexpr bool is_pointer_v = is_pointer<T>::value;

template <typename T>
struct is_reference : false_type {};

template <typename T>
struct is_reference<T&> : true_type {};

template <typename T>
struct is_reference<T&&> : true_type {};

template <typename T>
inline constexpr bool is_reference_v = is_reference<T>::value;

template <typename T>
struct is_rvalue_reference : false_type {};

template <typename T>
struct is_rvalue_reference<T&&> : true_type {};

template <typename T>
inline constexpr bool is_rvalue_reference_v = is_rvalue_reference<T>::value;

template <typename T>
struct is_const : false_type {};

template <typename T>
struct is_const<const T> : true_type {};

template <typename T>
inline constexpr bool is_const_v = is_const<T>::value;

template <typename T>
struct add_const {
    using type = const T;
};

template <typename T>
using add_const_t = typename add_const<T>::type;

template <typename T>
struct add_pointer {
    using type = remove_reference_t<T>*;
};

template <typename T>
using add_pointer_t = typename add_pointer<T>::type;

template <typename T>
struct add_lvalue_reference {
    using type = T&;
};

template <>
struct add_lvalue_reference<void> {
    using type = void;
};

template <>
struct add_lvalue_reference<const void> {
    using type = const void;
};

template <>
struct add_lvalue_reference<volatile void> {
    using type = volatile void;
};

template <>
struct add_lvalue_reference<const volatile void> {
    using type = const volatile void;
};

template <typename T>
using add_lvalue_reference_t = typename add_lvalue_reference<T>::type;

template <typename T>
struct add_rvalue_reference {
    using type = T&&;
};

template <>
struct add_rvalue_reference<void> {
    using type = void;
};

template <>
struct add_rvalue_reference<const void> {
    using type = const void;
};

template <>
struct add_rvalue_reference<volatile void> {
    using type = volatile void;
};

template <>
struct add_rvalue_reference<const volatile void> {
    using type = const volatile void;
};

template <typename T>
using add_rvalue_reference_t = typename add_rvalue_reference<T>::type;

template <typename T>
add_rvalue_reference_t<T> declval() noexcept;

template <typename T>
struct is_array : false_type {};

template <typename T>
struct is_array<T[]> : true_type {};

template <typename T, decltype(sizeof(0)) N>
struct is_array<T[N]> : true_type {};

template <typename T>
inline constexpr bool is_array_v = is_array<T>::value;

template <typename T>
struct type_identity {
    using type = T;
};

template <typename T>
using type_identity_t = typename type_identity<T>::type;

}
