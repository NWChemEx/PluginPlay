#include "SDE/ChemistryRuntime.hpp"
#include "SDE/BasisSetFileParser.hpp"

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

ChemistryRuntime load_basis(std::istream& is,
                            const std::string& key,
                            const BasisSetFileParser& parser,
                            ChemistryRuntime crt) {
    auto bs = parse_basis_set_file(is, parser, crt);
    for (const auto& x : bs)
        crt.bse[x.first].bases[key] = x.second;
    return crt;
}
}//End namespace
