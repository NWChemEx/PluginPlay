#pragma once
#include "sde/bounds_checking.hpp"
#include "sde/detail_/sde_any.hpp"
#include "sde/types.hpp"
#include <optional>
#include <typeindex>
#include <utilities/containers/case_insensitive_map.hpp>

namespace sde::detail_ {

/** @brief Implements the ModuleInput class
 *
 *  This class contains the details of how the ModuleInput class actually works
 *  whereas the ModuleInput class largely defines the API that users go through
 *  to use a ModuleInput. The fact that it lives in the detail_ namespace means
 *  that you shouldn't be directly using this class.
 *
 */
class ModuleInputPIMPL {
public:
    /// The type of the type-erased functors used for checking a value
    using any_check = std::function<bool(const type::any&)>;

    /** @brief Makes a copy of the PIMPL on the heap.
     *
     *  This function is used to make a deep copy of the PIMPL, which is
     *  allocated on the heap.
     *
     *  @return A unique_ptr to the new instance.
     *
     *  @throw std::bad_alloc if there is insufficient memory to copy the
     *                        instance. Strong throw guarantee.
     */
    auto clone() const { return std::make_unique<ModuleInputPIMPL>(*this); }

    /** @brief Is this input bound to a value?
     *
     *  When an input field is declared for a module the developer may choose to
     *  bind a default value to that field. Later users may bind a different
     *  value to the field. This function determines if a value is bound to this
     *  input or not. It does not distinguish between the developer binding that
     *  value or a user.
     *
     *  @return true if this input is set to a value and false otherwise.
     *
     *  @throw none No throw guarantee.
     */
    bool has_value() const noexcept { return m_value_.has_value(); }

    /** @brief Checks whether the type of the input has been set.
     *
     *  Ultimately the input is stored in a type-erased manner. For various
     *  reasons we store the RTTI of the type (e.g., so we can do runtime type
     *  checking and automatic documentation generation). This function is used
     *  to determine if the type has been set already.
     *
     *  @return True if the type has been set and false otherwise
     *
     *  @throw none No throw guarantee.
     */
    bool has_type() const noexcept { return m_type_.has_value(); }

    /** @brief Does this input have a description?
     *
     *  Developers are encouraged to provide a human-readable description of
     *  what this option does and what it is used for. This function determines
     *  whether or not the developer actually did this.
     *
     * @return True if the developer has set a description for this input and
     *         false otherwise.
     *
     * @throws none No throw guarantee.
     */
    bool has_description() const noexcept { return m_desc_.has_value(); }

    /** @brief Does this input need to hava a value for the module to run?
     *
     *  While most inputs to a module are required in the sense that the module
     *  can not run without them having a value. Some inputs need not have a
     *  value set in order for the module to run. A typical example is the level
     *  of debug printing. While it is usually possible to define some default
     *  value to indicate that an optional input value has not been set/turned
     *  on, allowing the developer to tag it as optional saves them the trouble.
     *  Optional input values are considered always ready and will not raise an
     *  error if the module is invoked without setting it.
     *
     *  @return true if this input is optional and false if it is required.
     *
     *  @throw none No throw guarantee.
     *
     */
    bool is_optional() const noexcept { return m_optional_; }

    /** @brief Should the input's value influence memoization?
     *
     *  One of the key features of the SDE is memoization. Memoization requires
     *  us to know when two function calls are guaranteed to return the same
     *  value. While hashing all inputs to a function will guarantee this, it is
     *  often the case that some input values do not actually affect the result.
     *  A good example is the debug printing level. The result of calling the
     *  module will usually be the same whether or not debug information is
     *  printed to a log. An input value that does **NOT** affect memoization is
     *  said to be transparent. If an input value does affect memoization it is
     *  said to be opaque.
     *
     * @return True if this input is transparent and false otherwise.
     *
     * @throw none No throw guarantee.
     */
    bool is_transparent() const noexcept { return m_transparent_; }

    /** @brief Checks if an input value is ready to be given to a module.
     *
     *  An input is "ready" if it is optional (in which case the user does not
     *  need to set it) or if it has a value. Strictly speaking in order for the
     *  input to be ready it must:
     *
     *  - be optional or have a value,
     *  - the value must be of the correct type, and
     *  - the value must be within the domain of the module
     *
     *  However, this class will not let the user (or developer) set the
     *  input to a bad value. Consequentially, we are guaranteed that if this
     *  input stores a value, that value is within the domain of the module and
     *  of the correct type.
     *
     * @return true if the input is ready and false otherwise.
     *
     * @throw none No throw guarantee.
     */
    bool is_ready() const noexcept { return is_optional() || has_value(); }

    /** @brief Used to check if the input can be set to specific value
     *
     *  When a user wants to change a value the SDE needs to know if that value
     *  is valid. This function can be used to determine if a specific value
     *  is of the correct type and is within the domain of the module.
     *
     *  @param[in] new_value The value we want to set the input to.
     *
     *  @return True if @p new_value represents a valid value and false
     *          otherwise.
     *
     *  @throw std::runtime_error if the type of the input has not been set by
     *                            the developer. Strong throw guarantee.
     */
    bool is_valid(const type::any& new_value) const;

