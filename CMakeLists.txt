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
SET(VERSION 1.0.0) #TODO: get from git
project(pluginplay VERSION "${VERSION}" LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(
    nwx_cmake
    GIT_REPOSITORY https://github.com/NWChemEx-Project/NWXCMake
)
FetchContent_MakeAvailable(nwx_cmake)
set(
    CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${nwx_cmake_SOURCE_DIR}/cmake"
    CACHE STRING ""
    FORCE
)

include(get_cmaize)

### Options ###
option(BUILD_TESTING "Should we build the tests?" OFF)
option(BUILD_PYBINDINGS "Should we build Python3 bindings?" ON)
option(BUILD_ROCKSDB "Should we build the RocksDB backend?" OFF)

# Work out the project paths
set(project_inc_dir "${CMAKE_CURRENT_LIST_DIR}/include/${PROJECT_NAME}")
set(project_src_dir "${CMAKE_CURRENT_LIST_DIR}/src/${PROJECT_NAME}")

include(nwx_cxx_api_docs)
# nwx_cxx_api_docs("${CMAKE_CURRENT_SOURCE_DIR}/src" "${CMAKE_CURRENT_SOURCE_DIR}/include")
nwx_cxx_api_docs("${project_inc_dir}")



cmaize_find_or_build_dependency(
    utilities
    URL github.com/NWChemEx-Project/utilities
    BUILD_TARGET utilities
    FIND_TARGET nwx::utilities
    CMAKE_ARGS BUILD_TESTING=${UTILITIES_BUILD_TESTING}
)

cmaize_find_or_build_dependency(
    libfort
    URL github.com/seleznevae/libfort
    BUILD_TARGET fort
    FIND_TARGET libfort::fort
    CMAKE_ARGS FORT_ENABLE_TESTING=OFF
)

cmaize_find_or_build_dependency(
    parallelzone
    URL github.com/NWChemEx-Project/ParallelZone
    PRIVATE TRUE
    BUILD_TARGET parallelzone
    FIND_TARGET nwx::parallelzone
    CMAKE_ARGS BUILD_TESTING=${PARALLELZONE_BUILD_TESTING}
)

find_package(Boost REQUIRED)

set(ROCKSDB_TARGET "")
if("${BUILD_ROCKSDB}")
    set(ROCKSDB_TARGET RocksDB::rocksdb-shared)
    find_package(RocksDB REQUIRED)
    # TODO: We need to do a superbuild for RocksDB
    # cpp_find_or_build_dependency(
    #     RocksDB
    #     URL github.com/facebook/rocksdb
    #     VERSION v7.0.4
    #     BUILD_TARGET rocksdb-shared
    #     FIND_TARGET ${ROCKSDB_TARGET}
    #     CMAKE_ARGS CMAKE_BUILD_TYPE=Release
    #                ROCKSDB_BUILD_SHARED=On
    #                WITH_BENCHMARK_TOOLS=Off
    #                WITH_GFLAGS=Off
    #                WITH_TOOLS=OFF
    #                JNI=OFF
    #                WITH_TESTS=OFF
    # )
    target_compile_definitions(${ROCKSDB_TARGET} INTERFACE BUILD_ROCKS_DB)
endif()

cmaize_add_library(
    ${PROJECT_NAME}
    SOURCE_DIR "${project_src_dir}"
    INCLUDE_DIRS "${project_inc_dir}"
    DEPENDS utilities parallelzone libfort ${ROCKSDB_TARGET}
            Boost::boost
)

include(nwx_python_mods)
cppyy_make_python_package(
    PACKAGE pluginplay
    NAMESPACES pluginplay
    DEPPACKAGES parallelzone
)

if("${BUILD_TESTING}")
    set(tests_src_dir "${CMAKE_CURRENT_LIST_DIR}/tests/${PROJECT_NAME}")
    set(examples_src_dir "${CMAKE_CURRENT_LIST_DIR}/tests/${PROJECT_NAME}/docs")
    cmaize_add_library(
        ${PROJECT_NAME}_examples
        SOURCE_DIR ${examples_src_dir}
        INCLUDE_DIRS ${examples_src_dir}
        DEPENDS ${PROJECT_NAME}
    )
    cppyy_make_python_package(
        PACKAGE ${PROJECT_NAME}_examples
        NAMESPACES pluginplay_examples
        DEPPACKAGES pluginplay
    )
    cmaize_find_or_build_dependency(
        Catch2
        URL github.com/catchorg/Catch2
        BUILD_TARGET Catch2
        FIND_TARGET Catch2::Catch2
        VERSION v2.13.8
    )
    cmaize_add_tests(
        test_${PROJECT_NAME}
        SOURCE_DIR ${tests_src_dir}
        INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/src/${PROJECT_NAME}"
        DEPENDS Catch2 ${PROJECT_NAME}_examples
    )
endif()

cmaize_add_package(${PROJECT_NAME} NAMESPACE nwx::)
