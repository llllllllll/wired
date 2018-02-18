#pragma once

#include <array>
#include <ratio>
#include <tuple>
#include <type_traits>
#include <utility>

#include "wired/scalar.h"
#include "wired/utils.h"

namespace wired {
/** An n-dimensional array of wired::fixed values.

    @tparam Vs Axis 0 of the array.
 */
template<typename... Vs>
struct array;

namespace dispatch {
template<typename A>
struct shape {
    using type = std::index_sequence<>;
};

template<typename V>
struct shape<array<V>> {
    using type = utils::cat_index_sequences<std::index_sequence<1>,
                                            typename shape<V>::type>;
};

template<typename V, typename... Vs>
struct shape<array<V, Vs...>> {
    using type =
        utils::cat_index_sequences<std::index_sequence<sizeof...(Vs) + 1>,
                                   typename shape<V>::type>;
};
}  // namespace dispatch

/** Get the shape of a `wired::array` as a `std::index_sequence`.

    @tparam The array to get the shape of. Scalar values have an empty shape.
*/
template<typename V>
using shape = typename dispatch::shape<V>::type;

namespace utils {
namespace dispatch {
template<typename A>
struct shape_to_array_type;

template<>
struct shape_to_array_type<std::index_sequence<>> {
    using type = std::array<double, 0>;
};

template<std::size_t ix>
struct shape_to_array_type<std::index_sequence<ix>> {
    using type = std::array<double, ix>;
};

template<std::size_t ix, std::size_t... ixs>
struct shape_to_array_type<std::index_sequence<ix, ixs...>> {
    using type = std::array<
        typename shape_to_array_type<std::index_sequence<ixs...>>::type,
        ix>;
};
}  // namespace dispatch

/** Convert an shape into a specialization of `std::array` suitable for
    representing the materialized value of the given array.

    This does not support scalar shapes (`{}`).

    @tparam I The index sequence representing the shape of a `wired::array`.
*/
template<typename I>
using shape_to_array_type = typename dispatch::shape_to_array_type<I>::type;
}  // namespace utils

/** Return the number of dimensions in a given `wired::array`. Scalars have an
    `ndim` of 0.

    @tparam V The value to get the number of dimensions of.
 */
template<typename V>
constexpr std::size_t ndim = shape<V>::size();

/** The size of an object. This is 0 for scalars, otherwise it is the length
    of the first axis.

    @tparam V The object to get the size of.
 */
template<typename V>
constexpr std::size_t size = 0;

template<typename... Vs>
constexpr std::size_t size<array<Vs...>> = sizeof...(Vs);

template<typename... Vs>
struct array {
    /** Materialize an array of scalars.

        @return An array of doubles.
     */
    constexpr static utils::shape_to_array_type<shape<array<Vs...>>>
    materialize() {
        return {Vs::materialize()...};
    }
};

template<typename V, typename... Vs>
struct array<V, Vs...>  {
    /** Materialize an array of ndim > 1.

        @return A nested array of doubles.
     */
    constexpr static std::enable_if_t<
        (... && utils::index_sequence_eq<shape<V>, shape<Vs>>),
        utils::shape_to_array_type<shape<array<V, Vs...>>>>
    materialize() {
        return {V::materialize(), Vs::materialize()...};
    }
};

/** Helper to generate an array of shape `{sizeof...(Vs), 1}` from a sequence
    of scalars.

    @tparam Vs The values of the column array.
*/
template<typename... Vs>
using column_array = array<array<Vs>...>;

/** Check if two objects are compatible for broadcasted operations.

    @tparam A The first object to compare.
    @tparam B The second object to compare.
 */
template<typename A, typename B>
constexpr bool shape_compatible =
    utils::index_sequence_shape_compat<utils::one_pad<shape<A>,
                                                      shape<B>::size()>,
                                       utils::one_pad<shape<B>,
                                                      shape<A>::size()>>;

namespace dispatch {
template<typename T, std::size_t... ns>
struct getitem;

template<std::size_t n, typename... Vs>
struct getitem<array<Vs...>, n> {
    // this will generate an error already but the message is horrific
    static_assert(n < sizeof...(Vs), "getitem index out of bounds");
    using type = std::remove_reference_t<
        decltype(std::get<n>(std::make_tuple(std::declval<Vs>()...)))>;
};

template<std::size_t n, std::size_t... ns, typename... Vs>
struct getitem<array<Vs...>, n, ns...> {
    // this will generate an error already but the message is horrific
    static_assert(n < sizeof...(Vs), "getitem index out of bounds");
    using type = typename getitem<typename getitem<array<Vs...>, n>::type,
                                  ns...>::type;
};
}  // namespace dispatch


/** Look up elements from a `wired::array`.

    @tparam T The array to index into.
    @tparam ns The indices to slice. This returns an object with an `ndim` of
               `ndim<T> - sizeof...(ns)`.
 */
template<typename T, std::size_t... ns>
using getitem = typename dispatch::getitem<T, ns...>::type;

namespace dispatch {
template<template<typename A, typename B> typename Op, typename Vs>
struct reduce;

// base case
template<template<typename A, typename B> typename Op, typename V>
struct reduce<Op, array<V>> {
    using type = V;
};

template<template<typename A, typename B> typename Op,
         typename V, typename... Vs>
struct reduce<Op, array<V, Vs...>> {
    using type = Op<V, typename reduce<Op, array<Vs...>>::type>;
};
}  // namespace dispatch

/** Reduce an array along axis 0 with a given binary operator.

    @tparam Op The operator to reduce with.
    @tparam T The array to reduce.
 */
template<template<typename A, typename B> typename Op, typename T>
using reduce = typename dispatch::reduce<Op, T>::type;

/** Sum an array along axis 0.

    This is shorthand for `reduce<add, T>`.

    @tparam T The array to sum.
 */
template<typename T>
using sum = reduce<add, T>;

/** Take the product of an array along axis 0.

    This is shorthand for `reduce<mul, T>`.

    @tparam T The array to take the product of.
 */
template<typename T>
using product = reduce<mul, T>;

namespace dispatch {
template<typename A, typename B>
struct align_dimensions {
private:
    struct base_case {
        using type = std::pair<A, B>;
    };

public:
    using type = typename utils::cond<
        utils::case_<(wired::ndim<A> < wired::ndim<B>), align_dimensions<array<A>, B>>,
        utils::case_<(wired::ndim<A> > wired::ndim<B>), align_dimensions<A, array<B>>>,
        utils::case_<true, base_case>>::type;
};
}  // namespace dispatch

/** Align the dimensions of two objects. This creates a `std::pair` of objects
    whose `ndim` is equal. The shape of the lower dimension object is extended
    to the left with ones.

    @tparam A The first object to align.
    @tparam B The second object to align.
 */
template<typename A, typename B>
using align_dimensions = typename dispatch::align_dimensions<A, B>::type;

namespace dispatch {
template<typename A, typename B>
struct bin_concat;

template<typename... Vs, typename... Us>
struct bin_concat<array<Vs...>, array<Us...>> {
    using type = array<Vs..., Us...>;
};
}  // namespace dispatch

namespace detail {
template<typename A, typename B>
using bin_concat = typename dispatch::bin_concat<A, B>::type;
}  // namespace detail


/** Concatenate a sequence of arrays along axis 0.

    @tparam As The arrays to concatenate.
 */
template<typename... As>
using concat = utils::reduce<detail::bin_concat, As...>;

namespace dispatch {
template<std::size_t len, typename V>
struct full {
    using type = detail::bin_concat<wired::array<V>,
                                    typename full<len - 1, V>::type>;
};

/** Helper for creating an array of length `len` populated with a constant
    value.

    @tparam len The length of the new array.
    @tparam V The value to populate with.
 */
template<typename V>
struct full<0, V> {
    using type = wired::array<>;
};
}  // namespace dispatch

/** Create an array filled with a single value.

    @tparam len The length of the array to create.
    @tparam V The value to initialize the array with.
 */
template<std::size_t len, typename V>
using full = typename dispatch::full<len, V>::type;

/** Helper for creating an array of length `len` populated with all zeros.

    @tparam len The length of the new array.
 */
template<std::size_t len>
using zeros = full<len, wired::from_integral<0>>;

/** Helper for creating an array of length `len` populated with all ones.

    @tparam len The length of the new array.
 */
template<std::size_t len>
using ones = full<len, wired::from_integral<1>>;

namespace dispatch {
template<std::size_t ix, typename A>
struct split;

template<std::size_t ix, typename V, typename... Vs>
struct split<ix, array<V, Vs...>> {
private:
    using rec = split<ix - 1, array<Vs...>>;

public:
    using type =
        std::pair<wired::concat<array<V>, typename rec::type::first_type>,
                  typename rec::type::second_type>;
};

template<typename V, typename... Vs>
struct split<0, array<V, Vs...>> {
private:
    using rec = split<0, array<Vs...>>;

public:
    using type =
        std::pair<typename rec::type::first_type,
                  wired::concat<array<V>, typename rec::type::second_type>>;
};

template<std::size_t ix>
struct split<ix, array<>> {
    using type = std::pair<array<>, array<>>;
};
}  // namespace dispatch

/** Split an array into two parts along axis 0.

    @tparam ix The pivot point.
    @tparam A The array to split.
 */
template<std::size_t ix, typename A>
using split = typename dispatch::split<ix, A>::type;

namespace dispatch {
namespace detail {
template<std::size_t n, typename V>
struct repeat_n {
    using type = concat<array<V>, typename repeat_n<n - 1, V>::type>;
};

template<typename V>
struct repeat_n<0, V> {
    using type = array<>;
};
}  // detail

template<std::size_t c, typename I>
struct repeat;

template<std::size_t c, typename... Vs>
struct repeat<c, array<Vs...>> {
    using type = concat<typename detail::repeat_n<c, Vs>::type...>;
};
}  // dispatch

/** Repeat each element of an array `c` times.

    @tparam c The number of times to repeat each element.
    @tparam A The array to repeat.
 */
template<std::size_t c, typename A>
using repeat = typename dispatch::repeat<c, A>::type;

namespace dispatch {
namespace detail {
template<typename R, typename C, typename A>
struct flat_T;

template<std::size_t... rows, std::size_t... columns, typename A>
struct flat_T<std::index_sequence<rows...>,
              std::index_sequence<columns...>,
              A> {
    using type = wired::array<wired::getitem<A, rows, columns>...>;
};

template<std::size_t nrows, typename A>
struct reshape_T {
private:
    using pair = wired::split<nrows, A>;
    using head = typename pair::first_type;
    using tail = typename pair::second_type;

public:
    using type = wired::concat<array<head>,
                               typename reshape_T<nrows, tail>::type>;
};

template<std::size_t nrows>
struct reshape_T<nrows, array<>> {
    using type = wired::array<>;
};
}  // namespace detail

template<typename V>
struct T;

template<typename... Vs, typename... As>
struct T<array<array<Vs...>, As...>> {
private:
    constexpr static std::size_t nrows = sizeof...(As) + 1;
    constexpr static std::size_t ncols = sizeof...(Vs);

