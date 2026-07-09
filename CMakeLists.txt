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
    # target (see below), which links libpython -- needed because
    # ${PROJECT_NAME} itself calls into the CPython C API directly.
    find_package(Python REQUIRED
        COMPONENTS Interpreter Development.Module Development.Embed)
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
    # set) calls the CPython C API directly, so ${PROJECT_NAME} itself needs
    # libpython linked in -- Python::Python (Development.Embed) rather than
    # Python::Module (Development.Module, which intentionally omits linking
    # libpython so a portable .so extension can be loaded by any Python
    # providing those symbols at runtime; that's still how the extension
    # itself is built, via pybind11_add_module in nwx_python_module). Without
    # this, anything that links ${PROJECT_NAME} as a normal executable (the
    # Catch2 test binaries below) fails with undefined references to
    # _Py_Dealloc and friends.
    target_link_libraries(${PROJECT_NAME} PUBLIC pybind11::pybind11 Python::Python)
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
# C++, not a pybind11 module, so it's built directly here.
if(BUILD_PYBIND11_BINDINGS AND BUILD_TESTING)
    set(_pluginplay_examples_dir "${CMAKE_CURRENT_SOURCE_DIR}/tests/cxx/doc_snippets")
    add_library(pluginplay_examples_impl STATIC
        "${_pluginplay_examples_dir}/classical_force.cpp"
        "${_pluginplay_examples_dir}/coulombs_law.cpp"
        "${_pluginplay_examples_dir}/load_modules.cpp"
        "${_pluginplay_examples_dir}/screened_coulombs_law.cpp"
        "${_pluginplay_examples_dir}/templated_coulombs_law.cpp"
    )
    target_include_directories(pluginplay_examples_impl PUBLIC "${_pluginplay_examples_dir}")
    target_link_libraries(pluginplay_examples_impl PUBLIC ${PROJECT_NAME})

    nwx_python_module(pluginplay_examples "tests/python/doc_snippets" NO_INSTALL
        DEPENDS parallelzone pluginplay_examples_impl
    )
endif()
