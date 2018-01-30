#include <NWChemExRuntime/MoleculeRepo.hpp>
#include <catch/catch.hpp>

using namespace NWXRuntime;
using namespace LibChemist;

TEST_CASE("Default has a water molecule (and Ryan's set for life)")
{
    MoleculeRepo pubchem;
    auto water = pubchem.get_molecule("water");
    REQUIRE(water.size() == 3);
    REQUIRE(water[0].property(AtomProperty::Z) == 1);
    REQUIRE(water[1].property(AtomProperty::Z) == 8);
    REQUIRE(water[2].property(AtomProperty::Z) == 1);
}
