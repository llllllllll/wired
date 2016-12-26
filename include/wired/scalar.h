#pragma once

#include <cstdint>
#include <type_traits>
#include <ratio>

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
using from_integral = fixed<fbits, value << fbits>;

namespace dispatch {

template<typename T, typename U>
struct add {};

template<std::int32_t lhs, std::int32_t rhs, std::uint8_t fbits>
struct add<fixed<lhs, fbits>, fixed<rhs, fbits>> {
    typedef fixed<lhs + rhs, fbits> type;
};

template<typename T, typename U>
struct sub {};

template<std::int32_t lhs, std::int32_t rhs, std::uint8_t fbits>
struct sub<fixed<lhs, fbits>, fixed<rhs, fbits>> {
    typedef fixed<lhs - rhs, fbits> type;
};

template<typename T, typename U>
struct mul {};

template<std::uint8_t fbits>
constexpr std::int32_t mul_f(std::int32_t lhs, std::int32_t rhs) {
    return (static_cast<std::int64_t>(lhs) * rhs) >> fbits;
}

template<std::int32_t lhs, std::int32_t rhs, std::uint8_t fbits>
struct mul<fixed<lhs, fbits>, fixed<rhs, fbits>> {
    typedef fixed<mul_f<fbits>(lhs, rhs), fbits> type;
};

template<typename T, typename U>
struct div {};

template<std::uint8_t fbits>
constexpr std::int32_t div_f(std::int32_t lhs, std::int32_t rhs) {
    return (static_cast<std::int64_t>(lhs) << fbits) / rhs;
}

template<std::int32_t lhs, std::int32_t rhs, std::uint8_t fbits>
struct div<fixed<lhs, fbits>, fixed<rhs, fbits>> {
    typedef fixed<div_f<fbits>(lhs, rhs), fbits> type;
};

template<typename T, std::uint8_t fbits>
struct from_ratio {};

template<std::intmax_t num, std::intmax_t den, std::uint8_t fbits>
struct from_ratio<std::ratio<num, den>, fbits> {
    typedef typename div<from_integral<num, fbits>,
                         from_integral<den, fbits>>::type type;
};

template<typename T>
struct exp {};

template<std::int32_t data, std::uint8_t fbits>
struct exp<fixed<data, fbits>> {
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

template<typename T, std::uint8_t fbits = default_fbits>
using from_ratio = typename dispatch::from_ratio<T, fbits>::type;

template<typename T>
using exp = typename dispatch::exp<T>::type;
}
