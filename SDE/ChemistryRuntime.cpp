#include "SDE/ChemistryRuntime.hpp"
#include "SDE/BasisSetFileParser.hpp"
#include "ChemistryRuntime.hpp"

namespace SDE {
ChemistryRuntime::molecule_type apply_basis(ChemistryRuntime::molecule_type mol,
                                            const std::string& key,
                                            const ChemistryRuntime::indexed_atom_type& bse) {
    auto charge = LibChemist::Atom::Property::charge;
    for(auto& atomi : mol.atoms) {
        const ChemistryRuntime::size_type Z = std::lround(atomi.properties.at(charge));
        atomi.bases[key] = bse.at(Z).bases.at(key);
    }
    return mol;
}

ChemistryRuntime::molecule_type apply_basis_istream(ChemistryRuntime::molecule_type mol,
                                                    const std::string& key,
                                                    std::istream& is,
                                                    const BasisSetFileParser& parser,
                                                    const ChemistryRuntime& crt) {
    auto bs = parse_basis_set_file(is, parser, crt);
    auto charge = LibChemist::Atom::Property::charge;
    for(auto& atomi : mol.atoms) {
        const ChemistryRuntime::size_type Z = std::lround(atomi.properties.at(charge));
        atomi.bases[key] = bs.at(Z);
    }
    return mol;
}
}//End namespace
