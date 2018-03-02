cmake_minimum_required(VERSION 3.6)
project(NWChemExRuntime VERSION 0.0.0 LANGUAGES CXX)
set(NWChemExRuntime_DEPENDENCIES LibChemist UtilitiesEx)
find_package(NWChemExBase)
build_nwchemex_module(${CMAKE_CURRENT_LIST_DIR})

