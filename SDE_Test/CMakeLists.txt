cmake_minimum_required(VERSION ${CMAKE_VERSION})
project(SDE-Test VERSION 0.0.0 LANGUAGES CXX)
include(TargetMacros)
install(FILES cc-pvdz.gbs
        DESTINATION ${CMAKE_INSTALL_PREFIX}/tests)

add_cxx_unit_test(TestBasisFileParser)
add_cxx_unit_test(TestChemistryRuntime)
add_cxx_unit_test(TestMoleculeFileParser)
