include_guard()

function(cmaize_wrap_target _cmaize_name _target)
    # Prepare the new target to be added to the top-level project
    cpp_get_global(_top_project CMAIZE_TOP_PROJECT)
    CMaizeTarget(CTOR _tgt_obj "${_target}")
    # install_path needs to be set to something or CMaize ignores the target
    # during config file generation. This will be treated as a pre-installed
    # target found by find_package(), but without a real install path
    CMaizeTarget(SET "${_tgt_obj}" install_path "tmp")
    CMaizeProject(add_target "${_top_project}" "${_cmaize_name}" "${_tgt_obj}")

    # Create package specification objct
    _fob_parse_arguments(_pkg_spec _pkg_name "${_cmaize_name}" ${ARGN})

    # Register the dependency with the current CMake package manager
    CMaizeProject(get_package_manager "${_top_project}" _pm "cmake")
    # TODO: Call this with NAME arg to handle components better
    CMakePackageManager(register_dependency
        "${_pm}" __dep "${_pkg_spec}"
        # Set both find and built target to avoid having COMPONENTS "" added
        # to every find_dependency() call generated
        FIND_TARGET "${_target}"
        ${ARGN}
    )
endfunction()

function(cmaize_find_package _package_name)
    set(_cfp_options "")
    set(_cfp_one_value "")
    # TARGETS option comes in pairs of "CMaize name" "CMake target"
    set(_cfp_multi_value "TARGETS")
    cmake_parse_arguments(_cfp "${_cfp_options}" "${_cfp_one_value}" "${_cfp_multi_value}" ${ARGN})

    # If TARGETS is not given, default the pairing to "${_package_name}" "${_package_name}"
    list(LENGTH _cfp_TARGETS _cfp_TARGETS_len)
    if(_cfp_TARGETS_len LESS_EQUAL 0)
        message(WARNING "No targets provided. Using default dependency target \"${_package_name}\" for \"${_package_name}\"")
        set(_cfp_TARGETS "${_package_name}" "${_package_name}")
        list(LENGTH _cfp_TARGETS _cfp_TARGETS_len)
    endif()

    # Validate that TARGETS is in the correct pair-wise form, simultaneously
    # TODO: Assert that length is even

    # Turn TARGETS into a map of CMaize name -> CMake target
    cpp_map(CTOR _cfp_target_map)
    foreach(_cfp_i RANGE 0 "${_cfp_TARGETS_len}" 2)
        # Grab the key-value pair of items
        list(POP_FRONT _cfp_TARGETS _cfp_cmaize_name)
        list(POP_FRONT _cfp_TARGETS _cfp_cmake_target)

        cpp_map(APPEND "${_cfp_target_map}"
            "${_cfp_cmaize_name}" "${_cfp_cmake_target}"
        )
    endforeach()
    # Grab the keys for later usage
    cpp_map(KEYS "${_cfp_target_map}" _cfp_cmaize_names)

    # Call find_package() to do the heavy lifting and find everything
    message(STATUS "Searching for ${_package_name}")
    find_package("${_package_name}" ${_cfp_UNPARSED_ARGUMENTS})

    message(DEBUG "${_package_name}_FOUND: ${${_package_name}_FOUND}")

    # Make sure each expected target exists after the find_package() call
    foreach(_cfp_cmaize_name ${_cfp_cmaize_names})
        cpp_map(GET "${_cfp_target_map}"
            _cfp_cmake_target "${_cfp_cmaize_name}"
        )

        if(NOT TARGET "${_cfp_cmake_target}")
            cpp_raise(TargetNotFound
                "Could not find target (${_cfp_cmaize_name}, ${_cfp_cmake_target}) after find_package()"
            )
        endif()
    endforeach()

    # Default to matching NAME and BUILD_TARGET so the find_dependency() call
    # doesn't have components
    set(_cfp_build_target "${_package_name}")

    # Handle components
    set(_cfp_multi_value "COMPONENTS")
    cmake_parse_arguments(
        _cfp
        "${_cfp_options}" "${_cfp_one_value}" "${_cfp_multi_value}"
        ${_cfp_UNPARSED_ARGUMENTS}
    )

    # If components were given, we want find_dependency() called with them
    if(NOT "${_cfp_COMPONENTS}" STREQUAL "")
        set(_cfp_build_target "${_cfp_COMPONENTS}")
    endif()

    # Add all targets to CMaize
    foreach(_cfp_cmaize_name ${_cfp_cmaize_names})
        cpp_map(GET "${_cfp_target_map}"
            _cfp_cmake_target "${_cfp_cmaize_name}"
        )

        # cmaize_wrap_target("${_cfp_cmaize_name}" "${_cfp_cmake_target}")

        cmaize_wrap_target("${_cfp_cmaize_name}" "${_cfp_cmake_target}"
            NAME "${_package_name}"
            BUILD_TARGET "${_cfp_build_target}"
        )
    endforeach()

    # foreach(_cfp_component ${_cfp_COMPONENTS})
    #     set(_cfp_cmaize_name "${_package_name}_${_cfp_component}")
    #     cmaize_wrap_target("${_cfp_cmaize_name}" "${_cfp_component}"
    #         NAME "${_package_name}"
    #         BUILD_TARGET "${_cfp_component}"
    #     )
    # endforeach()

endfunction()
