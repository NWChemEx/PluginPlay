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


.. _driver_tutorials:

#######
Drivers
#######

.. note::

   Drivers are a planned feature. The APIs on this page are planned APIs that
   are not yet implemented, and the code is not yet tested. See
   :ref:`driver_design` for the design.

Running a calculation usually means configuring more than one module. For
example, to compute the force on a charged particle (see
:ref:`module_development`) with a screened electric field, a user must:

- bind the "electric field" submodule of the "Classical Force" module to the
  "Coulomb's Law with screening" module, and
- set the "threshold" input of the "Coulomb's Law with screening" module.

The user has to know which modules to change, and which of their inputs and
submodules. If they forget one, *e.g.*, they set the threshold but not the
submodule, the setting silently has no effect.

A *driver* collects these choices in one place. A driver is a module whose
inputs and submodules are the settings of a whole calculation. When the driver
runs, it applies those settings to the modules below it. The user configures
the driver with the same ``change_input`` and ``change_submod`` calls they use
for any other module.

This part of the tutorials writes a driver, ``ForceDriver``, for the force
calculation above, and then shows how to use it. The design of drivers is
covered in :doc:`/developer/design/driver`.

.. toctree::
   :maxdepth: 2
   :caption: Contents:

   writing_a_driver
   using_a_driver