    using base_rows = std::make_index_sequence<nrows>;
    using base_columns = std::make_index_sequence<ncols>;


    using rows = utils::tile_index_sequence<ncols, base_rows>;
    using columns = utils::repeat_index_sequence<nrows, base_columns>;

    using flat = typename detail::flat_T<rows,
                                         columns,
                                         array<array<Vs...>, As...>>::type;

public:
    using type = typename detail::reshape_T<nrows, flat>::type;
};
}  // namespace dispatch

/** Transpose a 2d array.

    @tparam V The array to transpose.
 */
template<typename V>
using T = typename dispatch::T<V>::type;

namespace dispatch {
template<std::size_t ix, typename A>
struct hsplit {
private:
    using split_T = wired::split<ix, wired::T<A>>;

public:
    using type = std::pair<wired::T<typename split_T::first_type>,
                           wired::T<typename split_T::second_type>>;

};
}  // namespace dispatch

/** Split an array along axis 1.

    @tparam The pivot point.
    @tparam A The array to split.
 */
template<std::size_t ix, typename A>
using hsplit = typename dispatch::hsplit<ix, A>::type;

namespace dispatch {
template<typename A, typename B>
struct dot;

namespace detail {
template<std::size_t dim, typename A, typename B>
struct inner_dot;

template<typename A, typename B>
struct inner_dot<0, A, B> {
    using type = wired::mul<A, B>;
};

template<typename A, typename B>
struct inner_dot<1, A, B> {
    using type = wired::sum<wired::mul<A, B>>;
};

template<typename A, typename Ix, typename B, typename Jx>
struct flat_matmul;

template<typename A, std::size_t... rows, typename BT, std::size_t... columns>
struct flat_matmul<A, std::index_sequence<rows...>,
                   BT, std::index_sequence<columns...>> {
    using type = wired::array<typename dot<wired::getitem<A, rows>,
                                           wired::getitem<BT, columns>>::type...>;
};

template<std::size_t nrows, typename A>
struct reshape_matmul {
private:
    using pair = wired::split<nrows, A>;
    using head = typename pair::first_type;
    using tail = typename pair::second_type;

public:
    using type = wired::concat<array<head>,
                               typename reshape_matmul<nrows, tail>::type>;
};

template<std::size_t nrows>
struct reshape_matmul<nrows, array<>> {
    using type = wired::array<>;
};

template<typename A, typename B>
struct inner_dot<2, A, B> {
private:
    using BT = wired::T<B>;
    constexpr static std::size_t nrows = wired::size<A>;
    constexpr static std::size_t ncols = wired::size<BT>;

