#pragma once
#include <LibChemist/BasisShell.hpp>
#include <LibChemist/SetOfAtoms.hpp>
#include <map>

namespace NWXRuntime {

/**
 * @brief A class holding a list of pre-defined basis sets.
 *
 * The primary purpose of this class is to decouple the definition of basis sets
 * from the act of applying them to molecules.  By default this class comes
 * hard-coded with several popular basis sets.  Users are free to add additional
 * basis sets at runtime.  In particular, BasisSetParser's output can be fed
 * more or less into a BasisSetRepo instance (the hiccup being you'll have to
 * attach a string name to the output of the parser).
 *
 * Design decision:
 *
 * Like the other repo classes this one too is a somewhat dressed-up map.  The
 * logic here is that eventually this class can be used to automate additional
 * aspects of the process of applying basis sets to molecules (selection of
 * auxiliary basis sets, normalization, etc.), while still maintaining a uniform
 * application process.  It also hides the case-sensitivity convention of the
 * data.
 */
class BasisSetRepo {
public:
    ///The type of a basis set provided to this class as input
    using input_basis_type =
        std::map<size_t, std::vector < LibChemist::BasisShell>>;

    /**
     * @brief Makes an instance that is initialized with the default set of
     * basis sets.
     * @throw std::bad_alloc if there is insufficient memory to load the
     * initial basis sets.
     */
    BasisSetRepo();

    /**
     * @brief Makes a deep copy of another instance.
     * @param[in] rhs The instance to copy.
     * @throw std::bad_alloc if there is insufficient memory for the copy. Strong
     *        throw guarantee.
     */
    BasisSetRepo(const BasisSetRepo& /*rhs*/) = default;

    /**
     * @brief Takes ownership of another instance.
     * @param[in] rhs The instance to take ownership of.  After this call @p rhs
     *            is in a valid, but otherwise undefined state.
     * @throw None. No throw guarantee.
     */
    BasisSetRepo(BasisSetRepo&& /*rhs*/)noexcept = default;

    /**
     * @brief Allows a user to initialize a BasisSetRepo instance with their
     *        own preferred set of basis sets.
     *
     * @param bases A map of basis sets where the key is a descriptive name of
     * the basis set (the name will be used for look-up) and the value is the
     * basis set. Each of the basis sets should be provided as a map between
     * atomic number and the shells on an atom with that number.
     *
     * @throw std::bad_alloc if their is insufficient memory to copy the basis
     * sets over.  Strong throw guarantee.
     */
    BasisSetRepo(const std::map<std::string, input_basis_type>& bases) :
            bases_(bases)
    { }

    /**
     * @brief Cleans up any memory held by the current instance.
     * @throw None. No throw guarantee.
     *
     */
    ~BasisSetRepo()noexcept = default;

    /**
     * @brief Assigns a deep copy of another instance's state to the current
     *        instance.
     * @param[in] rhs The instance to copy.
     * @return The current instance containing a deep-copy of @p rhs's state.
     * @throw std::bad_alloc if there is insufficient memory for the copy. Strong
     *        throw guarantee.
     */
    BasisSetRepo& operator=(const BasisSetRepo& /*rhs*/)= default;

    /**
     * @brief Takes ownership of another instance.
     * @param[in] rhs The instance to take ownership of.  After this call @p rhs
     *            is in a valid, but otherwise undefined state.
     * @return The current instance containing @p rhs's state.
     * @throw None. No throw guarantee.
     */
    BasisSetRepo& operator=(BasisSetRepo&& /*rhs*/)noexcept= default;

    /**
     * @brief Allows a user to add additional basis sets to the instance.
     *
     *
     * @param name The tag that should be used to retrieve the basis set.
     * @param bs The basis set to store under the tag.
     *
     * @warning Note that if @p name is an already existing basis set then
     * this call will override it with @p bs.
     * @throw std::bad_alloc if there is insufficient memory to copy the basis
     * set over.  Strong throw guarantee (??? need to check std::map).
     */
    void add_basis(const std::string& name, const input_basis_type& bs)
    {
        bases_[name] = bs;
    }

    /**
     * @brief Applies a specific basis set to a molecule.
     *
     * @note If a particular basis set does not define basis functions for all
     * atoms in your molecule, no additional functions will be applied to that
     * atom.
     *
     * @param name The name of the basis set to apply.  If no basis set exists
     *        with the specific key the molecule is returned as is.
     * @param mol The molecule to apply the basis set to.
     * @return A copy of @p mol containing
     * @throw std::bad_alloc If there is insufficient memory to copy @p mol or
     * to copy the shells over.  Weak throw guarantee.
     */
    LibChemist::SetOfAtoms apply_basis(const std::string& name,
                                       const LibChemist::SetOfAtoms& mol)const;

private:

    ///The basis sets this instance is aware of.
    std::map<std::string, input_basis_type> bases_;

};

}
