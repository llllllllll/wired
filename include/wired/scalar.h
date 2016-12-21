#pragma once

#include <cstdint>
#include <type_traits>
#include <ratio>

#include "wired/utils.h"

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

namespace dispatch {

template<typename T, typename U>
struct add {};

template<std::uint8_t fbits, std::int32_t lhs, std::int32_t rhs>
struct add<fixed<fbits, lhs>, fixed<fbits, rhs>> {
    typedef fixed<fbits, lhs + rhs> type;
};

template<typename T, typename U>
struct sub {};

template<std::uint8_t fbits, std::int32_t lhs, std::int32_t rhs>
struct sub<fixed<fbits, lhs>, fixed<fbits, rhs>> {
    typedef fixed<fbits, lhs + rhs> type;
};

template<typename T, typename U>
struct mul {};

template<std::uint8_t fbits>
constexpr std::int32_t mul_f(std::int32_t lhs, std::int32_t rhs) {
    return (static_cast<std::int64_t>(lhs) * rhs) >> fbits;
}

template<std::uint8_t fbits, std::int32_t lhs, std::int32_t rhs>
struct mul<fixed<fbits, lhs>, fixed<fbits, rhs>> {
    typedef fixed<fbits, mul_f<fbits>(lhs, rhs)> type;
};

template<typename T, typename U>
struct div {};

template<std::uint8_t fbits>
constexpr std::int32_t div_f(std::int32_t lhs, std::int32_t rhs) {
    return (static_cast<std::int64_t>(lhs) << fbits) / rhs;
}

template<std::uint8_t fbits, std::int32_t lhs, std::int32_t rhs>
struct div<fixed<fbits, lhs>, fixed<fbits, rhs>> {
    typedef fixed<fbits, div_f<fbits>(lhs, rhs)> type;
};

template<std::uint8_t fbits, typename T>
struct from_ratio {};

template<std::uint8_t fbits, std::intmax_t num, std::intmax_t den>
struct from_ratio<fbits, std::ratio<num, den>> {
    typedef typename div<from_integral<fbits, num>,
                         from_integral<fbits, den>>::type type;
};

template<typename T>
struct exp {};

template<std::uint8_t fbits, std::int32_t data>
struct exp<fixed<fbits, data>> {
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
            term = mul_f<fbits>(term, div_f<fbits>(value, n << fbits));
            result += term;

            if (term < 500 && (n > 15 || term < 20)) {
                break;
            }
        }

        if (neg) {
            return div_f<fbits>(1 << fbits, result);
        }
        else {
            return result;
        }
    }
public:
    typedef fixed<fbits, compute()> type;
};
}

template<typename T, typename U>
using add = typename dispatch::add<T, U>::type;

template<typename T, typename U>
using sub = typename dispatch::sub<T, U>::type;

template<typename T, typename U>
using mul = typename dispatch::mul<T, U>::type;

template<typename T, typename U>
using div = typename dispatch::div<T, U>::type;

template<std::uint8_t fbits, typename T>
using from_ratio = typename dispatch::from_ratio<fbits, T>::type;

template<typename T>
using exp = typename dispatch::exp<T>::type;
}
