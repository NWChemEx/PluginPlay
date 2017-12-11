cmake_minimum_required(VERSION ${CMAKE_VERSION})
project(NWChemExRuntime-Test VERSION 0.0.0 LANGUAGES CXX)
find_package(NWChemExRuntime REQUIRED)
include(TargetMacros)

add_cxx_unit_test(TestDevice NWChemExRuntime)
add_cxx_unit_test(TestFuturePromise NWChemExRuntime)

