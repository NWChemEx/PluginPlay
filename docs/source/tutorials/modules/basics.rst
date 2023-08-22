.. Copyright 2022 NWChemEx-Project
..
.. Licensed under the Apache License, Version 2.0 (the "License");
.. you may not use this file except in compliance with the License.
.. You may obtain a copy of the License at
..
.. http://www.apache.org/licenses/LICENSE-2.0
..
.. Unless required by applicable law or agreed to in writing, software
.. distributed under the License is distributed on an "AS IS" BASIS,
.. WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
.. See the License for the specific language governing permissions and
.. limitations under the License.

**************************
Basics of Writing a Module
**************************

.. |acceleration| replace:: :math:`\vec{a}\left(\vec{r}\right)`
.. |force| replace:: :math:`\vec{F}\left(\vec{r}\right)`
.. |vec_r| replace:: :math:`\vec{r}`
.. |e_field| replace:: :math:`\vec{E}\left(\vec{r}\right)`

This page is intended to walk you through the basics of writing a module. Here
we focus purely on writing the module, not designing it. Because of how
PluginPlay works designing a module can sometimes be non-trivial; tips for
designing a module can be found in :ref:`module_designing`. This page assumes we are
writing a module which computes the electric field, |e_field| of a series of
point charges as detailed in :ref:`module_development`.

Declaring the Module
====================

In PluginPlay modules are implemented by inheriting from the ``ModuleBase``
class and then implementing a constructor and a ``run_`` member. The declaration
amounts to a large amount of boilerplate so PluginPlay provides a macro
``DECLARE_MODULE(T)`` which will declare a class ``T`` satisfying the required
API. Since it will be used as the name of the class, ``T`` must be a valid C
identifier.

Generally speaking modules of the same property type differ by the algorithm
used to compute the property. It thus makes sense to somehow include the
algorithm in the module's name. For the purposes of our electric field module we
will compute the electric field via Coulomb's Law so we choose to name our
module ``CoulombsLaw`` and we store the declaration in a file ``modules.hpp``.
The relevant lines are:

.. literalinclude:: ../../../../tests/cxx/doc_snippets/modules.hpp
   :language: c++
   :lines: 17-22,31-32

.. note::

   Most libraries include more than one module, and given that declaring a
   module requires a single line, it is not uncommon to declare multiple modules
   in a single header file.

.. note::

   Declaring a module is not required if the module is written in Python.

Defining the Module
===================

The definition of our ``CoulombsLaw`` module goes in a source file. Defining the
module amounts to implementing two functions: the default constructor and the
``run_`` member function. The constructor is where all the set-up for your
module occurs; it is also where you can set meta-data associated with your
module. The ``run_`` member function is where your module does the work.

Defining the Constructor
------------------------

PluginPlay provides the macro ``MODULE_CTOR(T)``, where ``T`` is the name of the
module to aid in defining the module's constructor. At a minimum the constructor
must set the property type(s) that the module satisfies. It is also a good idea
to provide a short description about the algorithm implemented in the module.

For our ``CoulombsLaw`` module a basic constructor looks like:

.. tabs::

   .. tab:: C++

      .. literalinclude:: ../../../../tests/cxx/doc_snippets/coulombs_law.cpp
         :language: c++
         :lines: 36-39

   .. tab:: Python

      .. literalinclude:: ../../../../tests/python/doc_snippets/coulombslaw_force.py
         :language: python
         :lines: 20-25

One particularly nice feature of PluginPlay is that it can autogenerate
restructured text documentation for modules by scraping the meta-data and fields
of the module. The description we provided serves as a preface to the resulting
page. Some of the other meta-data that module developers can set in the
constructor includes: a list of citations, the author of the module, and the
version. A full list of meta-data is available (TODO: Add link).


.. note::

   PluginPlay automatically documents input parameters and submodules so there
   is no need to include these in your description.

.. hint::

   The property type will be needed in a few places so it is a good idea to set
   a typedef, this way if you need to change the property type for any reason
   you only need to change it in the typedef.

.. hint::

   The raw string literal introduced in C++11 denoted by
   ``auto str = R"(...)";`` is a great way to write a lengthy description
   without having to use escapes or other workarounds for line endings.


Defining the Run Member
-----------------------

