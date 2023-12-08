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

.. _installing_pluginplay:

#####################
Installing PluginPlay
#####################

PluginPlay uses `CMaize <https://cmakepp.github.io/CMaize/index.html>`__ as
its build system, more detailed build instructions can be found
`here <https://cmakepp.github.io/CMaize/getting_started/building/index.html>`__.

.. note::

   We strongly recommend users use CMake toolchains to pass arguments to the
   ``cmake`` command. Typically CMake toolchains live in files named
   ``<prefix>-toolchain.cmake``, where ``<prefix>-`` is an optional prefix to
   distinguish among toolchains. The contents of the CMake toolchain file
   usually just sets configuration values set via the standard CMake ``set``
   function, *i.e.*, ``set(<option_name> <value>)``.

***************
Install Command
***************

In many cases PluginPlay can be installed by:

.. code-block:: console

   git clone https://github.com/NWChemEx/PluginPlay
   cd PluginPlay
   cmake -H. -Bbuild -DCMAKE_TOOLCHAIN_FILE=<path/to/toolchain/file> \
                     -DCMAKE_INSTALL_PREFIX=<where/to/install/PluginPlay>
   cmake --build build --target install

Where ``<path/to/toolchain/file>`` should be replaced by the path to your
toolchain file, and ``<where/to/install/PluginPlay>`` should be replaced
with where you want to put the installed library (absolute path is recommended
in both cases).

.. note::

   The above command will build serially. To build PluginPlay with ``N``
   processors additionally pass ``--parallel <N>`` to the last command in the
   above code block, *i.e.*,

   .. code-block:: console

      cmake --build build --parallel <N> --target install

*********************
Configuration Options
*********************

This is a list of configuration options recognized by PluginPlay's build
system.

``BUILD_TESTING``.
   Off by default. Set to a truth-y value to enable testing.
``BUILD_DOCS``.
   Off by default. Set to a truth-y value to build the C++ API documentation.
   This variable is defined by the ``nwx_cxx_api_docs`` CMake module.
``ONLY_BUILD_DOCS``.
   Off by default. If ``BUILD_DOCS`` is set to a truth-y value and
   ``ONLY_BUILD_DOCS`` is also set to a truth-y value, then the configure
   process will skip all other aspects of the configuration aside from creating
   the ``PluginPlay_cxx_api`` target. This variable is defined by the
   ``nwx_cxx_api_docs`` CMake module.
``BUILD_PYBINDINGS``.
  On by default. When enabled the optional Python API is built.


*************************
PluginPlay Dependencies
*************************

If you have a problem building PluginPlay it's most likely because you
don't have a new enough version of CMake installed or because CMake can't find
one of PluginPlay's dependencies. This section enumerates PluginPlay's
dependencies.

Required Dependencies
=====================

These are dependencies which must be pre-installed and can not be built by
PluginPlay's build system.

C++ Compiler
------------

PluginPlay relies on the C++17 standard and should work with any C++17
compliant compiler (GCC 9.x or newer). C++ compilers can often be installed by
your OS's package manager. CMake detects your C++ compiler via the value of
``CMAKE_CXX_COMPILER``. So ensure ``CMAKE_CXX_COMPILER`` is set to the C++
compiler you want to use.

MPI
---

PluginPlay's heaviest dependency is ParallelZone which implicitly brings in
MPI (the message passing interface). Users must have an MPI distribution
installed prior to building ParallelZone. MPI distributions exist in most
major OS-level package managers, so check your package manager before
installing MPI from source.

While CMake can find MPI if it is installed in your OS's standard spot, you may
need to give CMake a hint in some cases. MPI provides compiler wrappers and
it's best to let CMake discover MPI via the C++ wrapper. Add:

.. code-block:: cmake

   set(MPI_CXX_COMPILER <path/to/mpicxx>)

to your toolchain to point CMake to MPI's C++ compiler wrapper.


Optional Dependencies
=====================

These are dependencies that PluginPlay's build system can not build; however,
they are only required if certain features are enabled.

Cppyy
-----

URL: `<https://github.com/wlav/cppyy>`__

If Python bindings are enabled (controlled by the CMake variable
``BUILD_PYBINDINGS``, which is ``ON`` by default).  Cppyy is obtainable via
PyPI (*i.e.*, the ``pip`` command). If Python bindings are desired,
the python executable found by CMake must have Cppyy installed.


Doxygen
-------

URL: `<https://www.doxygen.nl/>`__

Used to generate the C++ API documentation. Only needed if ``BUILD_DOCS`` is
set to a truth-y value.

RocksDB
-------

URL: `<https://github.com/facebook/rocksdb>`__

If ``BUILD_ROCKSDB`` is enabled (default is ``OFF``) an installed version of
RocksDB must be locatable by CMaize. RocksDB is used as an optimized backend
for the cache.

Other Dependencies
==================

The dependencies in this section can be built by PluginPlay's build system
when they are not located. Under normal circumstances users can ignore them.
They are listed here primarily for completeness.

Catch2
------

URL: `<https://github.com/catchorg/Catch2>`__

Used for unit testing. Only needed if unit testing is enabled (controlled by
the CMake variable ``BUILD_TESTING``, which is ``OFF`` by default).

Cereal
------

URL: `<https://github.com/USCiLab/cereal>`__

Used for serialization. The build system can automatically build Cereal for
you if it's not found.

CMaize
------

URL: `<https://cmakepp.github.io/CMaize/index.html>`__

Used to simplify writing a CMake-based build system. The build system will grab
it for you.


libfort
-------

URL: `<https://www.github.com/seleznevae/libfort>`__

Used to create formatted tables in auto-generated module documentation.

ParallelZone
------------

URL: `<https://www.github.com/>`

Parallel runtime system built and maintained by the NWChemEx team. As a
slight caveat, PluginPlay's build system can only automatically build
ParallelZone if MPI is installed and visible to CMake.

utilities
---------

URL: `<https://www.github.com/NWChemEx/utilities>`__

Utility classes used throughout the NWChemEx stack.
