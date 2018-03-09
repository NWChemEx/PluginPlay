#pragma once
#include "SDE/ChemistryRuntime.hpp"
#include <string>
#include <map>
#include <vector>
#include <istream>

namespace SDE {

/** @brief This class abstracts away the layout of the basis set file and the
 *  mechanism by which it's parsed.
 *
 * Unfortunately there are a ton of different basis set file types.  At the
 * end of the day they all look something like:
 *
 * @verbatim
   Header (comments, and other information)
   Start of an Atom
   Shell 1
   Shell 2
   ...
   Start of Next Atom
   Shell 1
   Shell 2
   ...
   Footer
   @endverbatim
 *
 * Here's how this is going to work.  I'm going to loop over a file and give
 * you a line.  You're going to tell me if that line is worth parsing by
 * returning one of the action_type enums: none, same_atom, new_atom.
 * Respectively, these signal the line is not worth parsing, it is worth
 * parsing and the resulting data is for the current atom, it is worth parsing
 * but the data is for a new atom. (Note the first occurrence of parsable data
 * should be signaled by new_atom).
 *
 * For every parsable line you return an std::map<data_type,double> of the
 * data you obtained from the line.
 */
struct BasisSetFileParser
{
  ///Signals for what to do with a line
  enum class action_type
  {
      none, ///< Line is not worth parsing
      new_atom,///< Line starts a new atom
      new_shell,///< Line starts a new shell
      same_shell///< Line is part of the current shell
  };

  ///Tags for the type of data we're getting back
  enum class data_type
  {
      exponent, ///< The parameter in the exponent of the Gaussian
      coefficient, ///< The weight of the Gaussian in contraction
      angular_momentum, ///< The angular momentum of the shell
      Z                 ///< The atomic number the shell function is for
  };

  /**
   * @brief Implemented by derived class to determine if a line is worth parsing
   * and if it is to tell us what action to take.
   *
   * @param line The line the derived class needs to parse.
   * @return the action that the parser should do
   * @throw ??? Same throw guarantee and conditions as derived class.
   */
  virtual action_type worth_parsing(const std::string& line)const=0;

  /**
   * @brief Implemented by derived class to parse the current line.
   * @param line The line to parse
   * @param crt The definition of atomic data
   * @return A map of the data contained in that line, along with tags detailing
   *         what it is.
   * @throw ??? Same throw guarantee and conditions as derived class.
   */
  virtual std::map<data_type,std::vector<double>>
  parse(const std::string& line, const ChemistryRuntime& crt)const=0;
};

/** @brief This class implements a BasisSetFileParser for the Gaussian94 format.
 *
 *  I'm not sure that the Gaussian94 format is written down anywhere, but
 *  here's what I've managed to piece together:
 *
 *  - comment lines start with a !
 *  - atoms are separated by ****
 *  - after **** line appears the atomic symbol of the atom and a 0
 *    - I have no idea what the 0 is, but it seems to always be there
 *  - The remainder of the current atom block follows the form
 *    - Each shell starts with the letter code of the angular momentum
 *    followed by the number of primitives and then a 1.0
 *      - Again, not sure what the 1.0 is (normalization maybe?)
 *    - Next are n primitive lines of the form exponent coefficient
 *      - Two or more coefficients appear if the shell is a hybrid shell (sp)
 */
struct G94: public BasisSetFileParser
{
  action_type worth_parsing(const std::string& line)const override;
  std::map<data_type, std::vector<double> >
  parse(const std::string& line, const ChemistryRuntime& crt)const override;
};

/** @brief The function to call to parse a BasisSetFile.
 *
 *  Given a stream containing a basis set this function will automatically
 *  run that stream through a parser for you and return the basis set as
 *  an object.
 *
 *  @param[in] is The input stream to read the file from.  The stream is
 *  expected to point to the first line to parse and will be left in an empty
 *  state.
 *  @param[in] parser An instance of the parser to use.
 *  @return A map between atomic number and the shells for that atomic number
 *  (It's a map because not all basis sets define shells for all atoms).
 *  @throw ??? Throws if any of the parser's routines throw.  Same guarantee as
 *  the parser.
 */
std::map<size_t,std::vector<LibChemist::BasisShell>>
parse_basis_set_file(std::istream& is, const BasisSetFileParser& parser,
                     const ChemistryRuntime& crt);

}//End namespace LibChemist
