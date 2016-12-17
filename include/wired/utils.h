#pragma once

#include <cstdint>

namespace wired {
template<typename T, typename U>
constexpr auto template_rshift(T t, U u) {
    return t >> u;
}

template<std::int32_t value>
constexpr uint8_t clz() {
    return __builtin_clz(value);
}

template<std::int32_t value>
constexpr std::int32_t abs() {
    return __builtin_abs(value);
}

template<std::int32_t to, std::int32_t from>
constexpr std::int32_t copysign() {
    return __builtin_copysign(to, from);
}
}
