# This function will skim a CMake target and create a file __init__.py that
# should be placed next to the shared library created by that target. This
# function assumes the target's:
#
# * public header files are in the ``PUBLIC_HEADER`` property
# * include paths are in the ``INTERFACE_INCLUDE_DIRECTORIES`` property
# * dependencies are targets and in the ``INTERFACE_LINK_LIBRAIRES`` property
#
#
# :param target: The target name we are creating bindings for. Must be a valid
#                target.
#
# :Additional Named Arguments:
#     * *NAMESPACE* - The C++ namespace that your bindings live in. Will be used
#       as the name of the Python module for the resulting package.
#     * *PREFIX* - The path relative to header include root. Typically the name
#       of the directory that the target was added in. Defaults to the directory
#       this function was called from (note it's just the directory,not the full
#       path).
#     * *OUTPUT_DIR* - The build-time directory where the resulting file should
#       be placed. By default assumed to be the binary directory with the PREFIX
#       appended to it.
function(cppyy_make_python_package _cmpp_target)
    #---------------------------------------------------------------------------
    #-------------------------Basic error-checking------------------------------
    #---------------------------------------------------------------------------
    if("${_cmpp_target}" STREQUAL "")
        message(FATAL_ERROR "Target name may not be empty.")
    endif()
    if(NOT TARGET ${_cmpp_target})
        message(FATAL_ERROR "${_cmpp_target} is not a target.")
    endif()

    #---------------------------------------------------------------------------
    #--------------------------Argument Parsing---------------------------------
    #---------------------------------------------------------------------------
    set(_cmpp_options NAMESPACE PREFIX OUTPUT_DIR TEST)
    cmake_parse_arguments(_cmpp "" "${_cmpp_options}" "" ${ARGN})
    if("${_cmpp_PREFIX}" STREQUAL "")
        get_filename_component(_cmpp_PREFIX ${CMAKE_CURRENT_SOURCE_DIR} NAME_WE)
        string(TOLOWER ${_cmpp_PREFIX} _cmpp_PREFIX)
    endif()
    if("${_cmpp_OUTPUT_DIR}" STREQUAL "")
        set(_cmpp_OUTPUT_DIR "${CMAKE_BINARY_DIR}/PlugInPlay")
    endif()

    #---------------------------------------------------------------------------
    #------------Collect the information we need off the target-----------------
    #---------------------------------------------------------------------------
    #List of include directories, usually a generator
    get_target_property(
            _cmpp_inc_dir ${_cmpp_target} SOURCE_DIR)
    #Add dependent libraries
    list(APPEND _cmpp_inc_dir ${utilities_SOURCE_DIR})
    list(APPEND _cmpp_inc_dir ${pluginplay_SOURCE_DIR}/include)
    list(APPEND _cmpp_inc_dir ${parallelzone_SOURCE_DIR}/include)
    list(APPEND _cmpp_inc_dir ${cereal_SOURCE_DIR}/include ${BPHash_SOURCE_DIR})
    list(APPEND _cmpp_inc_dir ${MADNESS_SOURCE_DIR}/src ${MADNESS_BINARY_DIR}/src)
    list(APPEND _cmpp_inc_dir ${MPI_CXX_HEADER_DIR}) 
    #The library name (obviously a generator...)
    set(_cmpp_lib "$<TARGET_FILE_NAME:${_cmpp_target}>")

    #---------------------------------------------------------------------------
    #-----------------Generate __init__.py file contents------------------------
    #---------------------------------------------------------------------------
    set(_cmpp_file_name "${_cmpp_OUTPUT_DIR}/__init__.py")
    set(_cmpp_file "import cppyy\n\n")
    foreach(_item ${_cmpp_inc_dir})
          set(_cmpp_file "${_cmpp_file}cppyy.add_include_path(\"${_item}\")\n")
    endforeach()
    set(_cmpp_file "${_cmpp_file}cppyy.cppdef(\"\"\"\\ \n")
    set(_cmpp_file "${_cmpp_file}\#define thread_local\n")
    set(_cmpp_file "${_cmpp_file}\#define is_server_thread \*_cling_is_server_thread()\n")
    set(_cmpp_file "${_cmpp_file}\#include \"${MADNESS_SOURCE_DIR}/src/madness/world/worldrmi.h\"\n")
    set(_cmpp_file "${_cmpp_file}\#undef thread_local\n")
    set(_cmpp_file "${_cmpp_file}\"\"\")\n")
    set(_cmpp_file "${_cmpp_file}cppyy.include(\"${pluginplay_SOURCE_DIR}/include/pluginplay/pluginplay.hpp\")\n")
    set(_cmpp_file "${_cmpp_file}\ncppyy.load_library(\"${CMAKE_BINARY_DIR}/${_cmpp_lib}\")\n\n")
    set(_cmpp_file "${_cmpp_file}from cppyy.gbl import pluginplay\n")
    set(_cmpp_file "${_cmpp_file}from cppyy.gbl.std import array, vector, make_shared\n\n")
    #Write it out
    file(GENERATE OUTPUT ${_cmpp_file_name} CONTENT "${_cmpp_file}")
endfunction()
