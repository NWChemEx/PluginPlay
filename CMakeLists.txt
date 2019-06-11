cmake_minimum_required(VERSION 3.12)
project(sde VERSION 1.0.0)
set(CPP_NAMESPACE sde)
find_package(CPP REQUIRED)

### Options ###
option(BUILD_TESTS "Should we build the tests?" ON)
option(BUILD_PYBINDINGS "Should we build Python3 bindings?" ON)

### Dependencies ####
cpp_find_or_build_dependency(
    NAME cereal
    URL github.com/USCiLab/cereal
    CMAKE_ARGS JUST_INSTALL_CEREAL=ON
               SKIP_PORTABILITY_TEST=ON
)

cpp_find_or_build_dependency(NAME bphash URL github.com/ryanmrichard/BPHash
                                         BRANCH cmake_patch)

cpp_find_or_build_dependency(
    NAME utilities
    URL github.com/NWChemEx-Project/utilities
    PRIVATE
    CMAKE_ARGS BUILD_TESTS=OFF
)

if(BUILD_PYBINDINGS)
    find_package(Python3 REQUIRED)
endif()

### Target ###
add_subdirectory(sde)

### Testing ###
if(BUILD_TESTS)
    cpp_find_or_build_dependency(NAME Catch2 URL github.com/catchorg/Catch2)
    enable_testing()
    add_subdirectory(tests)
endif()
