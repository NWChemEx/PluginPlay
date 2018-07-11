#pragma once
#include <LibChemist/Molecule.hpp> // Includes Atom.hpp and BasisShell.hpp
#include <Utilities/Containers/CaseInsensitiveMap.hpp> // Includes <string>
#include <cmath>                                       //for lround

namespace SDE {

/**
 * @brief Structure for aggregating details pertaining to a physical constant.
 *
 * Physical constants are for our purposes are both the usual things like pi,
 * Planck's constant, the rest mass of the electron... as well as unit
 * conversion ratios like calories to Joules, Hartrees to Joules...
 *
 * Users wanting to provide their own constants/conversions should define a
 * series of PhysicalConstant instances for each constant/conversion.  Note SI
 * prefixes are defined by C++ in <ratio>.
 *
 * @par Implementation notes:
 * - I considered std::ratio for the values; however, things like Planck's
 *   constant are not expressible as the ratio of two integers without overflow
 * - As an aggregate, brace-initialization is preferred.
 * - For now I have decided having the units as a string is sufficient.  If a
 *   full fledged units library is made it should be preferred.
 */
struct PhysicalConstant {
    /// The actual value of the constant
    double value;

    /// A string describing the units of the constant
    std::string units;

    /// A string providing a description of the constant
    std::string description;
};

/**
 *  @brief Class for aggregating data pertaining to isotopic data for an
 *  element.
 *
 *  Users wanting to provide their own isotope data should create instances of
 *  this class.
 */
struct IsotopeData {
    /// The isotope number
    std::size_t isotope_number;

    /// The actual mass (in Daltons)
    double mass;

    /// The abundance of the isotope
    double abundance;
};

/** @brief Collects constants, conversion factors, atomic data, basis sets, etc.
 *         into a one-stop-shop.
 *
 *  This class is meant to aggregate chemical (and physical) data that we know
 *  about at runtime.  All parts of the code will use the values within this
 *  class to populate system instances and perform necessary conversions etc.
 *  A user should thus be able to customize the fundamental constants etc.
 *  used throughout the code by controlling their values within the instance of
 *  this class forwarded throughout the code.
 *
 *  @par Implementation Notes
 *  - strings are preferred as keys to avoid prohibiting users from adding
 *    constants, molecules, etc. that are not part of the standard NWX sets
 */
struct ChemistryRuntime {
    /// The type of a physical constant or ratio
    using constant_type = PhysicalConstant;

    /// The type of a struct holding isotope data
    using isotope_type = IsotopeData;

    /// The type of an atom
    using atom_type = LibChemist::Atom;

    /// The type used to store atomic numbers and otherwise index things
    using size_type = std::size_t;

    /// The type of a basis set
    using indexed_atom_type = std::map<size_type, atom_type>;

    /// The type of a list of isotopes
    using isotope_list = std::vector<isotope_type>;

    /// The type of the object used to look up physical constants
    using constant_lut_type = Utilities::CaseInsensitiveMap<constant_type>;

    /// The type of the object used to look up the atomic number by symbol
    using at_sym_lut_type = Utilities::CaseInsensitiveMap<size_type>;

    /// The type of the object used to convert angular momentum symbols to
    /// values
    using am_sym_lut_type = Utilities::CaseInsensitiveMap<int>;

    /// The type of the object used to look up isotopes
    using isotope_lut_type = std::map<size_type, isotope_list>;

    /// Wikipedia is a great place to find the value of physical constants
    constant_lut_type wikipedia;

    /// The periodic table houses basic properties about an element
    indexed_atom_type periodic_table;

    /// A list of known isotopes for each element
    isotope_lut_type isotopes;

    /// Conversion from an atomic symbol to the atomic number
    at_sym_lut_type at_sym_2_Z;

    /// Conversion from angular momentum letter to number
    am_sym_lut_type am_sym_2_l;
};

struct BasisSetFileParser;
std::map<std::size_t, LibChemist::Atom> default_bases();

/**
 * @brief Convenience function for applying a basis set to a molecule.
 *
 * Applying a basis set to a molecule is typically one of the first things
 * a user does.  This function is meant as a convenience function
 * for doing this.  In particular it assumes that the molecule it is
 * adding a basis set to is simply a set of atoms (i.e. no ghost atoms,
 * point charges, etc.).  It also assumes that the charge of the atoms is
 * the atomic number, which it will be when an atom is first made.
 *
 * @param mol The molecule whose atoms the basis will be applied to.
 * @param key The name of the basis set to apply.
 * @param bse A map from atomic number to atoms (which themselves hold basis
 * sets).
 * @return A deep copy of @p mol containing an additional basis set on each
 * atom with the name @p key.
 * @throw std::bad_alloc if there is insufficient memory to copy the basis
 * set over.  Strong throw guarantee.
 * @throw std::out_of_range if @p key is not known to the runtime.  Strong
 * throw guarantee.
 */
LibChemist::Molecule apply_basis(
  LibChemist::Molecule mol, const std::string& key,
  const std::map<std::size_t, LibChemist::Atom>& bse = default_bases());

/**
 * @brief Convenience function to load a basis set from an istream and apply it
 * to a Molecule @p mol.
 *
 * @param mol The molecule whose atoms the basis will be applied to.
 * @param key The name to use for the basis set on the molecule.
 * @param is the input stream to read the basis set from.
 * @param parser format used to parse the basis set file.
 * @param crt ChemistryRuntime required for the parser.
 * @return A deep copy of @p mol containing an additional basis set on each
 * atom with the name @p key.
 */
LibChemist::Molecule apply_basis_istream(LibChemist::Molecule mol,
                                         const std::string& key,
                                         std::istream& is,
                                         const BasisSetFileParser& parser,
                                         const ChemistryRuntime& crt);

} // namespace SDE
