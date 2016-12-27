#pragma once

#include <cstdint>

namespace wired {
namespace op {

constexpr std::int32_t add(std::int32_t lhs,
                           std::int32_t rhs,
                           std::uint8_t) {
    return lhs + rhs;
}

constexpr std::int32_t sub(std::int32_t lhs,
                           std::int32_t rhs,
                           std::uint8_t) {
    return lhs - rhs;
}

constexpr std::int32_t mul(std::int32_t lhs,
                           std::int32_t rhs,
                           std::uint8_t fbits) {
    return (static_cast<std::int64_t>(lhs) * rhs) >> fbits;
}

constexpr std::int32_t div(std::int32_t lhs,
                           std::int32_t rhs,
                           std::uint8_t fbits) {
    return (static_cast<std::int64_t>(lhs) << fbits) / rhs;
}

constexpr std::int32_t exp(std::int32_t data, std::uint8_t fbits) {
    bool neg = data < 0;
    std::int32_t value = data;
    if (neg) {
        value = -value;
    }
    std::int32_t result = value + (1 << fbits);
    std::int32_t term = value;

    for (std::uint8_t n = 2; n < 30; ++n) {
        term = mul(term, div(value, n << fbits, fbits), fbits);
        result += term;

        if (term < 500 && (n > 15 || term < 20)) {
            break;
        }
    }
    if (neg) {
        return div(1 << fbits, result, fbits);
    }
    else {
        return result;
    }
}

constexpr std::int32_t neg(std::int32_t data, std::uint8_t) {
    return -data;
}

constexpr std::int32_t inv(std::int32_t data, std::uint8_t) {
    return ~data;
}
}
}
