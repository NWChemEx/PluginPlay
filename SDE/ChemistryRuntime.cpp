#include "SDE/ChemistryRuntime.hpp"
#include "SDE/BasisSetFileParser.hpp"

namespace SDE {
    ChemistryRuntime::molecule_type ChemistryRuntime::apply_basis(const std::string& key, 
                                                ChemistryRuntime::molecule_type mol) const {
        auto charge = LibChemist::Atom::Property::charge;
        for(auto& atomi : mol.atoms) {
            const size_type Z = std::lround(atomi.properties.at(charge));
            atomi.bases[key] = bse.at(Z).bases.at(key);
        }
        return mol;
    }

    void ChemistryRuntime::load_basis_from_file(const std::string& file_path, const std::string& key, 
                         const BasisSetFileParser& parser) {
        auto fs = std::ifstream(file_path);
        auto bs = parse_basis_set_file(fs, parser, *this);
        for (const auto& x : bs)
            bse[x.first].bases[key] = x.second;
    }
    void ChemistryRuntime::load_basis_from_file(const std::string& file_path, 
                                     const BasisSetFileParser& parser)
    { load_basis_from_file(file_path,file_path,parser); }
}//End namespace
