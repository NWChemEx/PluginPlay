cmake_minimum_required(VERSION 3.14)
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/version.txt" VERSION)
project(sde VERSION "${VERSION}" LANGUAGES CXX)

set(
    CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${PROJECT_SOURCE_DIR}/cmake"
    CACHE STRING "" FORCE
)

include(get_cpp)

### Options ###
option(BUILD_TESTING "Should we build the tests?" OFF)
option(BUILD_DOCS "Should we build the documentation?" OFF)
option(
    ONLY_BUILD_DOCS
    "If enabled and BUILD_DOCS is true no libraries will be built" OFF
)

option(BUILD_PYBINDINGS "Should we build Python3 bindings?" ON)

if("${BUILD_DOCS}")
    find_package(Doxygen REQUIRED)
    set(DOXYGEN_EXTRA_PACKAGES amsmath) # Extra LaTeX packages for documentation
    doxygen_add_docs(sde_cxx_api "${CMAKE_CURRENT_LIST_DIR}/sde")
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
        CMAKE_ARGS BUILD_TESTING=OFF
)

cpp_find_or_build_dependency(
        parallelzone
        URL github.com/NWChemEx-Project/ParallelZone
        PRIVATE TRUE
        BUILD_TARGET parallelzone
        FIND_TARGET nwx::parallelzone
        CMAKE_ARGS BUILD_TESTING=OFF
)

cpp_add_library(
    sde
    SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/src/sde"
    INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/include/sde"
    DEPENDS nwx::utilities nwx::runtime
)

if("${BUILD_TESTING}")
    cpp_find_or_build_dependency(
        Catch2
        URL github.com/catchorg/Catch2
        BUILD_TARGET Catch2
        FIND_TARGET Catch2::Catch2
	VERSION v2.13.3
    )
    cpp_add_tests(
        test_sde
        SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/tests"
        INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/sde"
        DEPENDS Catch2::Catch2 sde
    )
endif()


