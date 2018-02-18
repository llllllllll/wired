wired
=====

Compile time fixed point scalars and n-dimensional arrays for C++17.

What?
-----

``wired`` is a library which implements n-dimensional arrays similar to `numpy
<http://www.numpy.org/>`_. We can do arithmetic and some linear algebra
operations on these arrays.

The catch is that the computation is evaluated with the
**template-expander**. All of the calculations are solved at compile time, so
the output binaries will only ever include the equivalent of array or scalar
literals, no computations!

Method
------

Encoding Values
```````````````

Wired works by lifting all values into C++ types. For example, we would encode
the value ``1.0`` as ``wired::from_integral<1>`` which expands to the type:
``wired::fixed<65536, 16>``.

We encode arrays as a variadic template, it looks something like:

.. code-block::

   /** An n-dimensional array of wired::fixed values.

       @tparam Vs Axis 0 of the array.
    */
   template<typename... Vs>
   struct array;

Note that ``Vs...`` is only axis 0, so 2d arrays are just arrays of arrays, 3d
arrays are arrays of arrays of arrays, and so on.

Given that we use variadic templates to hold the values, the name ``array`` is a
lie, it is really more like a singly linked list.

Note: ``wired`` does use C++ values if they are acceptable for use as template
arguments. For example ``wired::ndim`` (return the rank of an object) will
expand to a ``std::size_t`` because that may be used as a template argument.

Fixed?
``````

``double`` is not actually a valid template argument type.

.. code-block::

   $ g++ a.cc
   a.cc:1:17: error: ‘double’ is not a valid type for a template non-type parameter
    template<double v>

This poses a problem for a library that wants to encode all of its values as C++
types. To get around this, ``wired`` uses fixed-point binary values with a
variable radix point, called ``fbits``.

For more information of fixed-point arithmetic, see `the wikipedia article
<https://en.wikipedia.org/wiki/Fixed-point_arithmetic>`_.

Note: This means that, for some operations, ``wired`` can have very different
precision than you would expect. This can either be much more precise, or much
less precise. This also means that we cannot take advantage of builtins like
``exp``, so we must approximate these functions without the help of our
hardware.

Encoding Functions
``````````````````

If C++ types are our values, then templated-using clauses are our (eager)
functions! For example, calling ``add`` looks like:

.. code-block:: c++

   using result = wired::add<a, b>;

and the definition of add looks like:

.. code-block:: c++

   /** Add two objects together with proper broadcasting.

       @tparam T The left hand side of the expression.
       @tparam U The right hand side of the expression.
    */
   template<typename T, typename U>
   using add = typename dispatch::binop<op::add, T, U>::type;

We can also use templated-structs to represent lazy functions, for example:

.. code-block:: c++

   template<typename T>
   struct lazy_function {
       using type = /* some template expression that expands to a type */;;
   };

To create a closure, we would "call" the template like:

.. code-block:: c++

   using closure = lazy_function<T>;

To actually enter the closure, we need to request the type field, like:

.. code-block:: c++

   /* we need to use the ``typename`` keyword because ``closure`` is a
    dependent scope, meaning we don't know if ``::type`` is a C++ type or C++
    value until we enter the closure (expand the template) */
   using result = typename closure::type;


Getting back to C++ values
``````````````````````````

``wired::fixed`` and ``wired::array`` both implement a ``constexpr static auto
materialize()``. For scalars, this returns a ``double``. For arrays, this
returns a ``std::array`` of the result of materializing all elements along
axis 0.

Arrays have the extra constraint that they may only be materialized if they are
not jagged, meaning all arrays on a given axis must be the same size. In general
``wired`` assumes non-jagged arrays.


``layer9``
----------

You've made it this far, so you are likely already sold on this idea. For those
who are still skeptical, I have written a demo program that shows the use of
``wired``.

The demo is a machine learning framework called ``layer9`` which trains a simple
perception using nothing but ``wired::array`` and ``wired::fixed`` at compile
time. The result is that we can create a ``predict`` function which has the
final weights vector as a ``constexpr std::array``.

See the ``layer9-example`` directory for a fully working example of how this
works.

Performance
```````````

How does this compare to other machine learning libraries? Great question! Let's
compare ``layer9`` to a popular Python machine learning framework: ``keras``.

I re-implemented the simple perception from the ``layer9-example`` in keras like
so:

.. code-block:: python

   In [1]: import keras
   Using TensorFlow backend.

   In [2]: input_node = keras.layers.Input(shape=(3,))

   In [3]: output_node = keras.layers.Dense(1, activation='sigmoid')(input_node)

   In [4]: model = keras.models.Model(input_node, output_node)

   In [5]: model.compile('rmsprop', 'mse')

   In [6]: samples = np.array([[0, 0, 1], [1, 1, 1], [1, 0, 1], [0, 1, 1]])

   In [7]: observations = np.array([[0], [1], [1], [0]])


Let's look at the training time:

.. code-block:: python

   In [8]: %timeit model.fit(samples, observations, epochs=100, verbose=False)
   10 loops, best of 3: 54.7 ms per loop

Note: I am using 100 epochs to match the ``layer9-example`` configuration.

Now, let's see how long it takes ``layer9`` to train this simple model on this
small data:

.. code-block:: bash

   $ time make layer9-example/predict

   real    0m24.984s
   user    0m23.091s
   sys     0m1.762s

Wow, only 25 seconds!

Takeaway
````````
While you might be tempted to use ``keras`` or ``tensorflow`` based on these
results, I think the simplicity of the ``wired`` execution model makes up for
its slightly lower performance.

``protocol7``
-------------

``wired`` also comes with utilities for reading and writing data files in a
custom format called ``protocol7``. A ``protocol7`` file is a valid C++17 header
file which defines a namespace holding some number of
``wired::array``\s. ``wired`` comes with a utility, ``csv2p7`` to generate a
``protocol7`` file from a csv. The resulting ``protocol7`` file contains a
single 2d ``wired::array``. ``csv2p7`` is very useful to encode data to pass to
``layer9``.

Example
```````

.. code-block::

   $ cat etc/test.csv
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


``psyche``
----------

Our benchmark for correctness is the ``numpy`` Python library. In order to
facilitate comparing ``wired`` programs with ``numpy`` programs, ``wired`` comes
with a Python library called ``psyche`` which provides a Python DSL which can be
compiled into a ``wired`` witness, or evaluated with ``numpy`` directly. This
allows us to write tests in a unified way without worrying about manual
translation errors.

For example:

.. code-block:: python

   In [1]: import psyche as ps

   In [2]: a = ps.Expr([1, 2, 3])

   In [3]: b = ps.Expr([[-1], [-2], [-3]])

   In [4]: expr = (a * b).T

   In [5]: expr
   Out[5]: Expr(T(Mul(Literal(array([1., 2., 3.])), Literal(array([[-1.], [-2.], [-3.]])))))

   In [6]: ps.evaluate_numpy(expr)
   Out[6]:
   array([[-1., -2., -3.],
          [-2., -4., -6.],
          [-3., -6., -9.]])

   In [7]: ps.evaluate_wired(expr, include_dir='include/')
   Out[7]:
   array([[-1., -2., -3.],
          [-2., -4., -6.],
          [-3., -6., -9.]])


The ``wired`` test suite is written in terms of ``psyche``
