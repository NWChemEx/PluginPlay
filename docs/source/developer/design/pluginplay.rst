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

####################
Designing PluginPlay
####################

As summarized in :ref:`developing_scientific_software` there is a need to
transition to more modular scientific software. In practice, there are a number
of challenges associated with writing modular software and PluginPlay has been
developed to address them.

*******************
What is PluginPlay?
*******************

PluginPlay is a framework to facilitate writing modular scientific software.
PluginPlay is designed to be domain-agnostic, runtime configurable, and
easily extensible. At its core, PluginPlay manages a call graph whose nodes
are "modules", *i.e.*, self-contained, stateless functions. PluginPlay builds/
discovers the call graph in real time, allowing the call graph to be manipulated
at runtime and programmatically. We term the discovery and manipulation of the
call graph "call graph reflection". While call graph reflection was largely
intended to facilitate "hot-swapping" modules, it also provides a convenient
mechanism for automating many software maintenance tasks, albeit at a modular
level.


********************************
PluginPlay Design Considerations
********************************

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

***********************
PluginPlay Alternatives
***********************

.. warning::

   The developers of PluginPlay have limited to no experience actually using
   many of the packages in this section. Notes and impressions stem
   from perusing documentation and source code and may not actually reflect
   the true state of the packages.

Based on the distilled considerations from the previous section we have
considered a number of existing C++ frameworks. Admittedly the search space
of existing frameworks is very large and we have limited our search to
frameworks which do not assume networking. This is because most networking
applications assume a large communication latency between components, whereas
we are interested in frameworks where component communication latency is on
the order of a function call. We also have not considered entity-component
frameworks; these frameworks typically target game design where components
are instance-like.

Inversion of Control Frameworks
===============================

Frameworks satisfying our distilled considerations tend to rely on
inversion of control (IoC) and dependency injection. We are aware of the
following C++ IoC frameworks.

Autowiring
----------

- `GitHub <https://github.com/leapmotion/autowiring>`__
- Last updated 2018
- 68 watchers and 145 stars

Autowiring :cite:`autowiring` composes a dynamic call graph in terms of
``Context`` objects. Beyond this the documentation is a bit sparse and hard to
follow, but it appears that users write ``AutoFilter`` instances to define
type-safe APIs for interacting with the ``Context`` objects. The project
appears to now be abandoned.

ioc
---

- `GitHub <https://github.com/unixdev0/ioc>`__
- Last commit 2017
- 2 watchers and 2 stars.

ioc :cite:`ioc` was intended to be a header-only framework for writing C++
applications using the inversion of control design pattern. The project looks
like it was abandoned just after getting started.

ioc-cpp
-------

- `GitHub <https://github.com/mrts/ioc-cpp>`__
- Last commit 2014
- 2 watchers and 0 stars

ioc-cpp :cite:`ioc_cpp` provides an inversion of control/dependency injection
container. In lieu of documentation a simple example is provided. Based on the
example it seems that users register parent-child relationships with the
framework. Later when the user wants an object derived from the child class,
the user gets it from the framework. While ioc-cpp does indeed have the
potential to be used as part of an inversion of control framework, it appears
to be lacking many features a full featured framework would need (such as
the ability to modify what dependency is injected). Development seems to have
stopped in 2014 and the project is now abandoned.

Pococapsule
-----------

- `Google Code <https://code.google.com/archive/p/pococapsule>`__
- `GitHub <https://github.com/onioncong/pococapsule>`__
- GitHub last updated 2008
- 1 watcher and 0 stars

Pococapsule :cite:`pococapsule` appears to have originally been developed at
Google Code and then transitioned to GitHub. Based on the Google Code repo's
description, Pococapsule defines an inversion-of-control container that supports
dependency injection. A "dynamic proxy engine" is used for type-safe invocations
of the containers and "dynamic wiring" is used to connect the containers.
Documentation is lacking and ultimately development never seemed to take off
at GitHub, which would make resurrecting this project very difficult.

Computational Chemistry Component-Based Frameworks
==================================================

PluginPlay was motivated by the needs of NWChemEx, which is a scientific
research software package targeting electronic structure theory. As such,
one could reasonably argue that we need not code up a general component-based
solution, if a domain-specific solution exists and works. Point being, we'd be
remiss if we didn't also mention the work aimed at designing computational
chemistry specific solutions for modular software.

.. note::

   Historically computational chemists have been very loose with their
   software terminology. So while many packages boast a modular design, a lot
   of times this modularity amounts to the fact that the code uses functions
   and/or libraries. For our purposes, we consider something modular if it can
   reasonably stand alone. Notably we do not require interoperability in order
   for something to be modular.

Atomic Simulation Environment (ASE)
-----------------------------------

- `GitLab <https://gitlab.com/ase/ase>`__
- Active development
- 342 stars

