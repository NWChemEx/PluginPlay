# This function will skim a CMake target and create a file _init__.py that
# should be placed next to the shared library created by that target. This
# function assumes the target's:
#
# * public header files are in the ``PUBLIC_HEADER`` property
# * include paths are in the ``INTERFACE_INCLUDE_DIRECTORIES`` property
# * dependencies are targets and in the ``INTERFACE_LINK_LIBRAIRES`` property
#
#
# :Additional Named Arguments:
#     * NAMESPACE - The C++ namespace that your bindings live in. 
#     * PACKAGENAME - Package name to used as an alternative to NAMESPACE.  
#     * DEPENDSON - List of modules this module depends on.
#
function(cppyy_make_python_package)
    #---------------------------------------------------------------------------
    #--------------------------Argument Parsing---------------------------------
    #---------------------------------------------------------------------------
    set(options NOTUSED)
    set(oneValueArgs PACKAGENAME)
    set(multiValueArgs DEPENDSON HEADERS NAMESPACES DEPNAMESPACES)
    cmake_parse_arguments(install_data "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )
    #---------------------------------------------------------------------------
    #--------------------------Get include directories--------------------------
    #---------------------------------------------------------------------------
    get_target_property(include_dirs ${install_data_PACKAGENAME} INCLUDE_DIRECTORIES)
    foreach(item ${install_data_DEPENDSON})
        if ("${item}" STREQUAL "MPI")
           list(APPEND include_dirs ${MPI_CXX_HEADER_DIR})
           set(init_with_mpi ${item})
        else()
           get_target_property(include_item ${item} INCLUDE_DIRECTORIES)
           list(APPEND include_dirs ${include_item})
        endif()
    endforeach()
    #---------------------------------------------------------------------------
    #------------Collect the information we need off the target-----------------
    #---------------------------------------------------------------------------
    #List of include directories, usually a generator
    set(target_lib "$<TARGET_FILE_NAME:${install_data_PACKAGENAME}>")
    set(output_dir "${CMAKE_BINARY_DIR}/${install_data_PACKAGENAME}")
    #---------------------------------------------------------------------------
    #-----------------Generate _init__.py file contents------------------------
    #---------------------------------------------------------------------------
    set(init_file_name "${output_dir}/__init__.py")
    set(init_file "import cppyy\n\n")
    foreach(depnamespace ${install_data_DEPNAMESPACES})
        set(init_file "${init_file}from ${depnamespace} import \*\n")
    endforeach()
    set(init_file "${init_file}\nimport os\n")
    set(init_file "${init_file}paths = \"${include_dirs}\".split(';')\n")
    set(init_file "${init_file}for p in paths:\n")
    set(init_file "${init_file}    if p and p!=\"\":\n")
    set(init_file "${init_file}        cppyy.add_include_path(p)\n")
    if ("${init_with_mpi}" STREQUAL "MPI")
        set(init_file "${init_file}cppyy.cppdef(\"\"\"\\ \n")
        set(init_file "${init_file}\#define thread_local\n")
        set(init_file "${init_file}\#define is_server_thread \*_cling_is_server_thread()\n")
        set(init_file "${init_file}\#include \"${MADNESS_SOURCE_DIR}/src/madness/world/worldrmi.h\"\n")
        set(init_file "${init_file}\#undef thread_local\n")
        set(init_file "${init_file}\"\"\")\n")
    endif()
    foreach(header ${install_data_HEADERS})
        set(init_file "${init_file}cppyy.include(\"${install_data_PACKAGENAME}/${header}.hpp\")\n")
    endforeach()
    set(init_file "${init_file}\ncppyy.load_library(\"${CMAKE_BINARY_DIR}/${target_lib}\")\n\n")
    foreach(namespace ${install_data_NAMESPACES})
        set(init_file "${init_file}from cppyy.gbl import ${namespace}\n")
    endforeach()
    set(init_file "${init_file}from cppyy.gbl.std import array, vector, make_shared\n\n")
    #Write it out
    file(GENERATE OUTPUT ${init_file_name} CONTENT "${init_file}")
endfunction()
