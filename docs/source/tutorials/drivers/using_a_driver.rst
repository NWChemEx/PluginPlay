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

.. note::

   Drivers are a planned feature. The APIs on this page are planned APIs that
   are not yet implemented, and the code is not yet tested. See
   :ref:`driver_design` for the design.

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

      .. code-block:: c++

         pluginplay::ModuleManager mm;
         mm.load_plugin<PluginplayExamples>();
         mm.add_module<ForceDriver>("Force driver");
         mm.change_submod("Force driver", "force", "Classical Force");

   .. tab:: Python

      .. code-block:: python

         mm = pp.ModuleManager()
         mm.add_module("Coulomb's Law", clf.CoulombsLaw())
         mm.add_module(
             "Coulomb's Law with screening", clf.ScreenedCoulombsLaw()
         )
         mm.add_module("Classical Force", clf.ClassicalForce())
         mm.change_submod("Classical Force", "electric field", "Coulomb's Law")
         mm.add_module("Force driver", fd.ForceDriver())
         mm.change_submod("Force driver", "force", "Classical Force")

Running the Driver
==================

A driver is run like any other module. With its default settings,
``ForceDriver`` uses the unscreened electric field, and the force is
:math:`(5.5, 0, 0)`:

.. tabs::

   .. tab:: C++

      .. code-block:: c++

         // Run with the default settings
         auto F = mm.run_as<Force>("Force driver", q, m, a, charges);

   .. tab:: Python

      .. code-block:: python

         # Run with the default settings
         F = mm.run_as(pt, "Force driver", q, m, a, charges)

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
   was). Running a driver that has never run with ``Module::run_as`` throws.

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

      .. code-block:: c++

         // Use other settings by copying the driver
         mm.copy_module("Force driver", "Screened force driver");
         mm.change_input("Screened force driver", "screen", true);
         mm.change_input("Screened force driver", "threshold", 1.5);
         auto F_screened =
           mm.run_as<Force>("Screened force driver", q, m, a, charges);

   .. tab:: Python

      .. code-block:: python

         # Use other settings by copying the driver
         mm.copy_module("Force driver", "Screened force driver")
         mm.change_input("Screened force driver", "screen", True)
         mm.change_input("Screened force driver", "threshold", 1.5)
         F_screened = mm.run_as(pt, "Screened force driver", q, m, a, charges)

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

      .. code-block:: c++

         // Change "threshold" without turning on "screen"
         mm.copy_module("Force driver", "Unscreened force driver");
         mm.change_input("Unscreened force driver", "threshold", 1.5);
         mm.run_as<Force>("Unscreened force driver", q, m, a, charges);

         auto inactive = mm.at("Unscreened force driver").inactive_settings();
         // "threshold" is inactive: the branch that forwards it was not taken

   .. tab:: Python

      .. code-block:: python

         # Change "threshold" without turning on "screen"
         mm.copy_module("Force driver", "Unscreened force driver")
         mm.change_input("Unscreened force driver", "threshold", 1.5)
         mm.run_as(pt, "Unscreened force driver", q, m, a, charges)

         inactive = mm.at("Unscreened force driver").inactive_settings()
         # "threshold" is inactive: the branch that forwards it was not taken

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

      .. code-block:: c++

         // Override one of the driver's choices
         mm.copy_module("Force driver", "My force driver");
         auto& request = mm.at("My force driver").call_graph_request();
         request.set_submod("Classical Force", "electric field",
                            "Coulomb's Law with screening");
         request.set_input("Coulomb's Law with screening", "threshold", 1.5);
         auto F_mine = mm.run_as<Force>("My force driver", q, m, a, charges);

   .. tab:: Python

      .. code-block:: python

         # Override one of the driver's choices
         mm.copy_module("Force driver", "My force driver")
         request = mm.at("My force driver").call_graph_request()
         request.set_submod(
             "Classical Force", "electric field", "Coulomb's Law with screening"
         )
         request.set_input("Coulomb's Law with screening", "threshold", 1.5)
         F_mine = mm.run_as(pt, "My force driver", q, m, a, charges)

Like the driver's inputs, these values must be set before the driver is
locked.
