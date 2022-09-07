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

*********************************
Basics of Writing a Property Type
*********************************

A property type of type ``T`` is ultimately a class that inherits from
``pluginplay::PropertyType<T>``. The property type defines the API for computing
the associated property. This API has two parts: the positional input arguments
and the returned value (multiple returns are possible, but rarely needed).

Declaring A Property Type
=========================

Like declaring a module, declaring a property type involves a lot of
boilerplate. For this reason we have provided macros to faciliate the process.
To declare our ``ElectricField`` property type the relevant code is:

.. literalinclude:: ../../../tests/pluginplay/docs/electric_field.hpp
   :language: c++
   :lines: 1,3,5,7,22

Unlike modules, property types are usually defined in the header file (this is
because these macros hide a significant amount of template meta-programming).
Thus it is best practice to declare one property type per header file as opposed
to modules where it is not uncommon to declare several modules in one header
file.

Defining a Property Type
========================


.. warning::

   Property types use, and are used in, a lot of template meta-programming. In
   an attempt to make life easy for developers we have hidden all of this from
   the user. As long as you follow the pattern laid out in this subsection you
   can remain ignorant to how we did this for you. If you want to deviate from
   this pattern you likely will have to do quite a bit of template
   meta-programming on your own.

Defining the Input Positional Arguments
---------------------------------------

Defining a property type involves defining two functions: ``inputs_()`` and
``results_()``. Respectively these functions establish the positional input
arguments (and their types) for computing the property and how the property is
represented in code. Again we have provided macros to take care of the
boilerplate.

The definition of the ``inputs_()`` function looks like:

.. literalinclude:: ../../../tests/pluginplay/docs/electric_field.hpp
   :language: c++
   :lines: 9-22

As mentioned at the beginning of this chapter we are only concerning ourselves
with the scenario in which we want the value of the electric field at a single
point and we further assume that the electric field entirely arises from a
series of point charges. We thus need two inputs: where we are evaluating the
electric field, and the set of point charges. In these tutorials we are modeling
vectors, like positions, via the ``Point`` class so our first field is of type
``Point``. Point charges are being modeled via the ``PointCharge`` class and we
opt to have our second argument be an ``std::vector<PointCharge>`` instance. In
both cases we only need to read the value of the inputs and we opt to take the
inputs via constant references.

.. note::

   Inputs in PluginPlay can not be used for returns and must be either by value
   or by constant reference.

After agreeing on the types we declare the fields. This involves calling
``pluginplay::delare_input()`` and then chaining to the result two calls to
``add_field``. ``add_field`` takes one type parameter (the type of the field)
and a string which will be used as the name of the field. You can think of:

.. literalinclude:: ../../../tests/pluginplay/docs/electric_field.hpp
   :language: c++
   :lines: 13-15

as declaring a function ``ElectricField`` with the signature
``ElectricField(const Point& r, const std::vector<PointCharge>& charges)`` (the
type of the return will be set via the ``results_()`` function).

.. note::

   The need for the ``template`` keyword in the second (and all subsequent)
   ``add_field`` call is a quirk of the C++ language and can not be avoided.


The input fields we just declared are actually stored in an object with a
(potentially) nasty type (we let the compiler worry about the type by using
``auto``).

Once we have declared all inputs we can set the meta-data associated with those
inputs (in this case we set their descriptions; a full list of available
meta-data is located at (TODO: Add link)). For this all to work right it is
important to declare the fields first and then set the meta-data. The meta-data
is used in the automatically generated documentation.

Defining the Result
-------------------

The definition of the ``results_()`` function looks very similar to
``inputs_()`` and follows the same rules except that we use
``pluginplay::declare_result`` instead of ``pluginplay::declare_inputs``. The
definition is:

.. literalinclude:: ../../../tests/pluginplay/docs/electric_field.hpp
   :language: c++
   :lines: 24-26

The electric field at a point is simply a vector and we have been modeling
vectors with the ``Point`` class. Thus we establish that the return of our
``ElectricField`` property type is a ``Point`` instance. Since most properties
are a single object (*i.e.*, the electric field, the force, *etc.*) property
types typically only have a single return values. However, like the inputs we
could change multiple ``add_field`` calls together to create additional return
values (the actual return would be an ``std::tuple`` with those values since
strictly speaking C++ does not support multiple return types; the distinction is
immaterial since C++17 where structured bindings allow us to effectively have
multiple returns). Akin to the inputs, we also could have assigned a more
detailed description to the return value if we wanted to; for better or worse,
we decided that the tag ``"electric field at r"`` was descriptive enough and
forwent a more detailed description.

Summary
=======

Generally speaking for both the inputs and results follow this pattern when
defining the function:

- call ``pluginplay::declare_input()``/``pluginplay::declare_result()`` as
  appropriate
- chain to that call a call to ``add_field<T>(std::string)`` to create a field
  that is of type ``T`` and is tagged with the provided string.
- repeat the previous step for each additional field (making sure to include
  the ``template`` keyword before subsequent ``add_field`` calls; this is a
  quirk of the C++ language).
- Assign the result of the aforementioned calls to an object whose type is
  deduced by the compiler via the ``auto`` keyword (trust us, you don't want to
  see it/work it out).
- Working with the aforementioned object continuing setting meta-data such as
  descriptions and default values.
- Return the object

The full declaration and definition of the ``ElectricField`` property type is
shown below:

.. literalinclude:: ../../../tests/pluginplay/docs/electric_field.hpp
   :language: c++
