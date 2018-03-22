#include "SDE/MoleculeFileParser.hpp"
#include <regex>
#include <cmath> //For lround

namespace SDE {

using action_type = MoleculeFileParser::action_type;
using data_type   = MoleculeFileParser::data_type;
using return_type = std::map<data_type, std::vector<double>>;
using Molecule = LibChemist::Molecule;
using MProperty = typename Molecule::property_key;
using AProperty = typename LibChemist::Atom::property_key;


namespace detail_ {
struct atom {
    double Z = 0.0;
    std::array<double, 3> xyz;
};

void commit_atom(LibChemist::Molecule& rv, atom& a, const ChemistryRuntime&
crt) {
    if(a.Z != 0.0) {
        auto temp = crt.periodic_table.at(a.Z);
        temp.coords = a.xyz;
        rv.atoms.push_back(temp);
    }
    a = atom();
}

void parse(const return_type& data, atom& a) {
    if(data.count(data_type::AtNum)) a.Z = data.at(data_type::AtNum)[0];
    if(data.count(data_type::x)) a.xyz[0] = data.at(data_type::x)[0];
    if(data.count(data_type::y)) a.xyz[1] = data.at(data_type::y)[0];
    if(data.count(data_type::z)) a.xyz[2] = data.at(data_type::z)[0];
}

} // end namespace detail_

Molecule parse_molecule_file(std::istream& is,
                             const MoleculeFileParser& parser,
                             const ChemistryRuntime& crt) {
    Molecule rv;
    detail_::atom a;
    double charge{0.0};
    double mult{1.0};
    while(is) {
        std::string line;
        std::getline(is, line);
        switch(parser.worth_parsing(line)) {
            case(action_type::none) : {break;} //Junk line
            case(action_type::new_atom): {
                detail_::commit_atom(rv, a, crt);
                // Intentional fall_through
            }
            case(action_type::same_atom): {
                auto data = parser.parse(line, crt);
                detail_::parse(data, a);
                break;
            }
            default: {
                auto data = parser.parse(line, crt);
                if(data.count(data_type::charge))
                    charge = data.at(data_type::charge)[0];
                if(data.count(data_type::multiplicity))
                    mult = data.at(data_type::multiplicity)[0];
                break;
            }
        }
    }
    detail_::commit_atom(rv, a, crt);
    rv.properties[MProperty::multiplicity] = mult;
    long nelectrons = -1 * std::lround(charge);
    //Can't use nelectrons() because nalpha/nbeta not set yet.
    for(const auto& ai : rv.atoms)
        if(LibChemist::is_real_atom(ai))//"Charge" only refers to electrons
            nelectrons += std::lround(ai.properties.at(AProperty::charge));
    const long nopen = std::lround(mult) - 1;
    const long nclosed = nelectrons - nopen;
    if(nclosed%2) {
        auto msg = "Charge: " + std::to_string(charge) + "Multiplicity: " +
          std::to_string(mult) + "not possible for " +
          std::to_string(nelectrons) + " system.";
        throw std::domain_error(msg);
    }
    rv.properties[MProperty::nbeta] = nclosed/2;
    rv.properties[MProperty::nalpha] = nclosed/2 + nopen;
    return rv;
}

// charge and multiplicity
static const std::regex xyz_cm("^\\s*-?\\d+.?\\d*\\s+\\d+.?\\d*\\s*$");
static const std::regex xyz_atom("^\\s*[a-zA-Z]+\\s*(?:-?\\d*.?\\d*\\s*){3}$");

action_type XYZParser::worth_parsing(const std::string& line) const {
    if(std::regex_search(line, xyz_cm))
        return action_type::overall_system;
    else if(std::regex_search(line, xyz_atom))
        return action_type::new_atom;
    return action_type::none;
}

return_type XYZParser::parse(const std::string& line,
                             const ChemistryRuntime& crt) const {
    return_type rv;
    std::stringstream tokenizer(line);
    if(std::regex_search(line, xyz_cm)) {
        double temp;
        tokenizer >> temp;
        rv[data_type::charge].push_back(temp);
        tokenizer >> temp;
        rv[data_type::multiplicity].push_back(temp);
    } else if(std::regex_search(line, xyz_atom)) {
        std::string sym;
        tokenizer >> sym;
        double temp = crt.at_sym_2_Z.at(sym);
        rv[data_type::AtNum].push_back(temp);
        tokenizer >> temp;
        rv[data_type::x].push_back(temp);
        tokenizer >> temp;
        rv[data_type::y].push_back(temp);
        tokenizer >> temp;
        rv[data_type::z].push_back(temp);
    }
    return rv;
}

} // namespace LibChemist
