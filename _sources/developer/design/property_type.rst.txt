.. Copyright 2023 NWChemEx-Project
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

.. _property_type_design:

#####################################
Designing the Property Type Component
#####################################

:ref:`call_graph_design` calls for a property type component. This section
outlines the design decisions that went into that component.

************************************
What is the Property Type Component?
************************************

At the lowest level all modules have the same :ref:`api`: take a series of
type-erased inputs and return a set of type-erased outputs. In strongly typed
languages like C++, type-erased objects are cumbersome to work with. The
property type component facilitates the wrapping/unwrapping of
typed/type-erased objects while also enforcing a standardized :ref:`api`.

****************************
Property Type Considerations
****************************

From :ref:`call_graph_design` we have:

#. Dynamic determine module :ref:`api`

   - Property types will need to wrap/unwrap typed/type-erased data

#. Domain agnostic

   - PluginPlay needs to avoid coupling to the domain-specific types

While not explicitly called for at this point we also want:

3. Avoid exposing templates to the user (to the extent possible)

   - Property types will inevitably use templates, but many C++ programmers in
     computational science are not comfortable with advanced template usage.

#. Use property types to factor out input/result provenance

   - The same property type will be used by multiple modules. Rather than
     duplicating provenance, particularly input/result descriptions, in each
     module, we want to factor this out to the property type.

#. Allow property type inheritance

   - Many properties are related via "is-a-type-of" relationships.
   - Inheritance captures "is-a-type-of" relationships and can be used for
     avoiding duplication

******************************
Property Type Component Design
******************************

.. _fig_property_type_design:

.. figure:: assets/property_type_design.png
   :align: center

   Architecture of the property type component. Users derive from the
   ``PropertyType`` class to implement their property types. The template
   meta-programming needed for dealing with types is factored out into the
   ``FieldTuple`` class.

Fig :numref:`fig_property_type_design` shows the architecture of the property
type component. The ``PropertyType`` class holds the bulk of the implementation.
Users derive their property types from ``PropertyType``, using the curiously-
recursive template pattern (CRTP). CRTP facilitates PluginPlay implementing
features on behalf of the user, without PluginPlay knowing the types. *N.B.*,
normal inheritance would not allow the ``PropertyType`` class to access the
types defined in the derived class. In our implementation ``PropertyType`` is
templated on the derived type and the base type(s). The ``PropertyType`` class
will implement four functions ``wrap_inputs``, ``wrap_results``,
``unwrap_inputs``, and ``unwrap_results`` which can be used to type-erase and
un-type-erase data on behalf of the user.

In the derived class, users fill in two ``FieldTuple`` objects, one for the
inputs and one for the results. The process of doing this is wrapped by
"virtual" functions (since we're using CRTP they're not actually virtual)
``inputs_`` and ``results_`` respectively. The ``FieldTuple`` objects are
responsible for storing not only the types of the inputs, but also the default
values, descriptions, etc. It is the responsibility of the ``FieldTuple``
objects to define as simple of an :ref:`api` as possible.

Summary
=======

Our design addresses the above considerations by:

#. Dynamic determine module :ref:`api`

   - ``unwrap_inputs`` / ``unwrap_results`` and
     ``wrap_inputs`` / ``wrap_results`` functions can be used at runtime to go
     from/to type-erased inputs/results.

#. Domain agnostic

   - CRTP allows the ``PropertyType`` class to access derived class's types
     through "inheritance".
   - Derived classes, and their types, live in downstream code.

#. Avoid exposing templates to the user (to the extent possible)

   - Largely falls to ``FieldTuple`` component.
   - Macros further de-template the :ref:`api`.

#. Use property types to factor out input/result provenance

   - ``FieldTuple``` stores provenance for inputs/results.
   - Can be overridden on a per-module basis.

#. Allow property type inheritance

   - ``PropertyType`` is templated on base property types.
