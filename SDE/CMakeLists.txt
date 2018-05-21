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
                             ModuleHelpers.hpp
                             Module.hpp
                             MoleculeFileParser.hpp
                             Property.hpp
                             SDEAny.hpp
                             Serialization.hpp
        )
set(SDE_DEFINITIONS )
set(SDE_LINK_FLAGS)

list(FIND NWX_DEPENDENCIES pybind11 BUILD_PYTHON_BINDINGS)
if(NOT ${BUILD_PYTHON_BINDINGS} EQUAL -1)
    list(APPEND SDE_SRCS  PyBindings/Pythonize.cpp
                          PyBindings/PyModule.cpp)
    list(APPEND SDE_INCLUDES PyBindings/PyModule.hpp)
    set(INIT_FILE PyBindings/__init__.py)
    nwchemex_add_pymodule(SDE SDE_SRCS
            SDE_INCLUDES
            SDE_DEFINTIONS
            SDE_LINK_FLAGS
            ${INIT_FILE}
            )
else()
    nwchemex_add_library(SDE SDE_SRCS
            SDE_INCLUDES
            SDE_DEFINTIONS
            SDE_LINK_FLAGS
            )
endif()