ASE :cite:`ase` is designed with the goal of being a unified framework for
setting up, steering, and analyzing atomistic simulations. ASE provides a
number of modules, which serve as hooks where external components can be
hooked in. For example the ``calculator`` module defines the API for
computing energies and forces on a molecule. ASE notably also allows users to
write observers, which are called at pre-defined points in the module's
execution, thus facilitating even more extension points.

ASE is ultimately targeted at driving other computational chemistry packages.
From the perspective of developing a computational chemistry package, ASE is
thus a workflow tool. The hooks ASE provides are too high-level for our
purposes and ASE does not seem to provide any mechanism for adding more hooks.


Common Component Architecture (CCA)
-----------------------------------

The CCA :cite:`cca` was one of the first attempts at designing a component-based
framework for writing modular scientific software. Components had standardized
APIs and relatively specific tasks (e.g., compute integrals, or diagonalize a
matrix). Notably, CCA did not only target computational chemistry (although it
had a computational chemistry sub-community). Ultimately, the CCA was abandoned
because it was too cumbersome for developers.

Massively Parallel Quantum Chemistry (MPQC)
-------------------------------------------

- `GitHub <https://github.com/ValeevGroup/mpqc>`__
- Active development (although most occurs in a private fork)
- 27 watchers and 56 stars

Already in MPQC's original design :cite:`mpqc` you could see how MPQC could be
used as a modular framework for quantum chemistry. More specifically, the class
hierarchy took care to define abstract APIs for most algorithms, derived classes
implemented the algorithms; hence the base classes provided hooks for extending
the functionality in an encapsulated manner. MPQC's modern state :cite:`mpqc4`
includes support for plugin-based development. Developers wanting to use this
feature compile MPQC as a library, extend an MPQC class in their plugin, and
link MPQC and their plugin to the driver.

At present there does not seem to be support for dynamically discovering
plugins, nor is there a way to extend the set of hooks without modifying MPQC.


Psi4
----

- `Github <https://github.com/psi4/psi4/>`__
- Active development
- 61 watchers and 754 stars

.. note::

   I'm not sure what the official capitalization of Psi3/Psi4 is. It seems to
   be listed as Psi3/Psi4 in some places, *e.g.*, :cite:t:`psi4_11`
   (*N.B.* that small caps would have all letters the same size if they were
   all the same case) and PSI3/PSI4 in others, *e.g.*, :cite:t:`psi4_14`.

Psi3 :cite:`psi3`, contained a number of individual programs (termed modules),
each designed to perform a common task. These modules used disk to communicate
and a driver program to wire them together. This architecture made it difficult
to transition to high-performance computing, and was the motivation for the
Psi4 :cite:`psi4` rewrite. In it's initial incarnation, Psi4 was a single
executable with a Python front-end. From the initial publication :cite:`psi4`
it is not clear what plugin/module features it had, but as of the 1.1 release
of Psi4 :cite:`psi4_11`, Psi4's ability to support plugins and external
projects had been noted.


As of the 1.1. version of Psi4, Psi4's modular capabilities were relatively
limited. In particular, Psi4 version 1.1. allowed users to write modules that
satisfied one of a small set of high-level APIs (e.g. the ability to compute an
energy). While :cite:t:`psi4_11` lists a number of external projects which
interface with Psi4, most of these projects are better categorized as optional
dependencies because they interface through dependency-specific channels (as
opposed to more general APIs meant for extending Psi4). As of version 1.4
:cite:`psi4_14`, modularity of Psi4 relies on QCArchive and thus Psi4's module
capabilities are tied to the QCArchive project.

PySCF
-----

- `GitHub <https://github.com/pyscf/pyscf>`__
- Active development
- 75 watchers and 826 stars

PySCF :cite:`pyscf` is one of the few electronic structure packages to be
written almost entirely in Python. Compared to compiled packages, this makes
it remarkably easier to "hack" PySCF to do what you want. Nonetheless, PySCF
does include some features to facilitate modular extensions. First is the
custom Hamiltonian feature which allows the user, at runtime, to switch out
the Hamiltonian used by PySCF. The next is the streaming/functional nature of
PySCF. To over simplify, PySCF works by having each feature take in an initial
state, mutate the state, and then return the mutated state. Thus by nesting
function calls an entire series of features can be applied to an initial
input. Users are free to write their own functions compatible with this API
and to inject them into the nesting.

PySCF's component framework seems to primarily be targeted at high-level
electronic structure operations. This means it's easy to to modify and add
components when they deal with inputs/outputs close to the user. As far as
we can tell, there are no hooks beyond the custom Hamiltonian and the streaming
syntax, nor is there a way to add more hooks without modifying PySCF.
Furthermore, both extension locations are very chemistry-based making it hard
to, for example, change the numeric linear algebra used under the hood.

Summary
=======

In our opinion the highly multi-disciplinary nature of computational chemistry
means that we need the ability to leverage developments and libraries from
not just within computational chemistry, but from other scientific fields as
well. The fact that all existing computational chemistry frameworks assume
the domain of chemistry is thus a non-starter. With respect to the more generic
existing C++ plugin frameworks

*****************
PluginPlay Design
*****************

With
