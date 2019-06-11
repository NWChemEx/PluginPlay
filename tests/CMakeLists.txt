set(sde_TESTS test_main.cpp
              detail_/module_input_pimpl.cpp
              detail_/module_manager_pimpl.cpp
              detail_/module_pimpl.cpp
              detail_/module_result_pimpl.cpp
              detail_/property_type_builder.cpp
              detail_/sde_any.cpp
              detail_/submodule_request_pimpl.cpp
              examples/cxx_workflow.cpp
              module.cpp
              module_input.cpp
              module_manager.cpp
              module_result.cpp
              submodule_request.cpp
              utility.cpp
#        TestCache.cpp
#        #	      TestMemoization.cpp
)

cpp_add_executable(
        test_sde
        SOURCES ${sde_TESTS}
        DEPENDS sde Catch2::Catch2
)
target_include_directories(test_sde PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
add_test(NAME test_sde COMMAND test_sde)


if(BUILD_PYBINDINGS)
	set(
        py_sde_tests module examples/module_base
                            examples/property_type
                            examples/python_workflow
    )
    foreach(test_name ${py_sde_tests})
    add_test(
        NAME py_${test_name}
        COMMAND Python3::Interpreter
                ${CMAKE_CURRENT_SOURCE_DIR}/${test_name}.py
    )
    set_tests_properties(
        py_${test_name}
        PROPERTIES ENVIRONMENT "PYTHONPATH=${CMAKE_BINARY_DIR}"
    )
    endforeach()
endif()
