cmake_minimum_required(VERSION ${CMAKE_VERSION})
project(SDE-Test VERSION 0.0.0 LANGUAGES CXX)
include(TargetMacros)

add_cxx_unit_test(TestBasisFileParser)
add_cxx_unit_test(TestChemistryRuntime)
add_cxx_unit_test(TestModuleHelpers)
add_cxx_unit_test(TestModule)
add_cxx_unit_test(TestMoleculeFileParser)
add_cxx_unit_test(TestSDEAny)

set(DM_SRC DummyModule.cpp)
set(DM_INCS)
set(DM_FLAGS)
set(DM_LFLAGS)
nwchemex_add_pymodule(DummyModule DM_SRC DM_INC DM_FLAGS DM_LFLAGS __init__.py)
add_python_unit_test(TestModule)

