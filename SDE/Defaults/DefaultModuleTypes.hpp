#pragma once
#include "SDE/Module.hpp"
#include <LibChemist/Molecule.hpp>
#include <tamm/tamm.hpp>

namespace SDE {

template<std::size_t NBases, std::size_t Deriv = 0,
         typename element_type = double>
struct Integral : ModuleBase {
    using tensor_type      = tamm::Tensor<element_type>;
    using molecule_type    = LibChemist::Molecule;
    using basis_set_type   = LibChemist::BasisSet;
    using basis_array_type = std::array<basis_set_type, NBases>;

    virtual tensor_type run(const molecule_type&, const basis_array_type&) = 0;
};

} // namespace SDE
