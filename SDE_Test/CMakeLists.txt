set(
    SDE_TESTS test_main.cpp
              TestParameters.cpp
              TestSDEAny.cpp
              TestModuleHelpers.cpp
              TestModule.cpp
              TestModuleManager.cpp
)

add_catch_cxx_tests(
        NAME test_SDE
        SOURCES ${SDE_TESTS}
        DEPENDS SDE
)
if(BUILD_PYBINDINGS)
    add_python_module(
            NAME py_sde_utils
            SOURCES PySDEUtils.cpp
            DEPENDS SDE
    )
    if(APPLE)
        set_target_properties(py_sde_utils PROPERTIES SUFFIX .so)
    endif()
    add_python_test(NAME TestSDEAny)
    add_python_test(NAME TestParameters)
    add_python_test(NAME TestModule)
    add_python_test(NAME TestModuleManager)
endif()
