#pragma once

#include <cstdint>
#include <type_traits>
#include <ratio>

#include "wired/utils.h"

#include <iostream>

namespace wired {
template<std::uint8_t fbits, std::int32_t data>
struct fixed {
    static_assert(fbits <= 31, "fbits is too large");

    constexpr static std::int32_t rawdata = data;

    static double as_double() {
        return static_cast<double>(data) / (1 << fbits);
    }
};

template<std::uint8_t fbits, std::int64_t value>
using from_integral = fixed<fbits, value << fbits>;

template<typename T, typename U>
struct _add {};

template<std::uint8_t fbits, std::int32_t lhs, std::int32_t rhs>
struct _add<fixed<fbits, lhs>, fixed<fbits, rhs>> {
    typedef fixed<fbits, lhs + rhs> type;
};

template<typename T, typename U>
using add = typename _add<T, U>::type;

template<typename T, typename U>
struct _sub {};

template<std::uint8_t fbits, std::int32_t lhs, std::int32_t rhs>
struct _sub<fixed<fbits, lhs>, fixed<fbits, rhs>> {
    typedef fixed<fbits, lhs + rhs> type;
};

template<typename T, typename U>
using sub = typename _sub<T, U>::type;

template<typename T, typename U>
struct _mul {};

template<std::uint8_t fbits>
constexpr std::int32_t _mul_f(std::int32_t lhs, std::int32_t rhs) {
    return (static_cast<std::int64_t>(lhs) * rhs) >> fbits;
}

template<std::uint8_t fbits, std::int32_t lhs, std::int32_t rhs>
struct _mul<fixed<fbits, lhs>, fixed<fbits, rhs>> {
    typedef fixed<fbits, _mul_f<fbits>(lhs, rhs)> type;
};

template<typename T, typename U>
using mul = typename _mul<T, U>::type;

template<typename T, typename U>
struct _div {};

template<std::uint8_t fbits>
constexpr std::int32_t _div_f(std::int32_t lhs, std::int32_t rhs) {
    return (static_cast<std::int64_t>(lhs) << fbits) / rhs;
}

template<std::uint8_t fbits, std::int32_t lhs, std::int32_t rhs>
struct _div<fixed<fbits, lhs>, fixed<fbits, rhs>> {
    typedef fixed<fbits, _div_f<fbits>(lhs, rhs)> type;
};

template<typename T, typename U>
using div = typename _div<T, U>::type;

template<std::uint8_t fbits, typename T>
struct _from_ratio {};

template<std::uint8_t fbits, std::intmax_t num, std::intmax_t den>
struct _from_ratio<fbits, std::ratio<num, den>> {
    typedef div<from_integral<fbits, num>,
                from_integral<fbits, den>> type;
};

template<std::uint8_t fbits, typename T>
using from_ratio = typename _from_ratio<fbits, T>::type;

template<typename T>
struct _exp {};

template<std::uint8_t fbits, std::int32_t data>
struct _exp<fixed<fbits, data>> {
private:
    constexpr static std::int32_t compute() {
        bool neg = data < 0;
        std::int32_t value = data;
        if (neg) {
            value = -value;
        }
        std::int32_t result = value + (1 << fbits);
        std::int32_t term = value;

        for (std::uint8_t n = 2; n < 30; ++n) {
            term = _mul_f<fbits>(term, _div_f<fbits>(value, n << fbits));
            result += term;

            if (term < 500 && (n > 15 || term < 20)) {
                break;
            }
        }

        if (neg) {
            return _div_f<fbits>(1 << fbits, result);
        }
        else {
            return result;
        }
    }
public:
    typedef fixed<fbits, compute()> type;
};

template<typename T>
using exp = typename _exp<T>::type;
}
