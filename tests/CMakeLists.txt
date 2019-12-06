add_subdirectory(detail_)
add_subdirectory(examples)
add_subdirectory(printing)
list(APPEND sde_TESTS test_main.cpp
              bounds_checking.cpp
              module.cpp
              module_base.cpp
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
                            examples/workflow
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
