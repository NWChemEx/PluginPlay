set(
    SDE_SRCS Defaults/DefaultAM.cpp Defaults/DefaultAtoms.cpp
             Defaults/DefaultBases.cpp Defaults/DefaultIsotopes.cpp
             Defaults/DefaultMols.cpp Defaults/DefaultSymbols.cpp
             Defaults/PhysicalConstants.cpp BasisSetFileParser.cpp
             ChemistryRuntime.cpp Module.cpp MoleculeFileParser.cpp
)
set(
    SDE_INCLUDES BasisSetFileParser.hpp ChemistryRuntime.hpp NWXDefaults.hpp
                 Memoization.hpp MoleculeFileParser.hpp Pythonization.hpp
                 SDEAny.hpp Serialization.hpp ModuleHelpers.hpp Module.hpp
)

if(BUILD_PYBINDINGS)
    list(APPEND SDE_SRCS PyBindings/ExportSDE.cpp PyBindings/PyModule.cpp)
    list(
         APPEND SDE_INCLUDES PyBindings/PyModule.hpp PyBindings/PySmartEnums.hpp
    )
    add_python_module(SDE "${SDE_SRCS}" "${SDE_INCLUDES}")
else()
    add_nwx_library(SDE "${SDE_SRCS}" "${SDE_INCLUDES}")
endif()
target_link_libraries(SDE LibChemist::chemist bphash::bphash cereal::cereal)

