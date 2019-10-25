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

#if(BUILD_PYBINDINGS)
#    add_python_module(
#            NAME py_sde_utils
#            SOURCES PysdeUtils.cpp
#            DEPENDS sde
#    )
#    if(APPLE)
#        set_target_properties(py_sde_utils PROPERTIES SUFFIX .so)
#    endif()
#    add_python_test(NAME TestsdeAny)
#    add_python_test(NAME TestParameters)
#    add_python_test(NAME TestModule)
#    add_python_test(NAME TestModuleManager)
#endif()
