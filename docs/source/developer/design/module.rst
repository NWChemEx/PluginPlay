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

.. _module_design:

##############################
Designing the Module Component
##############################

The :ref:`call_graph_design` section motivated the need for a module component
(although the fact that we've written "module" like a thousand times by now
ought to have suggested it too...).

*****************************
What is the module component?
*****************************

See :ref:`module` for PluginPlay's definition of a module. The module component
is primarily responsible for interfacing PluginPlay to the algorithm the
module developer wrote.

*******************************
Module Component Considerations
*******************************

Based on the discussions in the :ref:`call_graph_design` section, the module
component must:

#. Provide a mechanism for a user to interface their algorithm with PluginPlay

#. Leverage cache component for memoization

#. Have a setup phase (presumably in the constructor)

#. Expose a ``run`` member for actually running the module

#. Have the ability to store call-back points.

***********************
Module Component Design
***********************

.. _fig_module_design:

.. figure:: assets/module_design.png
   :align: center

   Module component design showing the contact points. Users of the
   module go through the ``Module`` class :ref:`api`. PluginPlay interacts with
   the user of the module via the ``ModulePIMPL`` class and the module developer
   via the ``ModuleBase`` class. Module developers interact with the component
   by deriving from ``ModuleBase``.


The design of the module component is shown in Fig :numref:`fig_module_design`.
The subsections below go over the major pieces in more detail.

Module Development
==================

Following traditional object-oriented practices module developers implement
modules by deriving from the ``ModuleBase`` class. In the constructor of their
module, module developers set the property type(s) their module satisfies,
any additional inputs/results (beyond those specified by the property type),
callback hooks used throughout, and the metadata (version, author, papers to
cite, *etc.*). The actual state provided in the constructor is stored in the
``ModuleBase`` part of the object and accessible via protected accessors.

The other half of implementing a module is done when the module developer
overrides the ``run_()`` member. This member is assumed to be a pure function
(a pure function always returns the same results for the same inputs, and has
no side-effects). PluginPlay helps enforce this assumption by making the
``run_()`` member ``const``. The need for a pure function is brought on by the
desired black-box nature and for memoization purposes.
