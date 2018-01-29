#include <NWChemExRuntime/PhysicalConstants.hpp>

#include <catch/catch.hpp>

/** @file Tests of fundamental physical constants to be used in
 *        NWChemEx
 *
 *  @warning This file made by generate_constants.py.
 *           DO NOT EDIT!!!!!!!!
 */

using namespace NWXRuntime;
using namespace NWXRuntime::detail_;
TEST_CASE("Fundamental Constants")
{
    REQUIRE(constants.at(Constant::Cal2J) == Approx(4.1840000000000002e+00));
    REQUIRE(constants.at(Constant::pi) == Approx(3.1415926535897931e+00));
    REQUIRE(constants.at(Constant::alpha) == Approx(7.2973525663999998e-03));
    REQUIRE(constants.at(Constant::c) == Approx(2.9979245800000000e+08));
    REQUIRE(constants.at(Constant::h) == Approx(6.6260700399999999e-34));
    REQUIRE(constants.at(Constant::kb) == Approx(1.3806485199999999e-23));
    REQUIRE(constants.at(Constant::me) == Approx(9.1093835599999998e-31));
    REQUIRE(constants.at(Constant::na) == Approx(6.0221408570000002e+23));
    REQUIRE(constants.at(Constant::qe) == Approx(1.6021766208000001e-19));
}
TEST_CASE("Derived Constants")
{
    REQUIRE(constants.at(Constant::hbar) == Approx(1.0545718001391127e-34));
    REQUIRE(constants.at(Constant::e0) == Approx(8.8541878173856056e-12));
    REQUIRE(constants.at(Constant::kc) == Approx(8.9875517876064968e+09));
    REQUIRE(constants.at(Constant::Eh) == Approx(4.3597446510117192e-18));
    REQUIRE(constants.at(Constant::a0) == Approx(5.2917721054980892e-11));
    REQUIRE(constants.at(Constant::R) == Approx(8.3144598614485812e+00));
    REQUIRE(constants.at(Constant::Hz2J) == Approx(6.6260700399999999e-34));
    REQUIRE(constants.at(Constant::inv_m2J) == Approx(1.9864458241717582e-25));
}
