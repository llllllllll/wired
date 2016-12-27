#pragma once

#include <cstdint>
#include <type_traits>
#include <ratio>

#include "wired/op.h"

namespace wired {
constexpr std::uint8_t default_fbits = 16;

template<std::int32_t data, std::uint8_t fbits = default_fbits>
struct fixed {
    static_assert(fbits <= 31, "fbits is too large");

    constexpr static std::int32_t rawdata = data;

    constexpr static double materialize() {
        return static_cast<double>(data) / (1 << fbits);
    }
};

template<std::int64_t value, std::uint8_t fbits = default_fbits>
using from_integral = fixed<value << fbits, fbits>;

namespace dispatch {
template<typename T, std::uint8_t fbits>
struct from_ratio {};

template<std::intmax_t num, std::intmax_t den, std::uint8_t fbits>
struct from_ratio<std::ratio<num, den>, fbits> {
    typedef fixed<op::div(from_integral<num, fbits>::rawdata,
                          from_integral<den, fbits>::rawdata,
                          fbits), fbits> type;
};
}

template<typename T, std::uint8_t fbits = default_fbits>
using from_ratio = typename dispatch::from_ratio<T, fbits>::type;

namespace dispatch {
template<std::int32_t op(std::int32_t, std::uint8_t), typename value>
struct unop {};

template<std::int32_t op(std::int32_t, std::uint8_t),
         std::int32_t value,
         std::uint8_t fbits>
struct unop<op, fixed<value, fbits>> {
    typedef fixed<op(value, fbits), fbits> type;
};

template<std::int32_t op(std::int32_t, std::int32_t, std::uint8_t),
         typename lhs,
         typename rhs>
struct binop {};

template<std::int32_t op(std::int32_t, std::int32_t, std::uint8_t),
         std::int32_t lhs,
         std::int32_t rhs,
         std::uint8_t fbits>
struct binop<op, fixed<lhs, fbits>, fixed<rhs, fbits>> {
    typedef fixed<op(lhs, rhs, fbits), fbits> type;
};
}

template<typename T, typename U>
using add = typename dispatch::binop<op::add, T, U>::type;

template<typename T, typename U>
using sub = typename dispatch::binop<op::sub, T, U>::type;

template<typename T, typename U>
using mul = typename dispatch::binop<op::mul, T, U>::type;

template<typename T, typename U>
using div = typename dispatch::binop<op::div, T, U>::type;

template<typename T>
using exp = typename dispatch::unop<op::exp, T>::type;

template<typename T>
using neg = typename dispatch::unop<op::neg, T>::type;

template<typename T>
using inv = typename dispatch::unop<op::inv, T>::type;
}
