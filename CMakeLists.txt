cmake_minimum_required(VERSION 3.8)
project(SDE VERSION 1.0.0)
set(CPP_NAMESPACE SDE)
find_package(CPP REQUIRED)

### Options ###
option(BUILD_TESTS "Should we build the tests?" ON)
option(BUILD_PYBINDINGS "Should we build Python3 bindings?" ON)

### Dependencies ####
cpp_find_or_build_dependency(NAME Catch2 URL github.com/catchorg/Catch2)
cpp_find_or_build_dependency(NAME bphash URL github.com/ryanmrichard/BPHash
                                    BRANCH cmake_patch)
cpp_find_or_build_dependency(
    NAME cereal
    URL github.com/USCiLab/cereal
    CMAKE_ARGS JUST_INSTALL_CEREAL=ON
               SKIP_PORTABILITY_TEST=ON
)

cpp_find_or_build_dependency(NAME Catch2 URL github.com/catchorg/Catch2)
cpp_find_or_build_dependency(NAME bphash URL github.com/ryanmrichard/BPHash
                                         BRANCH cmake_patch)

cpp_find_or_build_dependency(
    NAME utilities
    URL github.com/NWChemEx-Project/Utilities
    PRIVATE
    CMAKE_ARGS BUILD_TESTS=OFF
)

### Target ###
add_subdirectory(SDE)

### Testing ###
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(SDE_Test)
endif()
