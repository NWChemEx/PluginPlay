#pragma once

/**
 *  @file PhysicalConstants.hpp
 *
 * Contains enumerations for the recognized physical constants
 */

#include <map>
namespace NWXRuntime {
enum class Constant{
    Cal2J, ///< Ratio of a Joule to a calorie
    pi, ///< Ratio of circle's circumference to it's diameter
    alpha, ///< Fine-structure constant (dimensionless)
    c, ///< Speed of light in vacuum in meters per second
    h, ///< Plank's constant in Joule seconds
    kb, ///< Boltzmann's constant in Joules per Kelvin
    me, ///< Rest mass of electron in kilograms
    na, ///< Avogadro's constant in things per mole
    qe, ///< Charge of electron in Coulombs
    hbar, ///< Reduced Plank constant Joule seconds
    e0, ///< Vacuum permittivity Coulomb^2/(Joule Meter)
    kc, ///< Coulomb's constant Joule meter/ Coulomb^2
    Eh, ///< Hartree constant in Joules
    a0, ///< Bohr radius in meters
    R, ///< Ideal gas constant Joules per Kelvin per mole
    Hz2J, ///< Conversion from Hertz to Joules
    inv_m2J, ///< Conversion from m^-1 to Joules
};

namespace detail_ {
extern const std::map<Constant, double> constants;
}
}
