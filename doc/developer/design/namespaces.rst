.. SPDX-FileCopyrightText: Intel Corporation
..
.. SPDX-License-Identifier: BSD-3-Clause

============
 Namespaces
============

See `oneAPI Style Guide`_.

Option 1
========

::

   dr::
   sp::
   mp::

Option 2
========

::

   dr::
   dr::sp::
   dr::mp::

Variation 1
===========

``sp.hpp`` does::

  namespace dr {

  using namespace sp;

  }}

If you include ``sp.hpp``, everything you need is in ``dr::``.

Variation 2
===========

::

   dr::spp::
   dr::mpp::

Variation 3
===========

::

   dr::sp::
   dr::mp::

Variation 4
===========

::

   dr::single::
   dr::multi::

Variation 5
===========

::

   dr::spmd::
   dr::smp::

Not clear what is counterpart to SPMD_.

.. _SPMD: https://en.wikipedia.org/wiki/Single_program,_multiple_data#:~:text=SPMD%20usually%20refers%20to%20message%20passing%20programming%20on%20distributed%20memory
.. _`oneAPI Style Guide`: oneapi-cpp-style-guide.md