The ``run_`` member of the module takes a set of inputs and a set of submodules,
and returns the property (or properties). Like the module constructor,
PluginPlay provides the macro ``MODULE_RUN(T)``, where ``T`` is the name of the
module to aid in defining the module's ``run_`` member function. By convention
(and hidden in the definition of the macro) the names of the two arguments are:
``inputs`` and ``submods``. Presently we will ignore the ``submods`` argument.

For defining our ``CoulombsLaw::run_`` member the relevant PluginPlay bits of
code are:

.. tabs::

   .. tab:: C++

      .. literalinclude:: ../../../../tests/cxx/doc_snippets/coulombs_law.cpp
         :language: c++
         :lines: 41-45,65-67

   .. tab:: Python

      .. literalinclude:: ../../../../tests/python/doc_snippets/coulombslaw_force.py
         :language: python
         :lines: 27-29,41-42

The inputs to a module are type-erased. Getting them back in a usable typed form
can be done automatically via template meta-programming. The details of this
process are beyond our present scope; for now know that every property type
defines a static function ``unwrap_inputs`` which takes the type-erased inputs
and returns an ``std::tuple`` with the typed inputs (in the above example we
use C++17's structured bindings to automatically unpack the tuple). In a similar
vein the results of every module are also type-erased and every property type
defines a static function ``wrap_results`` which takes the typed results and
returns a result map with the type-erased results.

The full definition of the ``run_`` member (including the source code for
computing the electric field) is:

.. tabs::

   .. tab:: C++

      .. literalinclude:: ../../../../tests/cxx/doc_snippets/coulombs_law.cpp
         :language: c++
         :lines: 41-67

   .. tab:: Python

      .. literalinclude:: ../../../../tests/python/doc_snippets/coulombslaw_force.py
         :language: python
         :lines: 27-42

Submodules
==========

Coulomb's Law is pretty simple. Most simulation techniques are much more
complicated than that and involve an intricate interplay of many different
properties.

As an example say we are writing a module which computes the force of a moving
charged particle at a position |vec_r|, |force|. Assuming the particle
has a mass :math:`m`, its acceleration at |vec_r| is given by |acceleration|,
and that it has a charge :math:`q`, the |force| is given by:

.. math::

   \newcommand{\force}{\vec{F}\left(\vec{r}\right)}
   \newcommand{\acceleration}{\vec{a}\left(\vec{r}\right)}
   \newcommand{\efield}{\vec{E}\left(\vec{r}\right)}

   \force = m\acceleration + q\efield

The important point for this tutorial is that :math:`F\left(\vec{r}\right)` can
be written in terms of the electric field, |e_field|. To capitialize on all the
innovations in computing electric fields, we decide to write our force module in
terms of an electric field submodule.

Submodules change the class's definition. In the constructor we now need to
declare that our module uses a submodule and that the submodule must satisfy the
property type `ElectricField`. This looks like:

.. literalinclude:: ../../../../tests/cxx/doc_snippets/force.cpp
   :language: c++
   :lines: 38-45

PluginPlay also requires that we specify a tag (we choose the tag
``"electric field"``). The tag ensures that the module can specify and
distinguish between multiple submodules of the same type. When appropriately
named, the tag also aids in readability of the code. The ``run_`` function for
our module looks like:

.. literalinclude:: ../../../../tests/cxx/doc_snippets/force.cpp
   :language: c++
   :lines: 47-59

Of note is the line:

.. literalinclude:: ../../../../tests/cxx/doc_snippets/force.cpp
   :language: c++
   :lines: 53

which says we are running the submodule tagged with ``"electric field"`` as an
``efield_type``. We provide the submodule with the point charge's location and
the list of point charges defining the electric field.

Submodules are integral to PluginPlay so it is worth noting:

- By using submodules we establish data dependencies (*i.e.* here we stated we
  need an electric field, we don't care how it's computed)
- If tomorrow someone writes a new electric field module, it can immediately be
  used with our force module without modifying the force module.
- The submodule that is called can be changed at runtime by the user and our
  module doesn't have to maintain the logic to do so (*i.e.* our force module
  doesn't need to maintain a list of all possible submodules and switch between
  them).
- If the calculation crashes after computing the electric field, but before the
  module completes (pretty unlikely given how few operations remain) memoization
  will avoid the need to recompute the electric field when the calculation is
  restarted. The module does not have to maintain separate checkpoint/restart
  logic!!!!!
