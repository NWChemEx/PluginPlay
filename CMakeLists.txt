# Copyright 2026 NWChemEx-Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

cmake_minimum_required(VERSION 3.14)

# Downloads common CMake modules used throughout NWChemEx. Included before
# project() so nwx_set_version() is available to compute the version.
include(cmake/get_nwx_cmake.cmake)

# Version from scikit-build-core (wheels/sdists) or the latest git tag.
include(nwx_set_version)
nwx_set_version(pp_version "${CMAKE_CURRENT_LIST_DIR}")
project(pluginplay VERSION "${pp_version}" LANGUAGES CXX)

include(disable_in_source_builds)
include(set_default_nwx_options)
option(BUILD_ROCKSDB          "Enable RocksDB backend of the cache"    OFF)

# Documentation target
include(nwx_cxx_api_docs)
nwx_cxx_api_docs("cxx/include" "cxx/src")

# Dependencies
include(get_dependencies)
get_dependencies(utilities parallelzone libfort boost)

# pybind11 must be fetched before nwx_library because python_wrapper.hpp is a
# public PluginPlay header that conditionally includes pybind11/pybind11.h when
# BUILD_PYBIND11 is defined.  Without pybind11 on the include path the library
# sources themselves fail to compile.
if(BUILD_PYBIND11_BINDINGS)
    get_dependencies(pybind11)
    # Development.Module is enough for the .so extension (built via
    # pybind11_add_module, which intentionally avoids linking libpython so
    # the module can be loaded by any Python that provides the symbols at
    # runtime). Development.Embed additionally provides the Python::Python
    # target (see below), needed so any C++-only executable that transitively
    # links this library (e.g. a Catch2 test binary, here or in a downstream
    # ecosystem project) gets those symbols resolved at link time instead --
    # an executable can't defer resolution to runtime the way a shared
    # library/extension can. This used to be gated on BUILD_TESTING, but
    # get_dependencies() force-sets BUILD_TESTING=OFF while FetchContent'ing
    # this project as someone else's dependency, which left downstream test
    # executables (e.g. simde's unit_test_simde) linking a libpluginplay.dylib
    # with no libpython and unresolved CPython symbols, aborting at load time.
    # Probing for Development.Embed instead of requiring it keeps manylinux
    # wheel-build containers (which deliberately don't ship libpython.so, and
    # never link a C++ executable against this library anyway) working: the
    # component there just won't be found, and Python::Python is skipped
    # below.
    find_package(Python REQUIRED COMPONENTS Interpreter Development.Module)
    find_package(Python QUIET COMPONENTS Development.Embed)
endif()

# TODO: add RocksDB handling when BUILD_ROCKSDB=ON

# Build the library
include(nwx_library)
nwx_library(${PROJECT_NAME} "cxx/include" "cxx/src"
    utilities parallelzone
    PUBLIC  boost
    PRIVATE libfort)

# PluginPlay exposes a compile-time flag so its source can #ifdef pybind11 paths.
# pybind11::pybind11 is added PUBLIC so downstream consumers that include
# python_wrapper.hpp (a public header) also get the pybind11 include dirs.
if(BUILD_PYBIND11_BINDINGS)
    target_compile_definitions(${PROJECT_NAME} PUBLIC BUILD_PYBIND11)
    # pybind11::pybind11 provides pybind11 headers. python_wrapper.hpp (a
    # public header, compiled into this library whenever BUILD_PYBIND11 is
    # set) calls the CPython C API directly. As a *shared* library,
    # ${PROJECT_NAME} itself tolerates those symbols staying undefined until
    # runtime (same as the pybind11 extension module); it's only the Catch2
    # test *executables* below that need them fully resolved at link time,
    # so only link libpython (Python::Python, from Development.Embed above)
    # when those executables are actually built.
    # Python::Module (Development.Module) provides the Python.h include path
    # python_wrapper.hpp needs to even compile; it's always available (see
    # find_package above) and, unlike Python::Python, doesn't link libpython.
    target_link_libraries(${PROJECT_NAME} PUBLIC pybind11::pybind11 Python::Module)
    if(TARGET Python::Python)
        target_link_libraries(${PROJECT_NAME} PUBLIC Python::Python)
    endif()
endif()

# Python module
include(nwx_python_module)
nwx_python_module(${PROJECT_NAME} "cxx/src")

# C++ tests
include(catch2_tests_from_dir)
catch2_tests_from_dir(
    "unit_test_${PROJECT_NAME}"
    "tests/cxx/unit_tests/${PROJECT_NAME}"
    ${PROJECT_NAME}
    PRIVATE_INCLUDES cxx/src
)
catch2_tests_from_dir(
    "test_regression_${PROJECT_NAME}"
    "tests/cxx/regression_tests"
    ${PROJECT_NAME}
    PRIVATE_INCLUDES cxx/src
)
catch2_tests_from_dir(
    "test_${PROJECT_NAME}_docs"
    "tests/cxx/doc_snippets"
    ${PROJECT_NAME}
    PRIVATE_INCLUDES cxx/src
)

# py_test_pluginplay: a pybind11 module exposing C++ test fixtures (BaseClass,
# property types, etc.) to tests/python/unit_tests/*.py. Not part of the
# installed package -- test-only, built in place alongside the other test
# binaries. The old build system built this via nwx_add_pybind11_module(...);
# reuses nwx_python_module (its NO_INSTALL keeps this out of the wheel). Its
# sources are prefixed export_ so they match the macro's source filter.
if(BUILD_PYBIND11_BINDINGS AND BUILD_TESTING)
    nwx_python_module(py_test_pluginplay "tests/python/unit_tests" NO_INSTALL)
endif()

# pluginplay_examples: a pybind11 module (EXPORT_PLUGIN, in
# tests/python/doc_snippets/export_pluginplay_examples.cpp) exposing the
# tutorial "Classical Force"/"Coulomb's Law" example modules to
# tests/python/doc_snippets/*.py. Depends on a small library built from the
# (non-test) implementation files under tests/cxx/doc_snippets -- listed
# explicitly rather than globbed, since that directory also holds Catch2
# TEST_CASE files and a custom main() (used by test_pluginplay_docs above)
# that aren't part of the example modules themselves. The pybind11 module
# itself reuses nwx_python_module (DEPENDS for the extra link libraries,
# NO_INSTALL to keep it out of the wheel); the supporting library is plain
# C++, not a pybind11 module, so it's built via nwx_library(... SOURCES ...
# NO_INSTALL) instead.
if(BUILD_PYBIND11_BINDINGS AND BUILD_TESTING)
    set(_pluginplay_examples_dir "${CMAKE_CURRENT_SOURCE_DIR}/tests/cxx/doc_snippets")
    nwx_library(pluginplay_examples_impl
        "tests/cxx/doc_snippets" "tests/cxx/doc_snippets"
        ${PROJECT_NAME}
        SOURCES
            "${_pluginplay_examples_dir}/classical_force.cpp"
            "${_pluginplay_examples_dir}/coulombs_law.cpp"
            "${_pluginplay_examples_dir}/load_modules.cpp"
            "${_pluginplay_examples_dir}/screened_coulombs_law.cpp"
            "${_pluginplay_examples_dir}/templated_coulombs_law.cpp"
        TYPE STATIC
        NO_INSTALL
    )

    nwx_python_module(pluginplay_examples "tests/python/doc_snippets" NO_INSTALL
        DEPENDS parallelzone pluginplay_examples_impl
    )
endif()
