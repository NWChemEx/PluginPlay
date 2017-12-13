#pragma once

/** @file Fundamental physical constants for NWChemEx
 *
 *  Generally speaking, the idea is to define as few constants as
 *  possible and derive all others.  This helps to maintain consistency.

 *  @warning Data made by generate_constants.py.
 *           DO NOT EDIT!!!!!!!!
 */

namespace NWXRuntime {
namespace constant {
//Fundamental physical constants

///Ratio of a Joule to a calorie
constexpr double J2cal = 4.184;

///Ratio of circle's circumference to it's diameter
constexpr double pi = 3.14159265358979323846;

///Fine-structure constant (dimensionless)
constexpr double alpha = 7.2973525664e-3;

///Speed of light in vacuum in meters per second
constexpr double c = 299792458;

///Plank's constant in Joule seconds
constexpr double h = 6.626070040e-34;

///Boltzmann's constant in Joules per Kelvin
constexpr double kb = 1.38064852e-23;

///Rest mass of electron in kilograms
constexpr double me = 9.10938356e-31;

///Avogadro's constant in per mole
constexpr double na = 6.022140857e23;

///Charge of electron in Coulombs
constexpr double qe = 1.6021766208e-19;

//Derived constants

///Reduced Plank constant Joule seconds
constexpr double hbar = (h)/(2.0*pi);

///Vacuum permittivity Coulomb^2/(Joule Meter)
constexpr double e0 = (qe*qe)/(2.0*alpha*h*c);

///Coulomb's constant Joule meter/ Coulomb^2
constexpr double kc = (1.0)/(4.0*pi*e0);

///Hartree constant in Joules
constexpr double Eh = (alpha*alpha*me*c*c)/(1.0);

///Bohr radius in meters
constexpr double a0 = (hbar)/(me*alpha*c);

}}//End namespaces
