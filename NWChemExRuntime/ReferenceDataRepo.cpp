#include "NWChemExRuntime/ReferenceDataRepo.hpp"
#include <string>
#include <algorithm>
#include <iterator>

namespace NWXRuntime {

using size_type = typename ReferenceDataRepo::size_type;

ReferenceDataRepo::ReferenceDataRepo():
        ReferenceDataRepo(detail_::atomic_data_, detail_::constants)
{}

LibChemist::Atom ReferenceDataRepo::atomic_info(const std::string& sym,
                                        size_type iso) const
{
    return atomic_info(sym2Z(sym), iso);
}

LibChemist::Atom ReferenceDataRepo::atomic_info(size_type Z, size_type iso)const
{
    auto info = atom_info_.at(Z);
    if(iso)
        info.props[LibChemist::AtomProperty::isotope_mass] =
                info.isotopes.at(iso).mass;
    return LibChemist::Atom({0.0, 0.0, 0.0}, info.props);
}

typename ReferenceDataRepo::size_type ReferenceDataRepo::sym2Z(std::string sym)const
{
    std::string sym_lowercase;
    std::back_insert_iterator<std::string> itr(sym_lowercase);
    std::transform(sym.begin(), sym.end(), itr, ::tolower);
    return detail_::sym2Z_.at(sym_lowercase);
}

}
