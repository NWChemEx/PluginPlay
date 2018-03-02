cmake_minimum_required(VERSION ${CMAKE_VERSION})
project(NWChemExRuntime-Test VERSION 0.0.0 LANGUAGES CXX)
include(TargetMacros)

add_cxx_unit_test(TestBasisFileParser)
