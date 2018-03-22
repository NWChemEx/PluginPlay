#include "SDE/ChemistryRuntime.hpp"
#include "SDE/BasisSetFileParser.hpp"
#include <fstream>


namespace SDE {
    ChemistryRuntime::molecule_type apply_basis(const std::string& key,
                                                const ChemistryRuntime& crt,
                                                ChemistryRuntime::molecule_type mol) {
        auto charge = LibChemist::Atom::Property::charge;
        for(auto& atomi : mol.atoms) {
            const ChemistryRuntime::size_type Z = std::lround(atomi.properties.at(charge));
            atomi.bases[key] = crt.bse.at(Z).bases.at(key);
        }
        return mol;
    }

    ChemistryRuntime load_basis_from_file(const std::string& file_path,
                                          const std::string& key,
                                          const BasisSetFileParser& parser,
                                          ChemistryRuntime crt) {
        auto fs = std::ifstream(file_path);
        auto bs = parse_basis_set_file(fs, parser, crt);
        for (const auto& x : bs)
            crt.bse[x.first].bases[key] = x.second;
        return crt;
    }
    ChemistryRuntime load_basis_from_file(const std::string& file_path,
                                          const BasisSetFileParser& parser,
                                          ChemistryRuntime crt) {
        return load_basis_from_file(file_path,file_path,parser,crt); }
}//End namespace