    using base_rows = std::make_index_sequence<nrows>;
    using base_columns = std::make_index_sequence<ncols>;

    using rows = utils::repeat_index_sequence<ncols, base_rows>;
    using columns = utils::tile_index_sequence<nrows, base_columns>;

    using flat = typename flat_matmul<A, rows, BT, columns>::type;
public:
    using type = typename reshape_matmul<ncols, flat>::type;
};
}  // namespace detail

template<typename A, typename B>
struct dot {
    using type =
        typename detail::inner_dot<std::min(ndim<A>, ndim<B>), A, B>::type;
};
}  // namespace dispatch

template<typename A, typename B>
using dot = typename dispatch::dot<A, B>::type;

namespace dispatch {
template<std::int32_t op(std::int32_t, std::int32_t, std::uint8_t),
         typename... Vs,
         typename U>
struct binop<op, array<Vs...>, U> {
    using type = array<typename binop<op, Vs, U>::type...>;
};

template<std::int32_t op(std::int32_t, std::int32_t, std::uint8_t),
         typename V,
         typename... Us>
struct binop<op, V, array<Us...>> {
    using type = array<typename binop<op, V, Us>::type...>;
};

namespace detail {
template<std::int32_t op(std::int32_t, std::int32_t, std::uint8_t),
         typename A,
         typename B>
struct binop_aligned;

// note: without this dispatch, it is ambiguous if we mean:
// "broadcast V against U" or "broadcast U against V", so we specialize the
// case of op(size 1 array, size 1 array)
template<std::int32_t op(std::int32_t, std::int32_t, std::uint8_t),
         typename V,
         typename U>
struct binop_aligned<op, array<V>, array<U>> {
    using type = array<typename binop<op, V, U>::type>;
};

template<std::int32_t op(std::int32_t, std::int32_t, std::uint8_t),
         typename V,
         typename... Us>
struct binop_aligned<op, array<V>, array<Us...>> {
    using type = array<typename binop<op, V, Us>::type...>;
};

template<std::int32_t op(std::int32_t, std::int32_t, std::uint8_t),
         typename... Vs,
         typename U>
struct binop_aligned<op, array<Vs...>, array<U>> {
    using type = array<typename binop<op, Vs, U>::type...>;
};

template<std::int32_t op(std::int32_t, std::int32_t, std::uint8_t),
         typename... Vs,
         typename... Us>
struct binop_aligned<op, array<Vs...>, array<Us...>> {
    using type = array<typename binop<op, Vs, Us>::type...>;
};
}  // detail

template<std::int32_t op(std::int32_t, std::int32_t, std::uint8_t),
         typename... Vs,
         typename... Us>
struct binop<op, array<Vs...>, array<Us...>> {
private:
    using pair = wired::align_dimensions<array<Vs...>, array<Us...>>;
    using A = typename pair::first_type;
    using B = typename pair::second_type;

public:
    using type = typename detail::binop_aligned<op, A, B>::type;
};

template<std::int32_t op(std::int32_t, std::uint8_t),
         typename... Vs>
struct unop<op, array<Vs...>> {
    using type = array<typename unop<op, Vs>::type...>;
};
}  // namespace dispatch
}  // namespace wired
