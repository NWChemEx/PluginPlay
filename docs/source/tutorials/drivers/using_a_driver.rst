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


.. _using_a_driver:

**************
Using a Driver
**************

This page uses the ``ForceDriver`` written in :ref:`writing_a_driver`. It
computes the force on a particle with charge 1 at the origin, from a charge of
1 at :math:`x=1` and a charge of 2 at :math:`x=2`.

Adding the Driver
=================

A driver is added to the ``ModuleManager`` like any other module, and its
submodules are set with ``change_submod``. Here the modules the driver
configures come from the ``pluginplay_examples`` plugin (in Python, from
Python versions of the same modules):

.. tabs::

   .. tab:: C++

      .. literalinclude:: ../../../../tests/cxx/doc_snippets/test_drivers.cpp
         :language: c++
         :lines: 33-36
         :dedent: 4

   .. tab:: Python

      .. literalinclude:: ../../../../tests/python/doc_snippets/test_drivers.py
         :language: python
         :lines: 48-56
         :dedent: 8

Running the Driver
==================

A driver is run like any other module. With its default settings,
``ForceDriver`` uses the unscreened electric field, and the force is
:math:`(5.5, 0, 0)`:

.. tabs::

   .. tab:: C++

      .. literalinclude:: ../../../../tests/cxx/doc_snippets/test_drivers.cpp
         :language: c++
         :lines: 38-39
         :dedent: 4

   .. tab:: Python

      .. literalinclude:: ../../../../tests/python/doc_snippets/test_drivers.py
         :language: python
         :lines: 58-59
         :dedent: 8

The first time a driver runs, it is locked, like any module. When a driver is
locked, the ``ModuleManager`` applies the driver's settings to *overlays* of
the modules below it: modules that point to the ``ModuleManager``'s modules
and hold only what the driver changed. The modules themselves are not changed.
Other modules that use "Classical Force", for example, still get the
unscreened electric field. As for any module, the modules below the driver are
locked too.

.. note::

   A driver's settings are applied by the ``ModuleManager``, so a driver must
   be run with ``ModuleManager::run_as`` (or as a submodule of a module that
   was). Running a driver with ``Module::run_as`` before it has run through the
   ``ModuleManager`` throws.

Changing the Settings
=====================

The driver's settings are its inputs and submodules, so they are changed with
``change_input`` and ``change_submod``. Since a driver that has run is locked,
using other settings afterwards means copying the driver first. The copy
starts from the driver's settings before it was locked, and applies them
again when it runs. With screening on and a threshold of 1.5, only the
charge at :math:`x=1` contributes to the electric field, and the force is
:math:`(5, 0, 0)`:

.. tabs::

   .. tab:: C++

      .. literalinclude:: ../../../../tests/cxx/doc_snippets/test_drivers.cpp
         :language: c++
         :lines: 44-49
         :dedent: 4

   .. tab:: Python

      .. literalinclude:: ../../../../tests/python/doc_snippets/test_drivers.py
         :language: python
         :lines: 64-68
         :dedent: 8

Settings With No Effect
=======================

A setting has no effect if the module it targets is not called by the driver,
or if it belongs to a branch of the driver that is not taken. ``ForceDriver``
passes "threshold" on only when "screen" is true. If the user changes
"threshold" but leaves "screen" false, the change does nothing. Such settings
are *inactive*. Inactive settings are logged as warnings when the driver is
locked, and a locked driver's ``inactive_settings()`` lists them:

.. tabs::

   .. tab:: C++

      .. literalinclude:: ../../../../tests/cxx/doc_snippets/test_drivers.cpp
         :language: c++
         :lines: 63-69
         :dedent: 4

   .. tab:: Python

      .. literalinclude:: ../../../../tests/python/doc_snippets/test_drivers.py
         :language: python
         :lines: 79-85
         :dedent: 8

With its default settings, ``ForceDriver`` reports nothing: "threshold" was
not changed, so nothing the user asked for was ignored.

Overriding the Driver's Choices
===============================

Most choices a driver makes are its inputs and submodules, and are changed as
above. For the rest, a user can set an input or submodule of a module below
the driver to a fixed value. The driver's ``call_graph_request()`` holds the
user's requests, and its ``set_input`` and ``set_submod`` work like the ones in
a chain (see :ref:`writing_a_driver`). ``call_graph_request()`` throws if the
module is not a driver. Values the user sets are applied after all of the
driver's settings, so they win.

Here the user picks the screened electric field without turning on "screen".
Since the "screen" branch is not taken, the driver does not pass its
"threshold" on, so the user also sets the screened module's threshold
directly:

.. tabs::

   .. tab:: C++

      .. literalinclude:: ../../../../tests/cxx/doc_snippets/test_drivers.cpp
         :language: c++
         :lines: 76-82
         :dedent: 4

   .. tab:: Python

      .. literalinclude:: ../../../../tests/python/doc_snippets/test_drivers.py
         :language: python
         :lines: 95-102
         :dedent: 8

Like the driver's inputs, these values must be set before the driver is
locked.
