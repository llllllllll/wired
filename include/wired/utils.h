#pragma once

#include <array>
#include <cstdint>
#include <utility>

namespace wired {
namespace utils {
namespace dispatch {
template<template<typename A, typename B> typename Op, typename... Vs>
struct reduce {};

// base case
template<template<typename A, typename B> typename Op, typename V>
struct reduce<Op, V> {
    typedef V type;
};

template<template<typename A, typename B> typename Op,
         typename V, typename... Vs>
struct reduce<Op, V, Vs...> {
    typedef Op<V, typename reduce<Op, Vs...>::type> type;
};

template<typename A, typename B>
struct bin_cat_index_sequence {};

template<std::size_t... ix, std::size_t... jx>
struct bin_cat_index_sequence<std::index_sequence<ix...>,
                          std::index_sequence<jx...>> {
    typedef std::index_sequence<ix..., jx...> type;
};
}

template<template<typename A, typename B> typename Op, typename... Vs>
using reduce = typename dispatch::reduce<Op, Vs...>::type;

namespace {
template<typename A, typename B>
using bin_cat_index_sequence =
    typename dispatch::bin_cat_index_sequence<A, B>::type;
}

template<typename... Ixs>
using cat_index_sequences = reduce<bin_cat_index_sequence, Ixs...>;

template<std::size_t... ixs>
std::array<std::size_t, sizeof...(ixs)>
index_sequence_to_array(std::index_sequence<ixs...>) {
    return {ixs...};
}

namespace dispatch {
template<typename A, typename B>
struct index_sequence_eq {};

template<std::size_t... ix, std::size_t... jx>
struct index_sequence_eq<std::index_sequence<ix...>,
                         std::index_sequence<jx...>> {
    constexpr static bool value = (true && ... && (ix == jx));
};
}

template<typename A, typename B>
constexpr bool index_sequence_eq = dispatch::index_sequence_eq<A, B>::value;
}
}
