#pragma once

#include <array>
#include <ratio>
#include <tuple>
#include <type_traits>
#include <utility>

#include "wired/scalar.h"

namespace wired {
template<typename... Vs>
struct array {
    static std::array<double, sizeof...(Vs)> as_doubles() {
        return {Vs::as_double()...};
    }
};

namespace dispatch {
template<typename T, std::size_t n>
struct getitem {};

template<std::size_t n, typename... Vs>
struct getitem<array<Vs...>, n> {
    // this will generate an error already but the message is horrific
    static_assert(n < sizeof...(Vs), "getitem index out of bounds");
    typedef std::remove_reference_t<
        decltype(std::get<n>(std::make_tuple(std::declval<Vs>()...)))> type;
};

template<typename... Vs, typename U>
struct add<array<Vs...>, U> {
    typedef array<wired::add<Vs, U>...> type;
};

template<typename U, typename... Vs>
struct add<U, array<Vs...>> {
    typedef array<wired::add<U, Vs>...> type;
};

template<typename... Vs, typename ...Us>
struct add<array<Vs...>, array<Us...>> {
    typedef array<wired::add<Vs, Us>...> type;
};

template<typename... Vs, typename U>
struct sub<array<Vs...>, U> {
    typedef array<wired::sub<Vs, U>...> type;
};

template<typename U, typename... Vs>
struct sub<U, array<Vs...>> {
    typedef array<wired::sub<U, Vs>...> type;
};

template<typename... Vs, typename ...Us>
struct sub<array<Vs...>, array<Us...>> {
    typedef array<wired::sub<Vs, Us>...> type;
};

template<typename... Vs, typename U>
struct mul<array<Vs...>, U> {
    typedef array<wired::mul<Vs, U>...> type;
};

template<typename U, typename... Vs>
struct mul<U, array<Vs...>> {
    typedef array<wired::mul<U, Vs>...> type;
};

template<typename... Vs, typename ...Us>
struct mul<array<Vs...>, array<Us...>> {
    typedef array<wired::mul<Vs, Us>...> type;
};

template<typename... Vs, typename U>
struct div<array<Vs...>, U> {
    typedef array<wired::div<Vs, U>...> type;
};

template<typename U, typename... Vs>
struct div<U, array<Vs...>> {
    typedef array<wired::div<U, Vs>...> type;
};

template<typename... Vs, typename ...Us>
struct div<array<Vs...>, array<Us...>> {
    typedef array<wired::div<Vs, Us>...> type;
};

template<typename... Vs>
struct exp<array<Vs...>> {
    typedef array<wired::exp<Vs>...> type;
};

template<template<typename A, typename B> typename Op, typename Vs>
struct reduce {};

// base case
template<template<typename A, typename B> typename Op, typename V>
struct reduce<Op, array<V>> {
    typedef V type;
};

template<template<typename A, typename B> typename Op,
         typename V, typename... Vs>
struct reduce<Op, array<V, Vs...>> {
private:
    typedef typename reduce<Op, array<Vs...>>::type recursive;
public:
    typedef Op<V, recursive> type;
};
}

template<typename T, std::size_t n>
using getitem = typename dispatch::getitem<T, n>::type;

template<template<typename A, typename B> typename Op, typename T>
using reduce = typename dispatch::reduce<Op, T>::type;

template<typename T>
using sum = reduce<add, T>;

template<typename T>
using product = reduce<mul, T>;
}
