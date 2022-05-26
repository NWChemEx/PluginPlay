include_guard()

set(ppgmd_config_file "${CMAKE_CURRENT_LIST_DIR}/generate_module_docs.cpp.in")

#[[[
# Generates documentation for modules in the project where it is run.
#
# :param OUTPUT_DIR: Output directory for the module documentation source 
#                    files.
# :type OUTPUT_DIR: path
# :param HEADER_PATH: Path to the header file that includes all of the
#                     modules for the project.
# :type HEADER_PATH: path
# :param LOAD_MODULES_FXN: Name of the function used to load the modules,
#                          including any namespaces necessary to qualify the
#                          function.
# :type LOAD_MODULES_FXN: str
# :param DEPEND: The CMake target names that the modules depend on.
# :type DEPEND: str
#]]
function(plugin_play_generate_module_docs)
    set(ppgmd_one_val OUTPUT_DIR HEADER_PATH LOAD_MODULES_FXN DEPEND)
    cmake_parse_arguments(ppgmd "" "${ppgmd_one_val}" "" ${ARGN})

    set(
        ppgmd_output_file
        "${CMAKE_CURRENT_BINARY_DIR}/pp_module_docs/generate_module_docs.cpp"
    )

    configure_file("${ppgmd_config_file}" "${ppgmd_output_file}")
    add_executable(generate_module_docs "${ppgmd_output_file}")
    target_link_libraries(generate_module_docs "${ppgmd_DEPEND}")
    add_dependencies(generate_module_docs "${ppgmd_DEPEND}")
endfunction()

#[[[
# Convenience function for when the module collection follows usual
# conventions.
#
# .. warning::
#
#    Calling the executable which results from this function will overwrite
#    the contents of the output directory supplied to this function.
#
# This function assumes your module collection's ``load_module`` function is
# included by including ``NAME/NAME.hpp``, the ``load_module`` function is
# in the namespace ``NAME`` and the name of the target which builds your
# module collection is also ``NAME``. If all of this is true, supplying
# ``NAME`` and where you want the output suffices to be able to call
# ``plugin_play_generate_module_docs``. If any of this is not the case, then
# you should go through the main ``plugin_play_generate_module_docs`` API.
#
# :param ppgmdfn_name: The case-sensitive name of your module collection.
# :type ppgmdfn_name: str
# :param ppgmdfn_output: The full path of the directory where the files
#                        should be written to. If the directory already
#                        exists it will be overwritten when the executable is
#                        run.
# :type ppgmdfn_output: path
#
# **Example Usage**
#
# In a top-level ``CMakeLists.txt`` file, the following code snippet can be
# included to generate module documentation.
#
# .. code-block:: cmake
# 
#    include(make_module_docs)
#    plugin_play_generate_module_docs_from_name(
#        ${CMAKE_PROJECT_NAME}
#        "${CMAKE_CURRENT_LIST_DIR}/build/module_apis"
#    )
#]]
function(plugin_play_generate_module_docs_from_name ppgmdfn_name ppgmdfn_output)
    plugin_play_generate_module_docs(
        OUTPUT_DIR "${ppgmdfn_output}"
        HEADER_PATH "${ppgmdfn_name}/${ppgmdfn_name}.hpp"
        LOAD_MODULES_FXN "${ppgmdfn_name}::load_modules"
        DEPEND "${ppgmdfn_name}"
    )
endfunction()
