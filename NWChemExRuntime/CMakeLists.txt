cmake_minimum_required(VERSION ${CMAKE_VERSION})
project(NWChemExRuntime-SRC VERSION 0.0.0 LANGUAGES CXX)
include(TargetMacros)
set(NWChemExRuntime_SRCS AtomicInfo.cpp)
set(NWChemExRuntime_INCLUDES AtomicInfo.hpp
                             PhysicalConstants.hpp)
set(NWChemExRuntime_DEFINITIONS )
nwchemex_add_library(NWChemExRuntime NWChemExRuntime_SRCS
                                 NWChemExRuntime_INCLUDES
                                 NWChemExRuntime_DEFINTIONS
)
