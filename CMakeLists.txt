cmake_minimum_required(VERSION 3.14)
project(sde VERSION 1.0.0)

set(
    CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${PROJECT_SOURCE_DIR}/cmake"
    CACHE STRING "" FORCE
)

include(get_cpp)

### Options ###
option(BUILD_TESTING "Should we build the tests?" ON)
option(BUILD_PYBINDINGS "Should we build Python3 bindings?" ON)

cpp_find_or_build_dependency(
        utilities
        URL github.com/NWChemEx-Project/utilities
        PRIVATE TRUE
        BUILD_TARGET utilities
        FIND_TARGET nwx::utilities
        CMAKE_ARGS BUILD_TESTING=OFF
)

cpp_find_or_build_dependency(
    cereal
    URL github.com/USCiLab/cereal
    BUILD_TARGET cereal
    FIND_TARGET cereal
    CMAKE_ARGS JUST_INSTALL_CEREAL=ON
               SKIP_PORTABILITY_TEST=ON
)


cpp_find_or_build_dependency(
    bphash
    URL github.com/ryanmrichard/BPHash
    BUILD_TARGET bphash
    FIND_TARGET bphash::bphash
    BRANCH cmake_patch
    CMAKE_ARGS BUILD_TESTING=OFF
)


if(BUILD_PYBINDINGS)
    find_package(Python3 REQUIRED)
    list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
    find_package(Cppyy REQUIRED)
    cppyy_make_python_package(sde NAMESPACE sde)
endif()


cpp_add_library(
    sde
    SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/sde"
    INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/sde"
    DEPENDS nwx::utilities cereal bphash::bphash
)

if(BUILD_TESTING)
    cpp_find_or_build_dependency(
        Catch2
        URL github.com/catchorg/Catch2
        BUILD_TARGET Catch2
        FIND_TARGET Catch2::Catch2
    )
    cpp_add_tests(
        test_sde
        SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/tests"
        INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/sde"
        DEPENDS Catch2::Catch2 sde
    )
    if(BUILD_PYBINDINGS)
	set(
        py_sde_tests tests/module tests/examples/module_base
                                  tests/examples/property_type
                                  tests/examples/workflow
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
endif()


