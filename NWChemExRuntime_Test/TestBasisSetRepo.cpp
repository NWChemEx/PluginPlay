#include <NWChemExRuntime/BasisSetRepo.hpp>
#include <NWChemExRuntime/MoleculeRepo.hpp>
#include <catch/catch.hpp>

using namespace NWXRuntime;

TEST_CASE("Default repo can apply cc-pvdz to water")
{
    auto h2o = MoleculeRepo().get_molecule("water");
    BasisSetRepo bse;
    auto h2o_w_basis = bse.apply_basis("cc-pvdz", h2o);
    auto bs = h2o_w_basis[0].get_basis("cc-pvdz");
    REQUIRE(bs.size() == 5);
    REQUIRE(bs.max_am() == 1);
}
