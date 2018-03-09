#include <SDE/MoleculeFileParser.hpp>
#include <SDE/NWXDefaults.hpp>
#include <catch/catch.hpp>
#include <sstream>

using namespace LibChemist;
using namespace SDE;


// A fake xyz example
std::string xyz_example = " -6.7 3\n"
  "         He 0.1 .1 0.0    \n"
  " HE 1.1 0.1 0.0\n";

std::string h2o_example =
  "3\n"
    "\n"
    "H    0.000000000000000   1.579252144093028   2.174611055780858\n"
    "O    0.000000000000000   0.000000000000000   0.000000000000000\n"
    "H    0.000000000000000   1.579252144093028  -2.174611055780858\n"
    " \n";

TEST_CASE("SetOfAtoms parsing") {
    std::array<double, 3> carts1({0.1, 0.1, 0.0}), carts2({1.1, 0.1, 0.0});
    std::array<Atom, 2> Hes({create_atom(carts1, 2), create_atom(carts2, 2)});
    SetOfAtoms corr;
    corr.insert(Hes[0]);
    corr.insert(Hes[1]);
    corr.charge       = -6.7;
    corr.multiplicity = 3.0;

    std::stringstream ss(xyz_example);
    SetOfAtoms mol = parse_SetOfAtoms_file(ss, XYZParser());
REQUIRE(corr == mol);

SetOfAtoms corr_h2o;
corr_h2o.insert(create_atom(
  {0.000000000000000, 1.579252144093028, 2.174611055780858}, 1));
corr_h2o.insert(create_atom(
  {0.000000000000000, 0.000000000000000, 0.000000000000000}, 8));
corr_h2o.insert(create_atom(
  {0.000000000000000, 1.579252144093028, -2.174611055780858}, 1));

std::stringstream ss3(h2o_example);
SetOfAtoms h2o = parse_SetOfAtoms_file(ss3, XYZParser());
REQUIRE(corr_h2o == h2o);

}
