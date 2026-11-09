.. Copyright 2026 NWChemEx-Project
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


.. _writing_a_driver:

****************
Writing a Driver
****************

A driver is written like a module (see :ref:`module_development`). Its
constructor declares, on top of what a module's constructor declares, how the
driver configures the modules below it. This page writes ``ForceDriver``,
which computes a force with "Classical Force" and lets the user choose
whether the electric field is screened.

Declaring the Driver
====================

PluginPlay provides the macro ``DECLARE_DRIVER(T)``, the driver counterpart of
``DECLARE_MODULE(T)``. It declares a class ``T`` that derives from
``DriverBase``, which in turn derives from ``ModuleBase``:

.. literalinclude:: ../../../../tests/cxx/doc_snippets/drivers.hpp
   :language: c++
   :lines: 17-22, 24-25

.. note::

   Declaring a driver is not required if the driver is written in Python. In
   Python, drivers derive from ``pluginplay.DriverBase``.

The Driver's Settings
=====================

The constructor is started with ``DRIVER_CTOR(T)``. As for a module, it sets
the property type the driver satisfies, and the driver's inputs and
submodules. ``ForceDriver`` satisfies ``Force``, so that it can be used
anywhere a force module can. Its "force" submodule is the module that does the
work, and its two inputs are the settings the user can change:

.. tabs::

   .. tab:: C++

      .. literalinclude:: ../../../../tests/cxx/doc_snippets/force_driver.cpp
         :language: c++
         :lines: 33-44

   .. tab:: Python

      .. literalinclude:: ../../../../tests/python/doc_snippets/force_driver.py
         :language: python
         :lines: 21-35

The rest of the constructor says how those settings reach the modules below
the driver. A driver has two ways to do that: wiring and conditional wiring.
Each names the modules it configures by their keys in the ``ModuleManager``.

Wiring
------

In a driver's constructor, ``add_input`` and ``add_submodule`` return
wrappers that, besides the usual setters, have
``set_target(module, target)``. It gives the input or submodule ``target`` of
the module ``module`` the value of the driver's input or submodule, whatever
the driver's other settings are. ``FieldChoiceDriver``, a driver that lets
the user pick the electric field directly, declares:

.. tabs::

   .. tab:: C++

      .. literalinclude:: ../../../../tests/cxx/doc_snippets/field_choice_driver.cpp
         :language: c++
         :lines: 30-32
         :dedent: 4

   .. tab:: Python

      .. literalinclude:: ../../../../tests/python/doc_snippets/force_driver.py
         :language: python
         :lines: 61-63
         :dedent: 8

``set_default`` on a submodule names the module used if the user binds none.
``set_target`` can be called more than once, to wire one input or submodule to
several modules. ``set_required_target`` does the same, but makes it an error
for the setting to have no effect (see :ref:`using_a_driver`).

``ForceDriver`` does not use ``set_target``: its "threshold" only matters when
the screened electric field is used, so it is wired in a branch of a chain
instead.

Conditional Wiring
------------------

Sometimes one setting changes several things, picks between fixed values, or
decides whether another setting matters. ``when(input, value)`` starts a
*chain* of branches. The first branch applies if the driver's input ``input``
equals ``value``. ``otherwise(input, value)`` starts another branch with its
own condition, and ``otherwise()`` starts a branch that applies if no other
does. Only the first branch whose condition holds is applied. A branch can
hold:

- fixed values, added with ``set_input(module, input, value)`` and
  ``set_submod(module, submod, choice)``; and
- inputs and submodules of the driver, wired with
  ``forward_input(module, input, driver_input)`` and
  ``forward_submod(module, submod, driver_submod)``, which work like
  ``set_target``, but only when the branch is taken.

``ForceDriver`` picks the electric field with its "screen" input, and passes
its "threshold" on only when the screened field is used:

.. tabs::

   .. tab:: C++

      .. literalinclude:: ../../../../tests/cxx/doc_snippets/force_driver.cpp
         :language: c++
         :lines: 46-52
         :dedent: 4

   .. tab:: Python

      .. literalinclude:: ../../../../tests/python/doc_snippets/force_driver.py
         :language: python
         :lines: 37-45
         :dedent: 8

Because "threshold" is forwarded only in the "screen" branch, it is ignored
quietly when "screen" is false, unless the user changed it, in which case the
user is warned that it has no effect (see :ref:`using_a_driver`).

A chain only sets fixed values and forwards the driver's settings. A choice
that needs more logic than that is better written as its own module or
driver.

One Setting per Target
----------------------

Within a driver, each input or submodule of a module below it is set by one
``set_target`` call, or by one chain, whose branches may all set or forward to
it. Setting it in two places is an error, so a driver never has to
decide which of its own settings wins. Values that a user sets on the driver
(see :ref:`using_a_driver`) are applied after the driver's settings, so they
always win.

Other Settings
--------------

Two more settings are not used by ``ForceDriver``:

- ``wire_memoization(on, except)`` turns memoization on or off for every
  module below the driver, except the modules whose keys are in ``except``.
- A branch of a chain can be marked ``.required()``, as ``set_required_target``
  marks a wired input or submodule. A required setting that would have no
  effect (see :ref:`using_a_driver`) is an error instead of a warning.

The Run Member
==============

The run member is defined with ``DRIVER_RUN(T)``, the driver counterpart of
``MODULE_RUN(T)``. By the time it is called, the driver's settings have been
applied, so ``ForceDriver`` only needs to call its "force" submodule:

.. tabs::

   .. tab:: C++

      .. literalinclude:: ../../../../tests/cxx/doc_snippets/force_driver.cpp
         :language: c++
         :lines: 55-61

   .. tab:: Python

      .. literalinclude:: ../../../../tests/python/doc_snippets/force_driver.py
         :language: python
         :lines: 47-51
         :dedent: 4
