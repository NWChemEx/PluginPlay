#pragma once
#include <LibChemist/BasisShell.hpp>
#include <map>

namespace NWXRuntime {

/**
 * @brief When people need Gaussian basis sets they turn to EMSL's Basis Set
 * exchange.  This class strives to perform a similar service by providing
 * basis sets to the NWChemEx program.
 *
 * Like the Wikipedia class, this class is meant to be the API people go through
 * to get basis sets on their objects.  The details of how one defines a basis
 * set are hidden inside the class and thus decoupled from the act of getting
 * it.
 *
 * By default this class will load several popular basis sets and make them
 * available for users.  Users are free to add additional basis sets at runtime.
 * In particular, BasisSetParser's output can be fed more or less into a
 * BasisSetExchange instance (the hiccup being you'll have to attach a string to
 * the output).
 */
class BasisSetExchange {
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
    BasisSetExchange();

    /**
     * @brief Makes a deep copy of another instance.
     * @param[in] rhs The instance to copy.
     * @throw std::bad_alloc if there is insufficient memory for the copy. Strong
     *        throw guarantee.
     */
    BasisSetExchange(const BasisSetExchange& /*rhs*/) = default;

    /**
     * @brief Takes ownership of another instance.
     * @param[in] rhs The instance to take ownership of.  After this call @p rhs
     *            is in a valid, but otherwise undefined state.
     * @throw None. No throw guarantee.
     */
    BasisSetExchange(BasisSetExchange&& /*rhs*/)noexcept = default;

    /**
     * @brief Allows a user to initialize a BasisSetExchange instance with their
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
    BasisSetExchange(const std::map<std::string, input_basis_type>& bases) :
            bases_(bases)
    { }

    /**
     * @brief Cleans up any memory held by the current instance.
     * @throw None. No throw guarantee.
     *
     */
    ~BasisSetExchange()noexcept = default;

    /**
     * @brief Assigns a deep copy of another instance's state to the current
     *        instance.
     * @param[in] rhs The instance to copy.
     * @return The current instance containing a deep-copy of @p rhs's state.
     * @throw std::bad_alloc if there is insufficient memory for the copy. Strong
     *        throw guarantee.
     */
    BasisSetExchange& operator=(const BasisSetExchange& /*rhs*/)= default;

    /**
     * @brief Takes ownership of another instance.
     * @param[in] rhs The instance to take ownership of.  After this call @p rhs
     *            is in a valid, but otherwise undefined state.
     * @return The current instance containing @p rhs's state.
     * @throw None. No throw guarantee.
     */
    BasisSetExchange& operator=(BasisSetExchange&& /*rhs*/)noexcept= default;

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

private:

    ///The basis sets this instance is aware of.
    std::map<std::string, input_basis_type> bases_;

};

}
