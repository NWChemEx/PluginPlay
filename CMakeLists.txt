cmake_minimum_required(VERSION 3.14)
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/version.txt" VERSION)
project(pluginplay VERSION "${VERSION}" LANGUAGES CXX)

set(
    CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${PROJECT_SOURCE_DIR}/cmake"
    CACHE STRING "" FORCE
)

include(get_cpp)

### Options ###
option(BUILD_TESTING "Should we build the tests?" OFF)
option(
    PARALLELZONE_BUILD_TESTING
    "Should we build the tests for ParallelZone?" OFF
)
option(UTILITIES_BUILD_TESTING "Should we build the tests for Utilities?" OFF)
option(BUILD_DOCS "Should we build the documentation?" OFF)
option(
    ONLY_BUILD_DOCS
    "If enabled and BUILD_DOCS is true no libraries will be built" OFF
)
option(BUILD_PYBINDINGS "Should we build Python3 bindings?" ON)
option(BUILD_ROCKSDB "Should we build the RocksDB backend?" ON)

# Work out the project paths
set(project_inc_dir "${CMAKE_CURRENT_LIST_DIR}/include/${PROJECT_NAME}")
set(project_src_dir "${CMAKE_CURRENT_LIST_DIR}/src/${PROJECT_NAME}")

if("${BUILD_DOCS}")
    find_package(Doxygen REQUIRED)
    set(DOXYGEN_EXTRA_PACKAGES amsmath) # Extra LaTeX packages for documentation
    doxygen_add_docs(pluginplay_cxx_api "${project_inc_dir}")
    if("${ONLY_BUILD_DOCS}")
        return()
    endif()
endif()

cpp_find_or_build_dependency(
    utilities
    URL github.com/NWChemEx-Project/utilities
    PRIVATE TRUE
    BUILD_TARGET utilities
    FIND_TARGET nwx::utilities
    CMAKE_ARGS BUILD_TESTING=${UTILITIES_BUILD_TESTING}
)

cpp_find_or_build_dependency(
    libfort
    URL github.com/seleznevae/libfort
    BUILD_TARGET fort
    FIND_TARGET libfort::fort
    CMAKE_ARGS FORT_ENABLE_TESTING=OFF
)

cpp_find_or_build_dependency(
        parallelzone
        URL github.com/NWChemEx-Project/ParallelZone
        PRIVATE TRUE
        BUILD_TARGET parallelzone
        FIND_TARGET nwx::parallelzone
        CMAKE_ARGS BUILD_TESTING=${PARALLELZONE_BUILD_TESTING}
)

set(ROCKSDB_TARGET RocksDB::rocksdb-shared)
if("${BUILD_ROCKSDB}")
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
else()
    add_library(${ROCKSDB_TARGET} INTERFACE)
endif()

cpp_add_library(
    ${PROJECT_NAME}
    SOURCE_DIR "${project_src_dir}"
    INCLUDE_DIR "${project_inc_dir}"
    DEPENDS nwx::utilities nwx::parallelzone libfort::fort ${ROCKSDB_TARGET}
)

include(nwx_python_mods)
cppyy_make_python_package(PACKAGE pluginplay NAMESPACES pluginplay DEPPACKAGES parallelzone )

if("${BUILD_TESTING}")
    set(tests_src_dir "${CMAKE_CURRENT_LIST_DIR}/tests/${PROJECT_NAME}")
    set(examples_src_dir "${CMAKE_CURRENT_LIST_DIR}/tests/${PROJECT_NAME}/docs")
    cpp_add_library(
        ${PROJECT_NAME}_examples
        SOURCE_DIR ${examples_src_dir}
        INCLUDE_DIR ${examples_src_dir}
        DEPENDS ${PROJECT_NAME}
    )
    include(nwx_python_mods)
        cppyy_make_python_package(PACKAGE ${PROJECT_NAME}_examples NAMESPACES pluginplay_examples DEPPACKAGES pluginplay)
    cpp_find_or_build_dependency(
        Catch2
        URL github.com/catchorg/Catch2
        BUILD_TARGET Catch2
        FIND_TARGET Catch2::Catch2
	    VERSION v2.13.8
    )
    cpp_add_tests(
        test_${PROJECT_NAME}
        SOURCE_DIR ${tests_src_dir}
        INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/src/${PROJECT_NAME}"
        DEPENDS Catch2::Catch2 ${PROJECT_NAME}_examples
    )
endif()
