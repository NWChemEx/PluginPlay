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

In designing PluginPlay we have noted the following considerations:

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

   - Scientific research is by its nature is highly uncertain. Promising
     avenues may not pan out. Funding sources dry up. New hot topics emerge.
   - Workflows are constantly subject to optimization and thus change
   - Users may come up with use cases beyond the original scope
   - Research can lead to discovering new properties, software needs to be
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


***********************
PluginPlay Alternatives
***********************

In computer engineering terms, PluginPlay is an inversion-of-control framework
which relies on dependency injection to build the call graph (*vide infra*).

.. warning::

   The developers of PluginPlay have limited to no experience actually using
   many of the packages in this section. Notes and impressions stem
   from perusing documentation and source code and may not actually reflect
   the true state of the packages.

Inversion of Control Frameworks
===============================

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


Computational Chemistry Specific Frameworks
===========================================

Admittedly PluginPlay was motivated by the needs of NWChemEx, which is
scientific research software targeting electronic structure theory. As such,
one could reasonably argue that we need not code up a general solution, if a
domain-specific solution exists and works. Point being, we'd be remiss if we
didn't also mention the on-going work aimed at designing modular and/or
plugin-based computational chemistry software.

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
