set(
        SDE_TESTS test_main.cpp
        detail_/TestModuleInputPIMPL.cpp
        detail_/TestModuleManagerPIMPL.cpp
        detail_/TestModulePIMPL.cpp
        detail_/TestModuleResultPIMPL.cpp
        detail_/TestPropertyTypeBuilder.cpp
        detail_/TestSDEAny.cpp
        detail_/TestSubmoduleRequestPIMPL.cpp
        examples/TestModuleBase.cpp
        examples/TestPropertyType.cpp
        examples/TestWorkflow.cpp
        TestModule.cpp
        TestModuleInput.cpp
        TestModuleManager.cpp
        TestModuleResult.cpp
        TestSubmoduleRequest.cpp
        TestUtility.cpp
#        TestCache.cpp
#        #	      TestMemoization.cpp
)

cpp_add_executable(
        test_sde
        SOURCES ${SDE_TESTS}
        DEPENDS sde Catch2::Catch2
)
target_include_directories(test_sde PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
add_test(NAME test_sde COMMAND test_sde)
if(BUILD_PYBINDINGS)
    foreach(py_test_i TestModuleBase TestWorkflow)
        set(file ${CMAKE_CURRENT_SOURCE_DIR}/examples/${py_test_i}.py)
        add_test(NAME Py${py_test_i} COMMAND ${PYTHON_EXECUTABLE} ${file})
        set_tests_properties(
           Py${py_test_i}
           PROPERTIES ENVIRONMENT "PYTHONPATH=${CMAKE_BINARY_DIR}"
        )
    endforeach()
endif()
