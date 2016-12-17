#pragma once

#include <array>
#include <ratio>
#include <tuple>
#include <type_traits>

#include "wired/fixed.h"

namespace wired {
template<typename... Vs>
struct array {
    static std::array<double, sizeof...(Vs)> as_doubles() {
        return {Vs::as_double()...};
    }
};

template<typename T, std::size_t n>
struct _getitem {};

template<std::size_t n, typename... Vs>
struct _getitem<array<Vs...>, n> {
    // this will generate an error already but the message is horrific
    static_assert(n < sizeof...(Vs), "getitem index out of bounds");
    typedef std::remove_reference_t<
        decltype(std::get<n>(std::make_tuple(Vs{}...)))> type;
};

template<typename T, std::size_t n>
using getitem = typename _getitem<T, n>::type;

template<typename... Vs, typename U>
struct _add<array<Vs...>, U> {
    typedef array<add<Vs, U>...> type;
};

template<typename U, typename... Vs>
struct _add<U, array<Vs...>> {
    typedef array<add<U, Vs>...> type;
};

template<typename... Vs, typename ...Us>
struct _add<array<Vs...>, array<Us...>> {
    typedef array<add<Vs, Us>...> type;
};

template<typename... Vs, typename U>
struct _sub<array<Vs...>, U> {
    typedef array<sub<Vs, U>...> type;
};

template<typename U, typename... Vs>
struct _sub<U, array<Vs...>> {
    typedef array<sub<U, Vs>...> type;
};

template<typename... Vs, typename ...Us>
struct _sub<array<Vs...>, array<Us...>> {
    typedef array<sub<Vs, Us>...> type;
};

template<typename... Vs, typename U>
struct _mul<array<Vs...>, U> {
    typedef array<mul<Vs, U>...> type;
};

template<typename U, typename... Vs>
struct _mul<U, array<Vs...>> {
    typedef array<mul<U, Vs>...> type;
};

template<typename... Vs, typename ...Us>
struct _mul<array<Vs...>, array<Us...>> {
    typedef array<mul<Vs, Us>...> type;
};

template<typename... Vs, typename U>
struct _div<array<Vs...>, U> {
    typedef array<div<Vs, U>...> type;
};

template<typename U, typename... Vs>
struct _div<U, array<Vs...>> {
    typedef array<div<U, Vs>...> type;
};

template<typename... Vs, typename ...Us>
struct _div<array<Vs...>, array<Us...>> {
    typedef array<div<Vs, Us>...> type;
};

template<typename... Vs>
struct _exp<array<Vs...>> {
    typedef array<exp<Vs>...> type;
};
}
