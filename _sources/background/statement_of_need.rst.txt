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

.. _pp_statement_of_need:

############################
PluginPlay Statement of Need
############################

As summarized in :ref:`developing_scientific_software` there is a need to
transition to more modular scientific software. For our purposes a modular
piece of code:

- is created, maintained, and distributed independently of other modules.
- fully encapsulates its dependencies.
- has well-defined, stable APIs, both internally and externally.
- is reusable by other packages as is.

Unfortunately, simply having a heap of modules does not equate to having
research capable software. This is because you still need to "wire" the modules
together to form a program. :numref:`fig_pp_wire_modules` illustrate this
point pictorially.

.. _fig_pp_wire_modules:

.. figure:: assets/modules2program.png
   :align: center

   Modules by themselves do not form a program. A program results when the
   modules have been wired together.

Generally speaking, to go from a set of modules to a program there are two
options: write a driver/workflow to call the modules, or load the modules
into a framework and have the framework assemble the program. In the driver/
workflow option the set of modules is typically fixed and the calls are
hard-coded. In turn adding more modules, or changing which module calls which,
requires modifying the driver/workflow. A framework contains a template for
the program and dynamically fills in the details from the available modules.
A good framework also makes it easy to add more modules or change which modules
call which modules.

With the points from :ref:`what_sets_scientific_software_apart` in mind we
argue that to successfully leverage scientific modules we need a framework
capable of:

1. Performance

   - Scientific algorithms are computationally expensive.
   - Framework needs to avoid inhibiting module performance.
   - If the framework is performant, performance concerns fall to the module
     implementations.
   - Suggests that the bulk of the framework should be in C++

#. Modular/Component-Based Architecture

   - Program's call graph assembled by wiring modules together.
   - Facilitates refactoring on a module basis, and in turn makes it easier to:
     retire modules, add new modules, and rewire the call graph.
   - Refactoring can be used as a means of rapid prototyping.
   - Encapsulation avoids leaking complicated details or large amounts of
     technical debt (the latter may occur as a result of rapid prototyping or
     different software development capabilities).
   - Encapsulation isolates developers from what everyone else is doing.
   - Python :ref:`api` facilitates manipulating the modules

#. Dynamic Module Interfaces

   - What we want to compute changes as research evolves.
   - Interfacing to software from new disciplines often requires new interfaces
   - Python is the *de facto* glue language of choice.

The need for the core framework to be written in C++, and the desire to support
Python bindings, are fundamental design choices affecting all of PluginPlay.
For this reason we do not continue to explicitly propagate them through the
design documentation. However, when their assumption raises explicit concerns
we will remind the reader of the language choices.
