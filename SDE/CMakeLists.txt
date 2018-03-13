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
                         MoleculeFileParser.cpp
        )
set(SDE_INCLUDES BasisSetFileParser.hpp
                             ChemistryRuntime.hpp
                             NWXDefaults.hpp
                             MoleculeFileParser.hpp
        )
set(SDE_DEFINITIONS )
set(SDE_LINK_FLAGS)
nwchemex_add_library(SDE SDE_SRCS
                                 SDE_INCLUDES
                                 SDE_DEFINTIONS
                                 SDE_LINK_FLAGS
)
