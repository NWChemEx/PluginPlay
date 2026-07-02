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
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

option(BUILD_TESTING          "Whether to build the unit tests"        OFF)
option(BUILD_PYBIND11_BINDINGS "Build Python bindings via pybind11"    ON)
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
    find_package(Python REQUIRED COMPONENTS Interpreter Development.Module)
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
    # pybind11::pybind11 provides pybind11 headers; Python::Module provides
    # Python.h (the Development.Module component of find_package(Python)).
    target_link_libraries(${PROJECT_NAME} PUBLIC pybind11::pybind11 Python::Module)
endif()

# Python module
include(nwx_python_module)
nwx_python_module(${PROJECT_NAME} "cxx/src")

# C++ tests
if(BUILD_TESTING)
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
endif()
