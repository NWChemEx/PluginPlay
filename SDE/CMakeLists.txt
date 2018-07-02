set(SDE_SRCS Defaults/DefaultAM.cpp
                         Defaults/DefaultAtoms.cpp
                         Defaults/DefaultBases.cpp
                         Defaults/DefaultIsotopes.cpp
                         Defaults/DefaultMols.cpp
                         Defaults/DefaultSymbols.cpp
                         Defaults/PhysicalConstants.cpp
                         BasisSetFileParser.cpp
                         ChemistryRuntime.cpp
                         MoleculeFileParser.cpp
        )
set(SDE_INCLUDES BasisSetFileParser.hpp
                             ChemistryRuntime.hpp
                             NWXDefaults.hpp
                             Memoization.hpp
                             MoleculeFileParser.hpp
                             SDEAny.hpp
                             Serialization.hpp
        )
add_nwx_library(SDE "${SDE_SRCS}" "${SDE_INCLUDES}")
target_link_libraries(SDE LibChemist::chemist bphash::bphash)
