set(
        SDE_TESTS test_main.cpp TestBasisFileParser.cpp TestChemistryRuntime.cpp
        TestMoleculeFileParser.cpp TestParameters.cpp TestSDEAny.cpp TestModuleHelpers.cpp
              TestModule.cpp
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
    add_python_test(NAME TestSDEAny)
    add_python_test(NAME TestParameters)
    add_python_test(NAME TestModule)
endif()
