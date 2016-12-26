#pragma once

#include <array>
#include <ratio>
#include <tuple>
#include <type_traits>
#include <utility>

#include "wired/scalar.h"
#include "wired/utils.h"

namespace wired {
template<typename... Vs>
struct array;

namespace dispatch {
template<typename A>
struct shape {
    typedef std::index_sequence<> type;
};

template<typename V>
struct shape<array<V>> {
    typedef utils::cat_index_sequences<std::index_sequence<1>,
                                       typename shape<V>::type> type;
};

template<typename V, typename... Vs>
struct shape<array<V, Vs...>> {
    typedef utils::cat_index_sequences<std::index_sequence<sizeof...(Vs) + 1>,
                                       typename shape<V>::type> type;
};
}

namespace utils {
namespace dispatch {
template<typename A>
struct shape_to_array_type {};

template<>
struct shape_to_array_type<std::index_sequence<>> {
    typedef std::array<double, 0> type;
};

template<std::size_t ix>
struct shape_to_array_type<std::index_sequence<ix>> {
    typedef std::array<double, ix> type;
};

template<std::size_t ix, std::size_t... ixs>
struct shape_to_array_type<std::index_sequence<ix, ixs...>> {
    typedef std::array<
        typename shape_to_array_type<std::index_sequence<ixs...>>::type,
        ix> type;
};
}

template<typename I>
using shape_to_array_type = typename dispatch::shape_to_array_type<I>::type;
}

template<typename V>
using shape = typename dispatch::shape<V>::type;

template<typename... Vs>
struct array {
    constexpr static utils::shape_to_array_type<shape<array<Vs...>>>
    materialize() {
        return {Vs::materialize()...};
    }
};

template<typename V, typename... Vs>
struct array<V, Vs...>  {
    constexpr static std::enable_if_t<
        (true && ... && utils::index_sequence_eq<shape<V>, shape<Vs>>),
        utils::shape_to_array_type<shape<array<V, Vs...>>>>
        materialize() {
        return {V::materialize(), Vs::materialize()...};
    }
};

namespace dispatch {
template<typename T, std::size_t... ns>
struct getitem {};

template<std::size_t n, typename... Vs>
struct getitem<array<Vs...>, n> {
    // this will generate an error already but the message is horrific
    static_assert(n < sizeof...(Vs), "getitem index out of bounds");
    typedef std::remove_reference_t<
        decltype(std::get<n>(std::make_tuple(std::declval<Vs>()...)))> type;
};

template<std::size_t n, std::size_t... ns, typename... Vs>
struct getitem<array<Vs...>, n, ns...> {
    // this will generate an error already but the message is horrific
    static_assert(n < sizeof...(Vs), "getitem index out of bounds");
    typedef typename getitem<typename getitem<array<Vs...>, n>::type,
                             ns...>::type type;
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
    typedef Op<V, typename reduce<Op, array<Vs...>>::type> type;
};
}

template<typename T, std::size_t... ns>
using getitem = typename dispatch::getitem<T, ns...>::type;

template<template<typename A, typename B> typename Op, typename T>
using reduce = typename dispatch::reduce<Op, T>::type;

template<typename T>
using sum = reduce<add, T>;

template<typename T>
using product = reduce<mul, T>;
}
