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
transition to more modular scientific software. In practice, there are a number
of challenges associated with writing modular software and PluginPlay has been
developed to address them.

In designing PluginPlay we have noted the following considerations pertaining
to writing scientific software in general:

1. Performance

   - Scientific software is among the most computationally expensive software
     in the world. For performance critical algorithms, even a small
     degradation in performance can result in a simulation becoming intractable.
   - For better or for worse, most high-performance computing relies on C/C++.
   - Performance is heavily tied to hardware; as hardware evolves so will the
     scientific software.

#. Scientific motivation

   - Software is a means to an end and usually developed by the scientists
     themselves.
   - Benefit to cost ratio of dependencies must be large, i.e., dependencies
     usually only considered if they save a lot of time, or are very performant.
   - Ideally automate as much software development as possible.

#. Dynamic nature of scientific research

   - Scientific research is by its nature highly uncertain. Promising
     avenues may not pan out. Funding sources dry up. New hot topics emerge.
   - Workflows vary widely among researchers
   - Users may come up with use cases beyond the original scope
   - Research leads to new quantities of interest software needs to be
     extensible to support these new properties.
   - New algorithms for computing a property emerge. Need to be able to use
     these algorithms throughout the code.

#. Complex nature of scientific research

   - Scientific simulations of real world phenomenon have many pieces.
   - Domains are often hard to grasp for non-experts.
   - Often multi-disciplinary.

#. Need for rapid prototyping

   - Design space for most scientific algorithms is huge. Need to be able to
     quickly scan this space.
   - Python is at present the *de facto* language of choice for rapid
     prototyping

#. Decentralized scientific software development

   - Developers are typically spread out across the world. Makes synchronizing
     difficult.
   - Entire range of software engineering capabilities. Quality of contributions
     and software products varies widely.

Generally speaking these considerations can be distilled down to the more
general considerations:

1. Performance

   - Attempt to avoid making design decisions which will limit performance, at
     least at the framework-level.
   - Assuming the framework is performant, the remaining performance concerns
     can be punted to the module implementations (see next point)

#. Modular/Component-Based Architecture

   - Leads to "defensive programming" where each piece is encapsulated and
     final program is obtained by "wiring" modules together
   - Makes refactoring easy, which in turn facilitates retiring modules, adding
     new modules, and rewiring the software. Also helps with rapid prototyping.
   - Encapsulation avoids leaking complicated details or large amounts of
     technical debt (the latter may occur as a result of rapid prototyping or
     different software development capabilities).
   - Encapsulation isolates developers from what everyone else is doing.

#. Dynamic Module Interfaces

   - What we want to compute changes as research evolves.
   - Interfacing to software from new disciplines often requires new interfaces
