#pragma once

#include <wired/array.h>
#include <wired/scalar.h>

namespace layer9 {
namespace train {
using one = wired::from_integral<1>;

template<typename V>
using sigmoid = wired::div<one, wired::add<one, wired::exp<wired::neg<V>>>>;

template<typename V>
using sigmoid_derivative = wired::mul<V, wired::sub<one, V>>;

template<typename W, typename S, typename O, std::size_t iterations>
struct step {
private:
    using output = sigmoid<wired::dot<S, W>>;
    using error = wired::sub<O, output>;
    using error_contribution = wired::mul<error, sigmoid_derivative<output>>;
    using adjustment = wired::dot<wired::T<S>, error_contribution>;

public:
    using type = typename step<wired::add<W, adjustment>,
                               S,
                               O,
                               iterations - 1>::type;
};

template<typename W, typename S, typename O>
struct step<W, S, O, 0> {
    using type = W;
};
}  // namespace train

template<std::size_t m, std::size_t n, std::size_t p>
auto matmul(const std::array<std::array<double, m>, n>& lhs,
            const std::array<std::array<double, p>, m>& rhs) {
    std::array<std::array<double, p>, n> out;

    for (std::size_t i = 0; i < n; ++i) {
        for (std::size_t j = 0; j < p; ++j) {
            double sum = 0;
            for (std::size_t k = 0; k < m; ++k) {
                sum += lhs[i][k] * rhs[k][j];
            }
            out[i][j] = sum;
        }
    }

    return out;
}

// special case where n == 1; return an array instead of a (1, p) array
template<std::size_t m, std::size_t p>
auto matmul(const std::array<double, m>& lhs,
            const std::array<std::array<double, p>, m>& rhs) {
    std::array<double, p> out;

    for (std::size_t j = 0; j < p; ++j) {
        double sum = 0;
        for (std::size_t k = 0; k < m; ++k) {
            sum += lhs[k] * rhs[k][j];
        }
        out[j] = sum;
    }

    return out;
}


/** Predict values based on training data.

    @tparam Samples The 2d array of samples. Each row corresponds to a single
                    sample.
    @tparam Observations The 2d array of observations. Each row corresponds to
                         the output value of the same row in `Samples`.
    @tparam iterations How many training iterations should be used?
    @param sample The sample to predict.
    @return The predicted values from `sample`.
 */
template<typename Samples, typename Observations, std::size_t iterations = 100>
auto predict(const decltype(wired::getitem<Samples, 0>::materialize())& sample) {
    using init_W =
        wired::T<wired::array<wired::ones<wired::size<wired::T<Samples>>>>>;
    using W = typename train::step<init_W,
                                   Samples,
                                   Observations,
                                   iterations>::type;
    constexpr static auto w = W::materialize();

    return matmul(sample, w);
}
}  // namespace layer9
