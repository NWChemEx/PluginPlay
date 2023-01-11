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

.. _terminology_and_abbreviations:

#############################
Terminology and Abbreviations
#############################

The purpose of this page is to define abbreviations and terminology used
throughout the PluginPlay documentation. Terms and abbreviations are listed in
alphabetic order. Other pages are encouraged to link to the terms and
abbreviations.

***********
Terminology
***********

.. _bound_input:

Bound Input
===========

For a particular :ref:`module`, ``M``, an input ``k`` is said to be
bound if the ``ModuleInput`` instance returned by ``M.inputs().at("k")`` has
a value. When a :ref:`module` is called as a particular :ref:`property_type`,
all inputs not appearing in the definition of the :ref:`property_type` must be
bound.

.. _bound_submodule:

Bound Submodule
===============

Same as a :ref:`bound_input` except that it refers to one of the
submodules that a module will call instead of one of the input options.

.. _field:

Field
=====

A term for generically referring to the arguments to/from a function, *i.e.*,
both inputs and results are fields.

.. _locked:

Locked
======

A :ref:`module` is said to be locked if the end-user accessible state can no
longer be changed. This occurs when a :ref:`module` is run and prevents
concurrent access from invalidating :ref:`memoization`.

.. _memoization:

Memoization
===========

A technique where repeated calls to an expensive function, with
the same input, are avoided by saving the input to the function, and the
result of calling the function.

.. _module:

Module
======

Self-contained pieces of code. Modules wrap developer provided code
in a common :ref:`api` that can be used by the rest of PluginPlay.

.. _opaque:

Opaque
======

The opposite of :ref:`transparent`. An option is "opaque" if changing its
value changes the result of a :ref:`module`.

.. _plugin:

Plugin
======

These are groups of :ref:`module` that are distributed together. In C++ plugins
will be libraries, whereas in Python they will be Python packages.

.. _property_type:

Property Type
=============

A property type is a domain-specific quantity of interest. For
example if your code computes geometric properties of shapes then you may
have property types for area, volume, and perimeter. Basically these are the
quantities that users of your code will want to compute and they define the
:ref:`api` for calling a :ref:`module` which can compute the namesake property.

.. _strong_type:

Strong Type
===========

In C++ a strong type ``U`` is a class which wraps an object of type ``T`` so
that it has a different type than other objects of type ``T``.  This is mainly
used when arguments to a function have the same type, but different meanings.
For example, consider:

.. code-block:: c++

   void draw_box(float length, float height);

Accidentally passing the height as the length and the length as the height
rotates the box by 90 degrees. Since both arguments are ``float`` there is no
way for the compiler to know you messed up. If we instead do:

.. code-block:: c++

   struct Length{ float value; };
   struct Height{ float value; };

   void draw_box(Length length, Height height);

It becomes much harder for the user to swap the length and height accidentally
because they now formally have different types. It should be noted that most
modern compilers can optimize the additional indirection out so the use of
strong types should have little to no performance overhead.

.. _submodule:

Submodule
=========

A :ref:`module` called from within the body of a :ref:`module`. In most cases
the "sub-" prefix is simply to discern the caller from the callee. The notable
exception is when discussing the ``Submodule`` class, which defines the
interface for how the caller will call the callee.

.. _transparent:

Transparent
===========

An input to a :ref:`module` is "transparent" if the result of the
:ref:`module` does not depend on the value. Most inputs are :ref:`opaque` in
that their values do change the resulting value. Examples of transparent
options include debug printing levels and options that affect the speed at
which an algorithm computes (block sizes, number of threads, *etc.*), but not
the accuracy.


*************
Abbreviations
*************

.. _api:

API
===

Application Programming Interface. The literal functions you call to
programmatically interact with a software package.

.. _pimpl:

PIMPL
=====

Pointer to IMPLementation. A common C++ technique used to separate the
:ref:`api` of the class from its implementation. The :ref:`api` of the class
is declared in an "outer" class. The outer class has a (usually private) member
which is a pointer to an "inner" class. The inner class holds the state and
implementation. Since the outer class holds a pointer to the inner class, it
suffices to forward declare the type of the inner class, in turn avoid the
leakage of details needed to power the inner class.
