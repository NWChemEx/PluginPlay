#include "NWChemExRuntime/PhysicalConstants.hpp"

namespace NWXRuntime {
namespace detail_ {
extern const std::map<Constant, double> constants([]()
{
    std::map<Constant, double> temp;
    temp[Constant::Cal2J] = 4.184;
    temp[Constant::pi] = 3.14159265358979323846;
    temp[Constant::alpha] = 7.2973525664e-3;
    temp[Constant::c] = 299792458;
    temp[Constant::h] = 6.626070040e-34;
    temp[Constant::kb] = 1.38064852e-23;
    temp[Constant::me] = 9.10938356e-31;
    temp[Constant::na] = 6.022140857e23;
    temp[Constant::qe] = 1.6021766208e-19;
    temp[Constant::hbar] = 1.0545718001391127e-34;
    temp[Constant::e0] = 8.8541878173856056e-12;
    temp[Constant::kc] = 8.9875517876064968e+09;
    temp[Constant::Eh] = 4.3597446510117192e-18;
    temp[Constant::a0] = 5.2917721054980892e-11;
    temp[Constant::R] = 8.3144598614485812e+00;
    temp[Constant::Hz2J] = 6.6260700399999999e-34;
    temp[Constant::inv_m2J] = 1.9864458241717582e-25;
    return temp;
}());
}}//End namespaces