    /** @brief Hashes the current input value if it is opaque.
     *
     *  If this instance is opaque this function will pass the value to the
     *  hasher. If the value is transparent this function does nothing.
     *
     *  @param[in,out] h The object to use for hashing the value. If the current
     *                   instance is opaque @p h 's internal state will be
     *                   updated with a hash of this instance.
     *
     *  @throws none If this instance is transparent this function is no throw
     *               guarantee.
     *  @throws ??? If this instance is opaque and hashing of the value throws.
     *              Same guarantee.
     */
    void hash(type::hasher& h) {
        if(!is_transparent()) h(m_value_);
    }

    /** @brief Adds a bounds check to the input.
     *
     *  In order to avoid module developers having to perform bounds checks on
     *  their input values inside their modules, the SDE allows developers to
     *  associate functors with the input. When the input value is set or
     *  changed it is run through each of these functors and an error is raised
     *  if that value is fails one or more bounds checks.
     *
     *  @param[in] check The functor implementing the check.
     *  @param[in] desc A human-readable description of what the check is
     *                  looking for. If the description is empty this function
     *                  will attempt to autogenerate one for you.
     *
     *  @throw std::invalid_argument if the instance contains a value and that
     *                               value fails @p check.
     *                               Strong throw guarantee.
     */
    void add_check(any_check check, type::description desc = "");

    void change(type::any new_value) {
        if(!is_type_set()) throw std::runtime_error("Must set type first");
        m_value.swap(new_value);
    }

private:
    /// The value bound to this input
    type::any m_value_;

    /// A human-readable description of this input
    std::optional<type::description> m_desc_ = "";

    /// Is this input optional?
    bool m_optional_ = false;

    /// Is this input transparent?
    bool m_transparent_ = false;

    /// A map of bounds check descriptions to bounds checks
    utilities::CaseInsensitiveMap<any_check> m_checks_;

    /// The type of this input
    std::optional<std::type_index> m_type_;
};

/** @brief Compares two ModuleInputPIMPL instances for equality
 *
 *  Two ModuleInputPIMPL instances are equivalent if their states are
 *  equivalent. This includes the input value they hold, as well as the
 *  meta-data such as their descriptions and whether they are transparent. As a
 *  disclaimer, there is no feasible way to compare the checks so they are
 *  omitted from this comparison.
 *
 *  @param[in] lhs The instance on the left side of the operator
 *  @param[in] rhs The instance on the right side of the operator
 *
 *  @return True if the requested comparison is true and false otherwise.
 *
 *  @throw none No throw guarantee.
 */
bool operator==(const ModuleInputPIMPL& lhs,
                const ModuleInputPIMPL& rhs) noexcept;

/** @brief Determines if two ModuleInputPIMPL instances are different
 *
 *  Two ModuleInputPIMPL instances are equivalent if their states are
 *  equivalent. This includes the input value they hold, as well as the
 *  meta-data such as their descriptions and whether they are transparent. As a
 *  disclaimer, there is no feasible way to compare the checks so they are
 *  omitted from this comparison.
 *
 *  @param[in] lhs The instance on the left side of the operator
 *  @param[in] rhs The instance on the right side of the operator
 *
 *  @return False if the instances are equal and true otherwise.
 *
 *  @throw none No throw guarantee.
 */
bool operator!=(const ModuleInputPIMPL& lhs,
                const ModuleInputPIMPL& rhs) noexcept;

//-----------------------------Implementations----------------------------------

inline bool ModuleInputPIMPL::is_valid(const type::any& new_value) const {
    if(!has_type()) throw std::runtime_error("Must set type first");
    for(const auto & [k, v] : m_checks_)
        if(!v(new_value)) return false;
    return true;
}

inline void ModuleInputPIMPL::add_check(any_check check,
                                        type::description desc) {
    if(desc.empty()) {
        try {
            desc = bounds_checking::print_bounds(check);
        } catch(...) {
            // Bounds check can fail b/c of allocation, just use empty string
            // in that case
        }
    }
    if(has_value())
        if(!check(m_value_))
            throw std::invalid_argument("Value failed provided bounds check");
    m_checks_.emplace(std::move(desc), std::move(check));
}

inline bool operator==(const ModuleInputPIMPL& lhs,
                       const ModuleInputPIMPL& rhs) noexcept {
    return std::tie(m_value, m_desc, m_optional, m_transparent, m_type) ==
           std::tie(rhs.m_value, rhs.m_desc, rhs.m_optional, rhs.m_transparent,
                    rhs.m_type);
}

inline bool operator!=(const ModuleInputPIMPL& lhs,
                       const ModuleInputPIMPL& rhs) noexcept {
    return !(lhs == rhs);
}

} // namespace sde::detail_
