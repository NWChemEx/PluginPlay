cmake_minimum_required(VERSION 3.8)
project(SDE VERSION 1.0.0)
find_package(CPP REQUIRED)
### Options ###
option(BUILD_TESTS "Should we build the tests?" ON)
option(BUILD_PYBINDINGS "Should we build Python3 bindings?" ON)

### Dependencies ####
cpp_find_or_build_dependency(Catch2 URL github.com/catchorg/Catch2)
cpp_find_or_build_dependency(bphash URL github.com/bennybp/BPHash)
find_package(bphash)
target_include_directories(bphash::bphash SYSTEM
                           INTERFACE ${bphash_INCLUDE_DIRS})
cpp_find_or_build_dependency(
    cereal
    URL github.com/USCiLab/cereal
    CMAKE_ARGS JUST_INSTALL_CEREAL=ON
)
cpp_find_or_build_dependency(
    utilities
    URL github.com/NWChemEx-Project/utilities
    PRIVATE
)


### Target ###
add_subdirectory(SDE)

### Testing ###
if(${BUILD_TESTS})
    enable_testing()
    add_subdirectory(SDE_Test)
endif()
