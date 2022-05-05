#pragma once
#include "pluginplay/any/any.hpp"
#include "pluginplay/types.hpp"
#include <optional>
#include <typeindex>
#include <utilities/containers/case_insensitive_map.hpp>

namespace pluginplay::detail_ {

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
    /// The type used to store the rtti
    using rtti_type = std::type_index;

    /// The type of the type-erased functors used for checking a value
    using any_check = std::function<bool(const type::any&)>;

    /// The type used to return the descriptions of the bounds checks
    using check_description_type = std::set<type::description>;

    /** @brief Constructs the PIMPL for a null input.
     *
     *  The resulting input has no type, value, or description. It is by default
     *  required and opaque (the latter two defaults being chosen to align with
     *  what is presumed to be the most common defaults for those properties).
     *
     *  @throw none No throw guarantee.
     *
     */
    ModuleInputPIMPL() noexcept = default;

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
     *  One of the key features of the pluginplay is memoization. Memoization
     * requires us to know when two function calls are guaranteed to return the
     * same value. While hashing all inputs to a function will guarantee this,
     * it is often the case that some input values do not actually affect the
     * result. A good example is the debug printing level. The result of calling
     * the module will usually be the same whether or not debug information is
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
     *  When a user wants to change a value the pluginplay needs to know if that
     * value is valid. This function can be used to determine if a specific
     * value is of the correct type and is within the domain of the module.
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

    /// Setters
    ///@{
    /** @brief Sets the RTTI for this input field.
     *
     *  This function is used to specify what the type of this input field
     *  should be. This function must be called before a value is set and cannot
     *  be used to change the type after a value has been set. In practice calls
     *  to this function will be hard-coded in a module's ctor and thus having
     *  multiple calls to it is nonsensical.
     *
     * @param[in] type The RTTI of this input field.
     *
     * @throw std::runtime_error if the user attempts to change the type after
     *                           a value has been set. Strong throw guarantee.
     */
    void set_type(rtti_type type);

    /** @brief Binds the provided value to the present input field.
     *
     * @param[in] any The type-erased value to bind to this input value. The
     *                wrapped value must pass all bounds checks.
     *
     * @throw std::runtime_error if the type the value must have has not been
     *                           set. Strong throw guarantee.
     * @throw std::invalid_argument if @p any fails one or more bounds checks.
     *                              Strong throw gurantee.
     */
    void set_value(type::any any);

    /** @brief Sets the human-readable description of this input field.
     *
     *  For documentation and help purposes it is essential to have a human
     *  readable description of the input field. This function is intended to
     *  be used by the module developer to set that description.
     *
     * @param[in] desc A brief description of what this input is used for.
     *
     * @throw none No throw guarantee.
     */
    void set_description(type::description desc) noexcept;

    /** @brief Adds a bounds check to the input.
     *
     *  In order to avoid module developers having to perform bounds checks on
     *  their input values inside their modules, the pluginplay allows
     * developers to associate functors with the input. When the input value is
     * set or changed it is run through each of these functors and an error is
     * raised if that value is fails one or more bounds checks.
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

    /** @brief Flags this input as optional
     *
     *  An optional input does not need to have a value bound to it in order for
     *  the module to run. Calling this function will flag the input as
     *  optional.
     *
     *  @throw none No throw guarantee.
     */
    void make_optional() noexcept { m_optional_ = true; }

    /** @brief Flags this input as required
     *
     *  An optional input does not need to have a value bound to it in order for
     *  the module to run. Calling this function will flag the input as
     *  required.
     *
     *  @throw none No throw guarantee.
     */
    void make_required() noexcept { m_optional_ = false; }

    /** @brief Flags this input as opaque
     *
     *  An opaque input's value changes the hash of the input for memoization
     *  purposes. Calling this function will flag the input as opaque.
     *
     *  @throw none No throw guarantee.
     */
    void make_opaque() noexcept { m_transparent_ = false; }

    /** @brief Flags this input as transparent
     *
     *  An opaque input's value changes the hash of the input for memoization
     *  purposes. Calling this function will flag the input as transparent (the
     *  opposite of opaque).
     *
     *  @throw none No throw guarantee.
     */
    void make_transparent() noexcept { m_transparent_ = true; }
    ///@}

    /// Getters
    ///@{
    /** @brief Returns the RTTI of this input field.
     *
     *  The exact state of the returned object is largely compiler dependent and
     *  should not be relied on for anything other than indexing and printing.
     *
     *  @return The RTTI of this input field.
     *
     *  @throw std::runtime_error if the type of this field has not been set.
     *                            Strong throw guarantee.
     */
    rtti_type type() const;

