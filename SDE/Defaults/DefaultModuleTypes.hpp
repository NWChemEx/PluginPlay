#pragma once
#include "SDE/Module.hpp"
#include <LibChemist/Molecule.hpp>
#include <Utilities/Containers/CaseInsensitiveMap.hpp>
#include <tamm/tamm.hpp>

namespace SDE {

/**
 * @brief Class for holding the details related to a particular set of orbitals.
 *
 * We assume that the orbitals within this class can be expanded as a linear
 * combination of atomic-orbitals
 *
 * Further considerations:
 * - Rely on TAMM for spin or hold alpha and beta versions?
 * - Need identity matrix class (should be such that TAMM skips evaluations
 *   involving it) to allow for the resulting orbitals to be non-orthogonal and
 *   to ensure the AO space can be treated as a special case of this class
 *   (Cdagger == C == 1)
 *
 *
 * @tparam element_type
 */
template<typename element_type = double>
struct OrbitalSpace {
    using tensor_type = tamm::Tensor<element_type>;
    using basis_type  = LibChemist::BasisSet;

    // AO basis set
    std::shared_ptr<basis_type> basis;

    // Overlap of AO basis
    std::shared_ptr<tensor_type> S;

    // Density matrix in the AO basis set
    tensor_type density;

    // Conversion from current orbital basis to AO basis
    tensor_type Cdagger;

    // Conversion from AO basis to current basis
    tensor_type C;
};

/**
 * @brief The base class for modules that build tensors filled with AO
 * integrals.
 *
 * This class is envisioned as wrapping builds of full AO integral tensors.
 *
 * @tparam NBases The total number of AO basis sets involved in the integral.
 * @tparam Deriv What derivative, w.r.t. electron position, should we compute?
 * @tparam element_type The type of the element in the tensor
 */
template<std::size_t NBases, std::size_t Deriv = 0,
         typename element_type = double>
struct AOIntegral : ModuleBase {
    using tensor_type      = tamm::Tensor<element_type>;
    using molecule_type    = LibChemist::Molecule;
    using basis_set_type   = LibChemist::BasisSet;
    using basis_array_type = std::array<basis_set_type, NBases>;

    // Multiple tensors for things like dipoles?
    virtual tensor_type run(const molecule_type&, const basis_array_type&) = 0;
};

/**
 * @brief The base class for modules that build the core Hamiltonian in the AO
 * basis set.
 *
 * @tparam Deriv What derivative, w.r.t. electron position, should we compute?
 * @tparam element_type The type of the element in the tensor
 */
template<std::size_t Deriv = 0, typename element_type = double>
struct CoreHamiltonian : ModuleBase {
    using tensor_type    = tamm::Tensor<element_type>;
    using molecule_type  = LibChemist::Molecule;
    using basis_set_type = LibChemist::BasisSet;
    // Is there ever more than one???
    virtual tensor_type run(const molecule_type&, const basis_set_type& bra,
                            const basis_set_type& ket) = 0;
};

/**
 * @brief The base class for modules that build the J and K matrices in the AO
 * basis set.
 *
 * In order to take advantage of integral re-use J and K are typically built
 * together.  This class is designed to build a series of Js and Ks given a
 * series of densities.
 *
 * @tparam Deriv What derivative, w.r.t. electron position, should we compute?
 * @tparam element_type The type of the element in the tensor
 */
template<std::size_t Deriv = 0, typename element_type = double>
struct JKMatrices : ModuleBase {
    using tensor_type    = tamm::Tensor<element_type>;
    using molecule_type  = LibChemist::Molecule;
    using basis_set_type = LibChemist::BasisSet;
    using tensor_map     = Utilities::CaseInsensitiveMap<tensor_type>;

    virtual tensor_map run(const molecule_type& mol, const OrbitalSpace& MOs,
                           const basis_set_type& bra,
                           const basis_set_type& ket) = 0;
};

/**
 * @brief The base class for modules that build Fock matrices in the AO
 * basis set.
 *
 *
 * @tparam Deriv What derivative, w.r.t. electron position, should we compute?
 * @tparam element_type The type of the element in the tensor
 */
template<std::size_t Deriv = 0, typename element_type = double>
struct FockBuilder : ModuleBase {
    using tensor_type    = tamm::Tensor<element_type>;
    using molecule_type  = LibChemist::Molecule;
    using basis_set_type = LibChemist::BasisSet;
    using tensor_map     = Utilities::CaseInsensitiveMap<tensor_type>;

    virtual tensor_map run(const molecule_type& mol, const OrbitalSpace& MOs,
                           const basis_set_type& bra,
                           const basis_set_type& ket) = 0;
};

} // namespace SDE
