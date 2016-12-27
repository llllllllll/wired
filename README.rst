wired
=====

wip

compile time fixed point scalars and arrays for C++17

Example
-------

.. code-block:: c++

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
   add<values, a>: {4, 7, 2.5, 3.5}
   sum<values>: 9
   reduce<sum, values>: 7.5
   arr2d: {{2, 5}, {0.5, 1.5}}
   add<arr2d, a>: {{4, 7}, {2.5, 3.5}}
   column_array<a, b>: {{2}, {5}}
   getitem<arr2d, 0, 1>: 5
   shape<a>: {}
   shape<values>: {4}
   shape<arr2d>: {2, 2}

``protocol7``
-------------

``wired`` also comes with utilities for reading and writing data files in a
custom format called ``protocol7``. A ``protocol7`` file is a valid C++17
header file which defines a namespace holding some number of
``wired::array``\s. ``wired`` comes with a utility, ``csv2p7`` to generate a
``protocol7`` file from a csv. The resulting ``protocol7`` file contains a
single 2d ``wired::array`` with no column labels.

Example
```````

.. code-block::

   $ cat etc/test.csv
   a,b,c
   1.0,2.0,3.0
   4.0,5.0,7.0
   7.0,8.0,9.0
   1.5,2.5,3.5
   $ bin/csv2p7 data etc/test.csv

.. code-block:: c++

   #pragma once

   #include <wired/array.h>
   #include <wired/scalar.h>

   namespace protocol7 {
   using data = wired::array<wired::array<wired::from_integral<1>, wired::from_integral<2>, wired::from_integral<3>>,
                             wired::array<wired::from_integral<4>, wired::from_integral<5>, wired::from_integral<7>>,
                             wired::array<wired::from_integral<7>, wired::from_integral<8>, wired::from_integral<9>>,
                             wired::array<wired::fixed<98304>, wired::fixed<163840>, wired::fixed<229376>>>;
   }

Full Usage
``````````

.. code-block::

   $ bin/csv2p7 --help
   usage: bin/csv2p7 [-h] [-o PATH] [--namespace IDENTIFIER] [--fbits UINT8]
                     NAME SRC

   Convert a csv file into a protocol7 file.

   positional arguments:
     NAME                  The name of the wired::array generated.
     SRC                   The path to the source csv file.

   optional arguments:
     -h, --help            show this help message and exit
     -o PATH, --output PATH
                           The path to write the output protocol7 file to. By
                           default or if '-' is explicitly passed, the output
                           will be printed to stdout.
     --namespace IDENTIFIER
                           The namespace to put the wired::array in.
     --fbits UINT8         The number of bits to the right of the decimal in the
                           resulting wired::fixed types.