    /** @brief Returns the value bound to the input in a type-erased state.
     *
     *  This function can be used to retrieve the value bound to this input
     *  field.
     *
     *  @return A read-only reference to the pluginplayAny stored in this input.
     *
     *  @throw std::runtime_error if no value is bound to this input field.
     *                            Strong throw guarantee.
     */
    const type::any& value() const;

    /** @brief Used to retrieve the description of what this option is used for.
     *
     *  @return The human-readable description of what the module will use this
     *          input field for.
     *
     *  @throw std::bad_optional_access if the description has not been set.
     *                                  Strong throw guarantee.
     */
    const type::description& description() const { return m_desc_.value(); }

    /** @brief Returns the descriptions of the bounds checks this input is
     *         subject to.
     *
     *  This function is intended to be used primarily for documentation
     *  purposes. It will return human-readable descriptions of each bounds
     *  check. To see if a value passes these bounds checks use `is_valid`.
     *
     *  @return A container with the descriptions.
     *
     *  @throw std::bad_alloc if there is insffucient memory to make the
     *                        container.
     */
    check_description_type check_descriptions() const;
    ///@}
private:
    /// Code factorization for ensuring the type of the input is set
    void assert_type_set_() const;

    /// Code factorization for ensuring the value has been set
    void assert_value_set_() const;

    /// The value bound to this input
    type::any m_value_;

    /// A human-readable description of this input
    std::optional<type::description> m_desc_;

    /// Is this input optional?
    bool m_optional_ = false;

    /// Is this input transparent?
    bool m_transparent_ = false;

    /// A map of bounds check descriptions to bounds checks
    utilities::CaseInsensitiveMap<any_check> m_checks_;

    /// The type of this input
    std::optional<rtti_type> m_type_;
};

/** @brief Compares two ModuleInputPIMPL instances for equality
 *
 *  @relates ModuleInputPIMPL
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
 *  @relates ModuleInputPIMPL
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
    assert_type_set_();
    for(const auto& [k, v] : m_checks_)
        if(!v(new_value)) return false;
    return true;
}

inline void ModuleInputPIMPL::set_type(
  typename ModuleInputPIMPL::rtti_type type) {
    if(has_type() && has_value() && m_type_ != type)
        throw std::runtime_error("Cannot change type after value is set.");
    m_type_ = type;
}

inline void ModuleInputPIMPL::set_value(type::any any) {
    assert_type_set_();
    if(!is_valid(any)) {
        std::string msg("Input value has failed bounds checks: ");
        for(const auto& [x, y] : m_checks_) {
            if(!y(any)) msg += x + " ";
        }
        throw std::invalid_argument(msg);
    }
    m_value_.swap(any);
}

inline void ModuleInputPIMPL::set_description(type::description desc) noexcept {
    m_desc_.emplace(std::move(desc));
}

inline void ModuleInputPIMPL::add_check(any_check check,
                                        type::description desc) {
    if(has_value())
        if(!check(m_value_)) {
            const auto msg = std::string("Value failed provided bounds "
                                         "check: ") +
                             desc;
            throw std::invalid_argument(desc);
        }
    m_checks_.emplace(std::move(desc), std::move(check));
}

inline typename ModuleInputPIMPL::rtti_type ModuleInputPIMPL::type() const {
    assert_type_set_();
    return m_type_.value();
}

inline const type::any& ModuleInputPIMPL::value() const {
    assert_value_set_();
    return m_value_;
}

inline void ModuleInputPIMPL::assert_type_set_() const {
    if(!has_type()) throw std::runtime_error("Must set type first");
}

inline void ModuleInputPIMPL::assert_value_set_() const {
    if(!has_value()) throw std::runtime_error("Value has not been set");
}

inline typename ModuleInputPIMPL::check_description_type
ModuleInputPIMPL::check_descriptions() const {
    check_description_type rv;
    for(const auto& [x, _] : m_checks_) rv.insert(x);
    return rv;
}

inline bool operator==(const ModuleInputPIMPL& lhs,
                       const ModuleInputPIMPL& rhs) noexcept {
    // Check boolean properties first
    if(lhs.has_type() != rhs.has_type()) return false;
    if(lhs.has_value() != rhs.has_value()) return false;
    if(lhs.is_optional() != rhs.is_optional()) return false;
    if(lhs.is_transparent() != rhs.is_transparent()) return false;
    if(lhs.has_description() != rhs.has_description()) return false;

    if(lhs.has_type() && (lhs.type() != rhs.type())) return false;
    if(lhs.has_value() && (lhs.value() != rhs.value())) return false;
    if(lhs.has_description() && (lhs.description() != rhs.description()))
        return false;

    return true;
}

inline bool operator!=(const ModuleInputPIMPL& lhs,
                       const ModuleInputPIMPL& rhs) noexcept {
    return !(lhs == rhs);
}

} // namespace pluginplay::detail_
