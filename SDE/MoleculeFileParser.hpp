#pragma once
#include "SDE/ChemistryRuntime.hpp"
#include <istream>
#include <map>
#include <string>
#include <vector>

/** @file This file contains the machinery for parsing a string representation
 *  of a set of atoms.
 *
 * Unfortunately, like basis sets, there are a plethora of formats for
 * representing collections of atoms.  We again break the parsing into two
 * phases an examination of the line, and a data obtaining phase.  For the
 * former phase we have the following actions:
 * - none: the line is not worth parsing
 * - new_atom: the line contains information for a new atom
 * - same_atom: the line contains information for the current atom
 * - overall_system: the line contains information for the overall system
 *
 * If your parser signals the line is worth parsing you will be given the line
 * again and this time you will need to return the data the line contains. The
 * choices at the moment are:
 * - AtNum: the atomic number of the atom
 * - x: the x-coordinate of the atom (you are expected to ensure it is in a.u.
 * - y: same as x except for the y-coordinate
 * - z: same as x and y except for the z-coordinate
 * - charge: the overall system charge
 * - multiplicity: the overall system multiplicity
 *
 * @TODO The following data pieces will be needed eventually, but have not been
 * coded up:
 * - q: the charge of a point charge
 * - point: a dummy atom
 * - ghost: a ghost atom
 * - UCSides: the sides of the unit cell
 * - UCAngles: the angles of the unit cell
 * - SpaceGroup: which space group the unit cell belongs to.
 */

namespace SDE {

/** @brief This class abstracts away the layout of a string representation of a
 *  Molecule.
 *
 *  This is the base class for all classes specifying the layout of a
 *  Molecule file.
 */
struct MoleculeFileParser {
    enum class action_type { none, new_atom, same_atom, overall_system };
    enum class data_type { AtNum, x, y, z, charge, multiplicity };
    virtual action_type worth_parsing(const std::string& line) const = 0;
    virtual std::map<data_type, std::vector<double>> parse(
      const std::string& line, const ChemistryRuntime& crt) const = 0;
};

/** @brief This class implements a MoleculeParser for the xyz format.
 *
 */
struct XYZParser : public MoleculeFileParser {
    action_type worth_parsing(const std::string& line) const override;
    std::map<data_type, std::vector<double>> parse(
      const std::string& line, const ChemistryRuntime& crt) const override;
};

/**
 * @brief The function to call to parse a MoleculeFile.
 *
 * @param[in] is An input stream containing a string representation of a
 *               Molecule instance in a format the parser understands.
 * @param[in] parser The parser to be used to parse the input stream.
 * @param[in] crt The definition of the chemistry runtime we're using
 * @returns The Molecule instance represented in the input stream.
 * @throws std::domain_error if the charge and multiplicity of the molecule is
 * in consistent.  Weak throw guarantee for @p is and strong throw for all other
 * parameters.
 * @throws std::bad_alloc if there is insufficient memory to allocate the new
 * molecule. Weak throw guarantee for @p is and strong throw for all other
 * parameters.
 */
LibChemist::Molecule parse_molecule_file(std::istream& is,
    const MoleculeFileParser& parser, const ChemistryRuntime& crt);

} // namespace LibChemist
