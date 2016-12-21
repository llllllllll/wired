#include <iostream>

#include "wired/array.h"
#include "wired/scalar.h"

static constexpr std::uint8_t base = 16;

int main() {
    // create 2 integral scalars
    using a = wired::from_integral<base, 2>;
    using b = wired::from_integral<base, 5>;

    // create 2 fraction scalars
    using c = wired::from_ratio<base, std::ratio<1, 2>>;
    using d = wired::from_ratio<base, std::ratio<3, 2>>;

    // create an array from our 4 scalars
    using values = wired::array<a, b, c, d>;

    // lookup items out of the array
    std::cout << "values[0]: "
              << wired::getitem<values, 0>::as_double() << '\n'
              << "values[1]: "
              << wired::getitem<values, 1>::as_double() << '\n'
              << "values[2]: "
              << wired::getitem<values, 2>::as_double() << '\n'
              << "values[3]: "
              << wired::getitem<values, 3>::as_double() << '\n';


    // scalar arithmetic
    std::cout << "a * b: " << wired::mul<a, b>::as_double() << '\n';

    // nested expressions
    std::cout << "(a * b + c) / d: "
              << wired::div<wired::add<wired::mul<a, b>, c>, d>::as_double()
              << '\n';

    // exp
    std::cout << "exp(b): " << wired::exp<b>::as_double() << '\n';

    // broadcasted arithmetic
    std::cout << "values + a:";
    for (auto d : wired::add<values, a>::as_doubles()) {
        std::cout << ' ' << d;
    }
    std::cout << '\n';

    // reductions
    std::cout << "sum<values>: " << wired::sum<values>::as_double() << '\n';

    // arbitrary reduction (could use product, but just for show)
    std::cout << "reduce<sum, values>: "
              << wired::reduce<wired::mul, values>::as_double()
              << '\n';

    return 0;
}
