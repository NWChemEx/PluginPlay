set(
    SDE_SRCS Defaults/DefaultAM.cpp Defaults/DefaultAtoms.cpp
             Defaults/DefaultBases.cpp Defaults/DefaultIsotopes.cpp
             Defaults/DefaultMols.cpp Defaults/DefaultSymbols.cpp
             Defaults/PhysicalConstants.cpp BasisSetFileParser.cpp
             ChemistryRuntime.cpp MoleculeFileParser.cpp
)
set(
    SDE_INCLUDES BasisSetFileParser.hpp ChemistryRuntime.hpp NWXDefaults.hpp
                 Memoization.hpp MoleculeFileParser.hpp Parameters.hpp
                 Pythonization.hpp SDEAny.hpp Serialization.hpp
)
set(depends LibChemist::chemist bphash::bphash)

if(BUILD_PYBINDINGS)
    list(APPEND depends pybind11::pybind11 pybind11::embed pybind11::module)
    list(APPEND SDE_SRCS PyBindings/PyParameters.cpp PyBindings/Pythonize.cpp)
    list(APPEND SDE_INCLUDES PyBindings/PySmartEnums.hpp)
endif()

add_nwx_library(SDE "${SDE_SRCS}" "${SDE_INCLUDES}")
target_link_libraries(SDE "${depends}")

if(BUILD_PYBINDINGS)
    target_compile_definitions(SDE PUBLIC USING_pybind11)
endif()
