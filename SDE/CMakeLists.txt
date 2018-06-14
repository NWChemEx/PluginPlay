cmake_minimum_required(VERSION ${CMAKE_VERSION})
project(SDE-SRC VERSION 0.0.0 LANGUAGES CXX)
include(TargetMacros)
set(SDE_SRCS Defaults/DefaultAM.cpp
                         Defaults/DefaultAtoms.cpp
                         Defaults/DefaultBases.cpp
                         Defaults/DefaultIsotopes.cpp
                         Defaults/DefaultMols.cpp
                         Defaults/DefaultSymbols.cpp
                         Defaults/PhysicalConstants.cpp
                         BasisSetFileParser.cpp
                         ChemistryRuntime.cpp
                         MoleculeFileParser.cpp
        )
set(SDE_INCLUDES BasisSetFileParser.hpp
                             ChemistryRuntime.hpp
                             NWXDefaults.hpp
                             Memoization.hpp
                             MoleculeFileParser.hpp
                             Parameters.hpp
                             Pythonization.hpp
                             SDEAny.hpp
                             Serialization.hpp
        )
list(FIND NWX_DEPENDENCIES pybind11 BUILD_PYTHON_BINDINGS)
if(NOT ${BUILD_PYTHON_BINDINGS} EQUAL -1)

endif()

set(SDE_DEFINITIONS )
set(SDE_LINK_FLAGS)
nwchemex_add_library(SDE SDE_SRCS
                                 SDE_INCLUDES
                                 SDE_DEFINTIONS
                                 SDE_LINK_FLAGS
)
