set(
    SDE_TESTS test_main.cpp TestBasisFileParser.cpp TestChemistryRuntime.cpp
              TestMoleculeFileParser.cpp TestSDEAny.cpp
)

add_catch_cxx_tests(test_SDE "${SDE_TESTS}" SDE)

function(add_python_test __name)
    configure_file(${__name}.py ${__name}.py COPYONLY)
    add_test(NAME Py${__name} COMMAND python3 ${__name}.py)
endfunction()

if(BUILD_PYBINDINGS)
    #Builds a Python module containing utilities for testing the SDE
    add_library(py_sde_utils MODULE PySDEUtils.cpp)
    target_link_libraries(py_sde_utils SDE)
    #Undo Hunter's defaults
    set_target_properties(py_sde_utils PROPERTIES PREFIX "")
    set_target_properties(py_sde_utils PROPERTIES DEBUG_POSTFIX "")
    set_target_properties(py_sde_utils PROPERTIES RELEASE_POSTFIX "")

    add_python_test(TestSDEAny)
endif()
