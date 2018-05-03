cmake_minimum_required(VERSION ${CMAKE_VERSION})
project(SDE-Test VERSION 0.0.0 LANGUAGES CXX)
include(TargetMacros)

add_cxx_unit_test(TestBasisFileParser)
add_cxx_unit_test(TestChemistryRuntime)
add_cxx_unit_test(TestModule)
add_cxx_unit_test(TestMoleculeFileParser)
add_cxx_unit_test(TestSDEAny)
