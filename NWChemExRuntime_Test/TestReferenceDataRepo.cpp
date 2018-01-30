#include <NWChemExRuntime/ReferenceDataRepo.hpp>
#include <catch/catch.hpp>

using namespace NWXRuntime;
using namespace LibChemist;

TEST_CASE("Default isotope")
{
    ReferenceDataRepo wiki;
    auto H = wiki.atomic_info(1);
    REQUIRE(H.property(AtomProperty::Z) == Approx(1.0));
    REQUIRE(H.property(AtomProperty::mass) == Approx(1.0079750000000001));
    REQUIRE(H.property(AtomProperty::isotope_mass) == Approx(1.00782503220000));
}

TEST_CASE("Exotic isotope")
{
    ReferenceDataRepo wiki;
    auto H = wiki.atomic_info(1, 2);
    REQUIRE(H.property(AtomProperty::Z) == Approx(1.0));
    REQUIRE(H.property(AtomProperty::mass) == Approx(1.0079750000000001));
    REQUIRE(H.property(AtomProperty::isotope_mass) == Approx(2.0141017781000001));
}

TEST_CASE("Getting a physical constant")
{
    ReferenceDataRepo wiki;
    REQUIRE(wiki.physical_constant(Constant::alpha) == Approx(7.2973525664e-3));
}

TEST_CASE("Made-up data")
{
    typename ReferenceDataRepo::atomic_info_map_type atoms;
    typename ReferenceDataRepo::physical_constants_map_type consts;
    detail_::AtomicInfo fake_h;
    fake_h.props[AtomProperty::mass] = 999.9;
    consts[Constant::h] = 1.0;
    atoms[1]=fake_h;
    ReferenceDataRepo wiki(atoms, consts);

    auto H = wiki.atomic_info(1);
    REQUIRE(H.property(AtomProperty::mass) == Approx(999.9));
    REQUIRE(wiki.physical_constant(Constant::h) == Approx(1.0));
}
