set(
    SDE_TESTS test_main.cpp TestBasisFileParser.cpp TestChemistryRuntime.cpp
              TestMoleculeFileParser.cpp TestSDEAny.cpp
)

add_catch_cxx_tests(test_SDE "${SDE_TESTS}" SDE)
