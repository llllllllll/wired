#include <iostream>
#include <utility>

#include "wired/array.h"
#include "wired/scalar.h"

namespace {
template<typename T, std::size_t n>
std::ostream& operator<<(std::ostream& stream, std::array<T, n> arr) {
    auto it = arr.cbegin();
    stream << '{';
    if (it == arr.cend()) {
        return stream << '}';
    }
    else {
        stream << *it++;
    }
    while (it != arr.cend()) {
        stream << ", " << *it++;
    }
    return stream << '}';
}

template<std::size_t... ns>
std::ostream& operator<<(std::ostream& stream, std::index_sequence<ns...>) {
    return stream << std::array<std::size_t, sizeof...(ns)>{ns...};
}
}

int main() {
    // create 2 integral scalars
    using a = wired::from_integral<2>;
    using b = wired::from_integral<5>;

    // create 2 fraction scalars
    using c = wired::from_ratio<std::ratio<1, 2>>;
    using d = wired::from_ratio<std::ratio<3, 2>>;

    // create an array from our 4 scalars
    using values = wired::array<a, b, c, d>;

    // lookup items out of the array
    std::cout << "getitem<values, 0>: "
              << wired::getitem<values, 0>::materialize() << '\n'
              << "getitem<values, 1>: "
              << wired::getitem<values, 1>::materialize() << '\n'
              << "getitem<values, 2>: "
              << wired::getitem<values, 2>::materialize() << '\n'
              << "getitem<values, 3>: "
              << wired::getitem<values, 3>::materialize() << '\n';


    // scalar arithmetic
    std::cout << "mul<a, b>: " << wired::mul<a, b>::materialize() << '\n';

    // nested expressions
    std::cout << "div<add<mul<a, b>, c>, d>: "
              << wired::div<wired::add<wired::mul<a, b>, c>, d>::materialize()
              << '\n';

    // exp
    std::cout << "exp<b>: " << wired::exp<b>::materialize() << '\n';

    // broadcasted arithmetic
    std::cout << "add<values, a>: "
              << wired::add<values, a>::materialize() << '\n';

    // reductions
    std::cout << "sum<values>: " << wired::sum<values>::materialize() << '\n';

    // arbitrary reduction (could use product, but just for show)
    std::cout << "reduce<sum, values>: "
              << wired::reduce<wired::mul, values>::materialize()
              << '\n';

    // 2d arrays
    using arr2d = wired::array<wired::array<a, b>,
                               wired::array<c, d>>;
    std::cout << "arr2d: " << arr2d::materialize() << '\n';

    std::cout << "add<arr2d, a>: "
              << wired::add<arr2d, a>::materialize() << '\n';

    std::cout << "column_array<a, b>: "
              << wired::column_array<a, b>::materialize() << '\n';

    // 2d indexing
    std::cout << "getitem<arr2d, 0, 1>: "
              << wired::getitem<arr2d, 0, 1>::materialize() << '\n';
    std::cout << "getitem<arr2d, 0>: "
              << wired::getitem<arr2d, 0>::materialize() << '\n';

    // check the shape arrays or scalars
    std::cout << "shape<a>: " << wired::shape<a>{} << '\n';
    std::cout << "shape<values>: " << wired::shape<values>{} << '\n';
    std::cout << "shape<arr2d>: " << wired::shape<arr2d>{} << '\n';

    using row = wired::array<a, b>;
    using column = wired::column_array<c, d>;
    std::cout << "row: " << row::materialize() << '\n';
    std::cout << "column: " << column::materialize() << '\n';

    // check the number of dimensions
    std::cout << "ndim<row>: " << wired::ndim<row>;
    std::cout << "ndim<column>: " << wired::ndim<column> << '\n';

    // broadcasted arithmetic to expand dimensions
    std::cout << "add<row, column>: "
              << wired::add<row, column>::materialize() << '\n';

    return 0;
}
