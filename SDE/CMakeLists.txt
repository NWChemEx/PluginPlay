set(
        SDE_SRCS Defaults/DefaultAM.cpp Defaults/DefaultAtoms.cpp
        Defaults/DefaultBases.cpp Defaults/DefaultIsotopes.cpp
        Defaults/DefaultMols.cpp Defaults/DefaultSymbols.cpp
        Defaults/PhysicalConstants.cpp BasisSetFileParser.cpp
        ChemistryRuntime.cpp MoleculeFileParser.cpp
)
set(
        SDE_INCLUDES BasisSetFileParser.hpp ChemistryRuntime.hpp NWXDefaults.hpp
        Memoization.hpp MoleculeFileParser.hpp Pythonization.hpp
        Parameters.hpp SDEAny.hpp Serialization.hpp
)

if(BUILD_PYBINDINGS)
    list(APPEND SDE_SRCS PyBindings/ExportSDE.cpp PyBindings/PyParameters.cpp)
    list(
            APPEND SDE_INCLUDES PyBindings/PySmartEnums.hpp 
                                PyBindings/PyParameters.hpp
    )
    add_python_module(
            NAME SDE
            SOURCES ${SDE_SRCS}
            DEPENDS LibChemist::chemist bphash::bphash cereal::cereal
    )
    target_compile_definitions(SDE PUBLIC USING_pybind11)
else()
    add_nwx_library(
            NAME SDE
            SOURCES ${SDE_SRCS}
            DEPENDS LibChemist::chemist bphash::bphash cereal::cereal
    )
endif()
install_targets(TARGETS SDE INCLUDES ${SDE_INCLUDES})
