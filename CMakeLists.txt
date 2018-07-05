cmake_minimum_required(VERSION 3.6)
#file(
#        DOWNLOAD
#        https://github.com/NWChemEx-Project/DeveloperTools/raw/master/cmake.tar.gz
#        ${CMAKE_BINARY_DIR}/cmake.tar.gz)
#execute_process(
#        COMMAND ${CMAKE_COMMAND} -E tar xzf ${CMAKE_BINARY_DIR}/cmake.tar.gz
#        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
#)

include(${CMAKE_BINARY_DIR}/cmake/Macros.cmake)
start_hunter(LOCAL) #Must be before project command

project(SDE VERSION 0.0.0 LANGUAGES CXX)

### Options ###
option(BUILD_TESTS "Should we build the tests?" ON)
option(BUILD_PYBINDINGS "Should we build Python3 bindings?" ON)

### Dependencies ####
foreach(depend Utilities LibChemist bphash cereal)
    hunter_add_package(${depend})
    find_package(${depend} CONFIG)
endforeach()

### Target ###
add_subdirectory(SDE)

### Testing ###
if(${BUILD_TESTS})
    enable_testing()
    add_subdirectory(SDE_Test)
endif()
