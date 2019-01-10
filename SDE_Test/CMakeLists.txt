set(
        SDE_TESTS test_main.cpp
        detail_/TestSDEAny.cpp
        detail_/TestPropertyTypeBuilder.cpp
        examples/TestModuleBase.cpp
        examples/TestPropertyType.cpp
        TestModuleInput.cpp
        TestModuleOutput.cpp
#        TestCache.cpp
#        TestComputedProperties.cpp
#        TestParameters.cpp
#        TestProperty.cpp
#        TestSDEAny.cpp
#        #	      TestMemoization.cpp
#        TestModuleHelpers.cpp
#        TestModule.cpp
#        TestModuleManager.cpp
#        TestSubmoduleRequestForm.cpp
)

cpp_add_executable(
        test_sde
        SOURCES ${SDE_TESTS}
        DEPENDS sde Catch2::Catch2
)
target_include_directories(test_sde PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
add_test(NAME test_sde COMMAND test_sde)
#if(BUILD_PYBINDINGS)
#    add_python_module(
#            NAME py_sde_utils
#            SOURCES PySDEUtils.cpp
#            DEPENDS SDE
#    )
#    if(APPLE)
#        set_target_properties(py_sde_utils PROPERTIES SUFFIX .so)
#    endif()
#    add_python_test(NAME TestSDEAny)
#    add_python_test(NAME TestParameters)
#    add_python_test(NAME TestModule)
#    add_python_test(NAME TestModuleManager)
#endif()
