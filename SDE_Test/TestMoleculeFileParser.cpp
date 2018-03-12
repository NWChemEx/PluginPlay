#include <SDE/MoleculeFileParser.hpp>
#include <SDE/NWXDefaults.hpp>
#include <catch/catch.hpp>
#include <sstream>

using namespace LibChemist;
using namespace SDE;
using Property = Molecule::Property;

TEST_CASE("Normal XYZ") {
    std::string h2o_example =
    "3\n"
    "\n"
    "H    0.000000000000000   1.579252144093028   2.174611055780858\n"
    "O    0.000000000000000   0.000000000000000   0.000000000000000\n"
    "H    0.000000000000000   1.579252144093028  -2.174611055780858\n"
    " \n";
    auto crt = default_runtime();
    auto H = crt.periodic_table[1];
    auto H2{H};
    auto O = crt.periodic_table[8];
    H.coords = {0.000000000000000, 1.579252144093028, 2.174611055780858};
    H2.coords = {0.000000000000000, 1.579252144093028, -2.174611055780858};
    O.coords = {0.000000000000000, 0.000000000000000, 0.000000000000000};
    Molecule corr_h2o;
    corr_h2o.atoms = {H, O, H2};
    corr_h2o.properties[Property::nalpha] = 5.0;
    corr_h2o.properties[Property::nbeta] = 5.0;
    corr_h2o.properties[Property::multiplicity] = 1.0;
    std::stringstream ss3(h2o_example);
    Molecule h2o = parse_molecule_file(ss3, XYZParser(), crt);
    REQUIRE(corr_h2o == h2o);
}


TEST_CASE("Hardest XYZ parsing") {
    std::string xyz_example = " -2 3\nHe 0.1 .1 0.0\nHE 1.1 0.1 0.0\n";
    auto crt = default_runtime();
    std::array<double, 3> carts1(), carts2();
    auto He{crt.periodic_table.at(2)};
    auto He2{He};
    He.coords = {0.1, 0.1, 0.0};
    He2.coords = {1.1, 0.1, 0.0};
    Molecule corr;
    corr.atoms = {He, He2};
    corr.properties[Property::nalpha] = 2.0;
    corr.properties[Property::nbeta] = 0.0;
    corr.properties[Property::multiplicity] = 3.0;

    std::stringstream ss(xyz_example);
    Molecule mol = parse_molecule_file(ss, XYZParser(), crt);
    REQUIRE(corr == mol);



}
