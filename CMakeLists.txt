# Copyright 2022 NWChemEx-Project
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

# Downloads common CMake modules used throughout NWChemEx
include(cmake/get_nwx_cmake.cmake)

#Sets the version to whatever git thinks it is
include(get_version_from_git)
get_version_from_git(pluginplay_version "${CMAKE_CURRENT_LIST_DIR}")
project(pluginplay VERSION "${pluginplay_version}" LANGUAGES CXX)

include(nwx_versions)
include(get_cmaize)

# Work out full paths to the project's include/source dirs
set(project_inc_dir "${CMAKE_CURRENT_LIST_DIR}/include/${PROJECT_NAME}")
set(project_src_dir "${CMAKE_CURRENT_LIST_DIR}/src/${PROJECT_NAME}")

# Builds C++ API documentation
include(nwx_cxx_api_docs)
nwx_cxx_api_docs("README.md" "${project_inc_dir}" "${project_src_dir}")

### Options ###
cmaize_option_list(
    BUILD_TESTING OFF "Should we build unit tests?"
    BUILD_PYBIND11_PYBINDINGS OFF "Build Pybind11 Python bindings?"
    BUILD_ROCKSDB OFF "Enable RocksDB backend of the cache?"
)

### Dependendencies ###
cmaize_find_or_build_dependency(
    utilities
    URL github.com/NWChemEx/utilities
    VERSION ${NWX_UTILITIES_VERSION}
    BUILD_TARGET utilities
    FIND_TARGET nwx::utilities
    CMAKE_ARGS BUILD_TESTING=${UTILITIES_BUILD_TESTING}
)

cmaize_find_or_build_dependency(
    libfort
    URL github.com/seleznevae/libfort
    VERSION ${NWX_LIBFORT_VERSION}
    BUILD_TARGET fort
    FIND_TARGET libfort::fort
    CMAKE_ARGS FORT_ENABLE_TESTING=OFF
)

cmaize_find_or_build_dependency(
    parallelzone
    URL github.com/NWChemEx/ParallelZone
    VERSION ${NWX_PARALLELZONE_VERSION}
    BUILD_TARGET parallelzone
    FIND_TARGET nwx::parallelzone
    CMAKE_ARGS BUILD_TESTING=${PARALLELZONE_BUILD_TESTING}
)

# CI doesn't like this
#cmaize_find_dependency(
#    boost_container
#    URL github.com/boostorg/container
#    FIND_TARGET Boost::container
#)

find_package(Boost REQUIRED)

## Optional Dependencies ##
cmaize_find_optional_dependency(
    RocksDB
    BUILD_ROCKSDB
    URL github.com/facebook/rocksdb
    FIND_TARGET RocksDB::rocksdb-shared
)

set(pluginplay_depends utilities parallelzone libfort Boost::boost RocksDB)

# As of 1.0.0 CMaize does not support multiple build or find targets. This will
# be fixed in a future feature release. For now we handle the
include(nwx_pybind11)
if("${BUILD_PYBIND11_PYBINDINGS}")
    nwx_find_pybind11()
    list(
        APPEND pluginplay_depends
        pybind11_headers pybind11::embed Python::Python
    )
endif()

cmaize_add_library(
    ${PROJECT_NAME}
    SOURCE_DIR "${project_src_dir}"
    INCLUDE_DIRS "${project_inc_dir}"
    DEPENDS "${pluginplay_depends}"
)

# TODO: Update source to use BUILD_PYBIND11_PYBINDINGS, then goes away when
# CMaize supports mulitple targets
if("${BUILD_PYBIND11_PYBINDINGS}")
    target_compile_definitions("${PROJECT_NAME}" PUBLIC BUILD_PYBIND11)
endif()

nwx_add_pybind11_module(
    ${PROJECT_NAME}
    SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/src/python"
    DEPENDS "${PROJECT_NAME}"
)

if("${BUILD_TESTING}")
    set(cxx_test_dir "${CMAKE_CURRENT_LIST_DIR}/tests/cxx")
    set(tests_src_dir "${cxx_test_dir}/unit_tests/${PROJECT_NAME}")
    set(examples_src_dir "${cxx_test_dir}/doc_snippets")

    ### C++ Testing ###
    cmaize_find_or_build_dependency(
        Catch2
        URL github.com/catchorg/Catch2
        BUILD_TARGET Catch2
        FIND_TARGET Catch2::Catch2
        VERSION ${NWX_CATCH2_VERSION}
    )

    cmaize_add_tests(
        test_unit_${PROJECT_NAME}
        SOURCE_DIR ${tests_src_dir}
        INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/src/${PROJECT_NAME}"
        DEPENDS Catch2::Catch2 ${PROJECT_NAME}
    )

    cmaize_add_tests(
        test_${PROJECT_NAME}_docs
        SOURCE_DIR "${cxx_test_dir}/doc_snippets"
        INCLUDE_DIRS "${project_src_dir}"
        DEPENDS Catch2::Catch2 ${PROJECT_NAME}
    )

    ### Python Tests ###
    set(python_test_dir "${CMAKE_CURRENT_LIST_DIR}/tests/python")
    cmaize_find_or_build_dependency(
        tox
        PACKAGE_MANAGER pip
    )

    # In the python tests we will pass Python into PluginPlay, we will then
    # need to know that C++ can handle those Python types. To this end we
    # create a series of Python bindings for unit testing this functionality
    # which live in the py_test_pluginplay library.
    nwx_add_pybind11_module(
        py_test_pluginplay
        INSTALL OFF
        SOURCE_DIR "${python_test_dir}/unit_tests"
        INCLUDE_DIRS "${project_inc_dir}"
        DEPENDS ${PROJECT_NAME} tox
    )

    nwx_tox_test(
        py_pluginplay ${python_test_dir}/unit_tests
        SUBMODULES parallelzone
    )

    cmaize_add_library(
        ${PROJECT_NAME}_examples
        SOURCE_DIR ${examples_src_dir}
        INCLUDE_DIRS ${examples_src_dir}
        DEPENDS Catch2::Catch2 ${PROJECT_NAME}
    )

    nwx_add_pybind11_module(
        ${PROJECT_NAME}_examples
        INSTALL OFF
        SOURCE_DIR "${python_test_dir}/doc_snippets"
        DEPENDS parallelzone ${PROJECT_NAME} ${PROJECT_NAME}_examples
    )

    nwx_pybind11_tests(
        py_${PROJECT_NAME}_docs
        "${python_test_dir}/doc_snippets/test_doc_snippets.py"
        SUBMODULES parallelzone pluginplay
    )
endif()

cmaize_add_package(${PROJECT_NAME} NAMESPACE nwx::)
