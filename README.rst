wired
=====

wip

compile time fixed point scalars and arrays.

Example
-------

.. code-block:: c++

   #include <iostream>
   #include <utility>

   #include "wired/array.h"
   #include "wired/scalar.h"

   static constexpr std::uint8_t base = 16;

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
       using a = wired::from_integral<base, 2>;
       using b = wired::from_integral<base, 5>;

       // create 2 fraction scalars
       using c = wired::from_ratio<base, std::ratio<1, 2>>;
       using d = wired::from_ratio<base, std::ratio<3, 2>>;

       // create an array from our 4 scalars
       using values = wired::array<a, b, c, d>;

       // lookup items out of the array
       std::cout << "getitem<values, 0>: "
                 << wired::getitem<values, 0>::as_double() << '\n'
                 << "getitem<values, 1>: "
                 << wired::getitem<values, 1>::as_double() << '\n'
                 << "getitem<values, 2>: "
                 << wired::getitem<values, 2>::as_double() << '\n'
                 << "getitem<values, 3>: "
                 << wired::getitem<values, 3>::as_double() << '\n';


       // scalar arithmetic
       std::cout << "mul<a, b>: " << wired::mul<a, b>::as_double() << '\n';

       // nested expressions
       std::cout << "div<add<mul<a, b>, c>, d>: "
                 << wired::div<wired::add<wired::mul<a, b>, c>, d>::as_double()
                 << '\n';

       // exp
       std::cout << "exp<b>: " << wired::exp<b>::as_double() << '\n';

       // broadcasted arithmetic
       std::cout << "add<values, a>: " << wired::add<values, a>::as_doubles() << '\n';

       // reductions
       std::cout << "sum<values>: " << wired::sum<values>::as_double() << '\n';

       // arbitrary reduction (could use product, but just for show)
       std::cout << "reduce<sum, values>: "
                 << wired::reduce<wired::mul, values>::as_double()
                 << '\n';

       // 2d arrays
       using arr2d = wired::array<wired::array<a, b>,
                                  wired::array<c, d>>;

       using arrbroken = wired::array<wired::array<a, b, c>,
                                      wired::array<c>>;

       // 2d indexing
       std::cout << "getitem<arr2d, 0, 1>: "
                 << wired::getitem<arr2d, 0, 1>::as_double() << '\n';

       // check the shape arrays or scalars
       std::cout << "shape<a>: " << wired::shape<a>{} << '\n';
       std::cout << "shape<values>: " << wired::shape<values>{} << '\n';
       std::cout << "shape<arr2d>: " << wired::shape<arr2d>{} << '\n';

       return 0;
   }

.. code-block::

   $ ./a.out
   getitem<values, 0>: 2
   getitem<values, 1>: 5
   getitem<values, 2>: 0.5
   getitem<values, 3>: 1.5
   mul<a, b>: 10
   div<add<mul<a, b>, c>, d>: 7
   exp<b>: 148.408
   add<values, a>: {7, 2.5, 3.5, 0}
   sum<values>: 9
   reduce<sum, values>: 7.5
   getitem<arr2d, 0, 1>: 5
   shape<a>: {}
   shape<values>: {4}
   shape<arr2d>: {2, 2}
