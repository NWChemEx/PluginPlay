#include "SDE/MoleculeFileParser.hpp"
#include <regex>

namespace SDE {

using action_type = MoleculeFileParser::action_type;
using data_type   = MoleculeFileParser::data_type;
using return_type = std::map<data_type, std::vector<double>>;

namespace detail_ {
struct atom {
    double Z = 0.0;
    std::array<double, 3> xyz;
};

void commit_atom(LibChemist::SetOfAtoms& rv, atom& a) {
    if(a.Z != 0.0) { rv.insert(create_atom(a.xyz, a.Z)); }
    a = atom();
}

void parse(const return_type& data, atom& a) {
    if(data.count(data_type::AtNum)) a.Z = data.at(data_type::AtNum)[0];
    if(data.count(data_type::x)) a.xyz[0] = data.at(data_type::x)[0];
    if(data.count(data_type::y)) a.xyz[1] = data.at(data_type::y)[0];
    if(data.count(data_type::z)) a.xyz[2] = data.at(data_type::z)[0];
}

} // end namespace detail_

SetOfAtoms parse_molecule_file(std::istream& is,
                                 const SetOfAtomsFileParser& parser,
                                 const ptable_t& periodic_table) {
    SetOfAtoms rv;
    detail_::atom a;
    while(is) {
        std::string line;
        std::getline(is, line);
        switch(parser.worth_parsing(line)) {
            case(action_type::none): // Junk line
            {
                break;
            }
            case(action_type::new_atom): {
                detail_::commit_atom(rv, a);
                // Intentional fall_through
            }
            case(action_type::same_atom): {
                auto data = parser.parse(line);
                detail_::parse(data, a);
                break;
            }
            default: {
                auto data = parser.parse(line);
                if(data.count(data_type::charge))
                    rv.charge = data.at(data_type::charge)[0];
                if(data.count(data_type::multiplicity))
                    rv.multiplicity = data.at(data_type::multiplicity)[0];
                break;
            }
        }
    }
    detail_::commit_atom(rv, a);
    return rv;
}

// charge and multiplicity
static const std::regex xyz_cm("^\\s*-?\\d+.?\\d*\\s*\\d+.?\\d*\\s*$");
static const std::regex xyz_atom("^\\s*[a-zA-Z]+\\s*(?:-?\\d*.?\\d*\\s*){3}$");

action_type XYZParser::worth_parsing(const std::string& line) const {
    if(std::regex_search(line, xyz_cm))
        return action_type::overall_system;
    else if(std::regex_search(line, xyz_atom)) {
        return action_type::new_atom;
    }
    return action_type::none;
}

return_type XYZParser::parse(const std::string& line) const {
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
        // First letter of atomic symbol
        std::transform(sym.begin(), sym.begin() + 1, sym.begin(), ::toupper);
        // second plus letters
        std::transform(sym.begin() + 1, sym.end(), sym.begin() + 1, ::tolower);
        double temp = detail_::sym2Z_.at(sym);
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
