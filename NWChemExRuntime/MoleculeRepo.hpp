#pragma once
#include <LibChemist/SetOfAtoms.hpp>
#include <map>
#include <string>

namespace NWXRuntime {

/**
 * @brief MoleculeRepo is intended to store pre-built molecules.
 * 
 * It is impractical to even begin to attempt to store every possible molecule
 * that users may want to run calculations on.  That said, particularly for
 * testing purposes or for applications where the focus is more on the theory
 * (as opposed to the particular chemical system) it is useful to have some
 * pre-built molecule instances.  This class stores said instances. Obviously
 * there are many factors that influence a geometry so usage of the ones
 * contained within an instance of this class should be done with a grain of
 * salt.
 *
 * At the moment this is little more than a wrapper over std::map; however, one
 * can imagine making this into more of a database (say tagging molecules with
 * the level of theory used to optimize them, if they are part of a benchmark
 * set, ...)
 */
class MoleculeRepo {
public:

    /**
     * @brief Makes a MoleculeRepo instance filled with the default molecules.
     *
     * @throw std::bad_alloc if there is insufficient memory to copy the
     * molecules into the current instance.
     */
    MoleculeRepo();

    /**
     * @brief Makes a deep copy of another instance.
     * @param[in] rhs The instance to copy.
     * @throw std::bad_alloc if there is insufficient memory for the copy. Strong
     *        throw guarantee.
     */
    MoleculeRepo(const MoleculeRepo& /*rhs*/) = default;

    /**
     * @brief Takes ownership of another instance.
     * @param[in] rhs The instance to take ownership of.  After this call @p rhs
     *            is in a valid, but otherwise undefined state.
     * @throw None. No throw guarantee.
     */
    MoleculeRepo(MoleculeRepo&& /*rhs*/)noexcept = default;

    /**
     * @brief Cleans up any memory held by the current instance.
     * @throw None. No throw guarantee.
     *
     */
    ~MoleculeRepo()noexcept = default;

    /**
     * @brief Assigns a deep copy of another instance's state to the current
     *        instance.
     * @param[in] rhs The instance to copy.
     * @return The current instance containing a deep-copy of @p rhs's state.
     * @throw std::bad_alloc if there is insufficient memory for the copy. Strong
     *        throw guarantee.
     */
    MoleculeRepo& operator=(const MoleculeRepo& /*rhs*/)= default;

    /**
     * @brief Takes ownership of another instance.
     * @param[in] rhs The instance to take ownership of.  After this call @p rhs
     *            is in a valid, but otherwise undefined state.
     * @return The current instance containing @p rhs's state.
     * @throw None. No throw guarantee.
     */
    MoleculeRepo& operator=(MoleculeRepo&& /*rhs*/)noexcept= default;

    void add_molecule(const std::string& name,
                      const LibChemist::SetOfAtoms& mol)
    {
        mols_[name] = mol;
    }

    LibChemist::SetOfAtoms get_molecule(const std::string& name)const
    {
        return mols_.at(name);
    }
private:

    std::map<std::string, LibChemist::SetOfAtoms> mols_;

};

}
