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

.. _module_manager_design:

############################
Designing the Module Manager
############################

:ref:`pp_architecture` calls for a module manager component which will be the
user-facing :ref:`api` of PluginPlay. This section describes the design of
that component.

***************************
What is the module manager?
***************************

Based on object-oriented design principles we need an object to represent the
PluginPlay framework. The majority of interactions with the framework can be
summarized as "module management" so we have opted to call the corresponding
component the "module manager" and the literal class ``ModuleManager``.

*****************************
Module Manager Considerations
*****************************

Since the module manager is the user-facing :ref:`api` every major feature of
PluginPlay needs to be accessible through it. This means the module manager
component needs to support:

# Dynamic module management

  - Users may hard-coded some configuration, but ultimately
    users also need to be able to override those configurations at runtime.
  - The state of the framework needs to adapt to changes in real time
  - The state of the framework needs to be query-able  in real time.
  - Responsible for lifetime of modules (and framework)

#. Managing of :ref:`module` pool

   - Need to be able to load, remove, *etc.* :ref:`module` objects
   - Iterate over, list, count, *etc.* loaded :ref:`module` objects
   - Discern among different configurations

#. Modify call graph

   - Change which :ref:`submodule`(s) a :ref:`module` calls
   - Change inputs bound to :ref:`module` objects

#. Run call graph

   - Once the call graph is setup need to actually be able to run it.

***************************
Module Manager Architecture
***************************

.. _fig_mm_arch:

.. figure:: assets/mm_arch.png
   :align: center

   Architecture of the module manager component.
