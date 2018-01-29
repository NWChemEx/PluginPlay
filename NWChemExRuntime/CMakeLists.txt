cmake_minimum_required(VERSION ${CMAKE_VERSION})
project(NWChemExRuntime-SRC VERSION 0.0.0 LANGUAGES CXX)
include(TargetMacros)
set(NWChemExRuntime_SRCS AtomicInfo.cpp
                         BasisSetFileParser.cpp
                         PhysicalConstants.cpp
                         Wikipedia.cpp)
set(NWChemExRuntime_INCLUDES AtomicInfo.hpp
                             BasisSetFileParser.hpp
                             PhysicalConstants.hpp
                             Wikipedia.hpp)
set(NWChemExRuntime_DEFINITIONS )
set(NWChemExRuntime_LINK_FLAGS )
nwchemex_add_library(NWChemExRuntime NWChemExRuntime_SRCS
                                 NWChemExRuntime_INCLUDES
                                 NWChemExRuntime_DEFINTIONS
                                 NWChemExRuntime_LINK_FLAGS
)
