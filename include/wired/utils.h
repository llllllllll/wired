#pragma once

#include <array>
#include <cstdint>
#include <utility>

namespace wired::utils {
namespace dispatch {
template<template<typename A, typename B> typename Op, typename... Vs>
struct reduce;

// base case
template<template<typename A, typename B> typename Op, typename V>
struct reduce<Op, V> {
    using type = V;
};

template<template<typename A, typename B> typename Op,
         typename V, typename... Vs>
struct reduce<Op, V, Vs...> {
    using type = Op<V, typename reduce<Op, Vs...>::type>;
};
}  // namespace dispatch

/** Reduce a sequence of scalars with a binary operator.

    @tparam Op The operator to reduce with.
    @tparam Vs The values to reduce.
 */
template<template<typename A, typename B> typename Op, typename... Vs>
using reduce = typename dispatch::reduce<Op, Vs...>::type;

namespace dispatch {
template<typename A, typename B>
struct bin_cat_index_sequence;

template<std::size_t... ix, std::size_t... jx>
struct bin_cat_index_sequence<std::index_sequence<ix...>,
                          std::index_sequence<jx...>> {
    using type = std::index_sequence<ix..., jx...>;
};
}  // namespace dispatch

namespace detail {
template<typename A, typename B>
using bin_cat_index_sequence =
    typename dispatch::bin_cat_index_sequence<A, B>::type;
}  // namespace detail

/** Concatenate a sequence of index sequences.

    @tparam Ixs The index sequences to concatenate.
 */
template<typename... Ixs>
using cat_index_sequences = reduce<detail::bin_cat_index_sequence, Ixs...>;

namespace dispatch {
namespace detail {
template<std::size_t n, std::size_t value>
struct repeat_n {
    using type = cat_index_sequences<std::index_sequence<value>,
                                     typename repeat_n<n - 1, value>::type>;
};

template<std::size_t value>
struct repeat_n<0, value> {
    using type = std::index_sequence<>;
};
}  // detail

template<std::size_t c, typename I>
struct repeat_index_sequence;

template<std::size_t c, std::size_t... Ixs>
struct repeat_index_sequence<c, std::index_sequence<Ixs...>> {
    using type = cat_index_sequences<typename detail::repeat_n<c, Ixs>::type...>;
};
}  // dispatch

/** Repeat each element of an index sequence `c` times.

    @tparam c The number of times to repeat each element.
    @tparam I The index sequence to repeat.
 */
template<std::size_t c, typename I>
using repeat_index_sequence =
    typename dispatch::repeat_index_sequence<c, I>::type;

namespace dispatch {
template<std::size_t c, typename I>
struct tile_index_sequence {
    using type =
        cat_index_sequences<I, typename tile_index_sequence<c - 1, I>::type>;
};

template<typename I>
struct tile_index_sequence<0, I> {
    using type = std::index_sequence<>;
};
}  // namespace dispatch

/** Tile an index sequence `c` times.

    @tparam c The number of times to tile `I`.
    @tparam I The index sequence to tile.
 */
template<std::size_t c, typename I>
using tile_index_sequence = typename dispatch::tile_index_sequence<c, I>::type;

/** Convert an index sequence into a `std::array`.

    @param I The index sequence to convert.
    @returns A constexpr array with the same values as `I`.
 */
template<std::size_t... ixs>
constexpr std::array<std::size_t, sizeof...(ixs)>
index_sequence_to_array(std::index_sequence<ixs...>) {
    return {ixs...};
}

namespace dispatch {
template<typename A, typename B>
struct index_sequence_eq;

template<std::size_t... ix, std::size_t... jx>
struct index_sequence_eq<std::index_sequence<ix...>,
                         std::index_sequence<jx...>> {
    constexpr static bool value = (... && (ix == jx));
};

template<typename A, typename B>
struct index_sequence_shape_compat;

template<std::size_t... ix, std::size_t... jx>
struct index_sequence_shape_compat<std::index_sequence<ix...>,
                                   std::index_sequence<jx...>> {
    constexpr static bool value = (... && (ix == jx || ix == 1 || jx == 1));
};
}  // namespace dispatch

/** Check if two index sequences are equal.

    @tparam A The first index sequence.
    @tparam B The second index sequence.
 */
template<typename A, typename B>
constexpr bool index_sequence_eq = dispatch::index_sequence_eq<A, B>::value;

/** Check if two shapes, represented with index sequences, are compatible for
    broadcasting.

    @tparam A The first index sequence.
    @tparam B The second index sequence.
 */
template<typename A, typename B>
constexpr bool index_sequence_shape_compat =
    dispatch::index_sequence_shape_compat<A, B>::value;

namespace dispatch {
template<std::size_t len, std::size_t n>
struct full {
    using type = cat_index_sequences<std::index_sequence<n>,
                                     typename full<len - 1, n>::type>;
};

template<std::size_t n>
struct full<0, n> {
    using type = std::index_sequence<>;
};
}  // namespace dispatch

/** Helper for creating an index sequence of length `len` populated with a
    constant value.

    @tparam len The length of the new index sequence.
    @tparam n The value to populate with.
 */
template<std::size_t len, std::size_t n>
using full = typename dispatch::full<len, n>::type;

/** Helper for creating an index sequence of length `len` populated with all
    zeros.

    @tparam len The length of the new index sequence.
 */
template<std::size_t len>
using zeros = full<len, 0>;

/** Helper for creating an index sequence of length `len` populated with all
    ones.

    @tparam len The length of the new index sequence.
 */
template<std::size_t len>
using ones = full<len, 0>;

namespace dispatch {
template<typename Ix, std::size_t n, std::size_t len>
struct pad;

template<std::size_t... ixs, std::size_t n, std::size_t len>
struct pad<std::index_sequence<ixs...>, n, len> {
private:
    constexpr static std::size_t sz = sizeof...(ixs);
public:
    using type =
        cat_index_sequences<typename full<n, (sz > len) ? 0 : len - sz>::type,
                            std::index_sequence<ixs...>>;
};
}

/** Left-pad an index sequence with a given value.

    @tparam Ix The index sequence to pad.
    @tparam n The value to pad with.
    @tparam len The length to pad to.
 */
template<typename Ix, std::size_t n, std::size_t len>
using pad = typename dispatch::pad<Ix, n, len>::type;

/** Left-pad an index sequence with zero.

    @tparam Ix The index sequence to pad.
    @tparam len The length to pad to.
 */
template<typename Ix, std::size_t len>
using zero_pad = typename dispatch::pad<Ix, 0, len>::type;

/** Left-pad an index sequence with one.

    @tparam Ix The index sequence to pad.
    @tparam len The length to pad to.
 */
template<typename Ix, std::size_t len>
using one_pad = typename dispatch::pad<Ix, 1, len>::type;

/** A single case in a cond expression.

    @tparam p The boolean predicate.
    @tparam V The value to return if `p`.
 */
template<bool p, typename V>
struct case_;

namespace dispatch {
template<typename... Cases>
struct cond;

template<typename V, typename... Cases>
struct cond<case_<true, V>, Cases...> {
    using type = V;
};

template<typename V, typename... Cases>
struct cond<case_<false, V>, Cases...> {
    using type = typename cond<Cases...>::type;
};
}  // namespace dispatch

/** A lisp-like cond expression for types.

    @tparam Cases The cases to check in order. This results in the first type
                  paired with a predicate evaluating to true.
 */
template<typename... Cases>
using cond = typename dispatch::cond<Cases...>::type;

template<typename T>
struct print_type;

template<auto v>
struct print_value;
}  // namespace wired::utils
