Simulation Development Environment (SDE)
=========================================

Computational science is a constantly evolving field. New numeric techniques
are always being developed, hardware is a moving target, and no two research
problems are the same. Designing a monolithic package that is capable of
adapting to the dynamic nature of computational science is a difficult task.
The situation is not unlike mobile app development where developers need to
rapidly adopt to new platforms, stay abreast with current trends, and be able to
integrate with other commonly used apps. Generally speaking most mobile
developers only need to partially worry about these tasks. That is because they
have a framework on which to rely.

The SDE aims to provide numerical science with a framework akin to one used in
mobile app development. Specifically developers are encouraged to develop self-
contained modules, each of which have the granularity of a function. Each module
is designed to perform one task and calls other modules for any subtasks it may
encounter. Which submodule is called for which subtask is managed by the SDE,
thereby decoupling the calling module from the callee. Interoperability of
modules is guaranteed by community standardized APIs.

This introduction was only meant to whet your appetite. Many more details
pertaining to using the SDE, including tutorials can be found in the
documentation.

Building SDE
------------

SDE uses [CMakePP](https://github.com/CMakePP/CMakePackagingProject) for its
build system. Full details on how to build SDE can be found in the SDE's
documentation, which is available [here](add link to read the docs when up).
Alternatively, please see the `docs` directory of this repository for
instructions on building the documentation.


