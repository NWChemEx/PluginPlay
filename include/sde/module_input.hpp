#pragma once
#include "sde/bounds_checking.hpp"
#include "sde/detail_/sde_any.hpp"
#include "sde/types.hpp"
#include "sde/utility.hpp"
#include <utilities/printing/demangler.hpp>
#include <functional>
#include <set>
#include <string>

namespace sde {
namespace detail_ {
class ModuleInputPIMPL;
}

/** @brief Holds an input value for a module in a type-erased form.
 *
 *  When a module gets its input parameters they come as instances of this
 *  class. As part of its state, the ModuleInput class contains an SDEAny
 *  instance, which wraps the value. The value can be retrieved via the `value`
 *  member, assuming the caller knows the correct type. In addition to the
 *  actual value, this class also stores various metadata about the input
 *  including:
 *
 *  - a description of what the input will be used for,
 *  - bounds checks that the value must satisfy,
 *  - if the value is optional (*i.e.*, the module works even if the value is
 *    not set),
 *  - if the value is transparent (*i.e.*, the vale does not change the output
 *    of the module, for example a printing threshold),
 *
 *  Features:
 *
 *  Bounds Checking. Any time the value of the input is changed the new value
 *  is passed through each bounds check. If a bounds check fails, changing the
 *  value fails. This ensures that the input is always valid and prevents the
 *  module from having to do these checks manually. Minimally bounds checking
 *  will ensure that the value is of the correct type.
 *
 */
class ModuleInput {
private:
    /// Helper type for declaring a reference wrapper around an object of type T
    template<typename T>
    using cref_wrapper = std::reference_wrapper<const std::decay_t<T>>;

public:
    /// Type of a container containing the bounds check descriptions
    using bounds_check_desc_t = std::set<type::description>;

    /// Type of a functor that can be used to check a value
    template<typename T>
    using validity_check = std::function<bool(const T&)>;

    /// Type of a check that operates on a type-erased value
    using any_check = validity_check<type::any>;

    /** @brief Makes a new, null ModuleInput instance
     *
     *  The instance resulting from this call will have no type, value, or
     *  metadata (description, opaqueness, etc.). This data can be set using
     *  the class's provided setters.
     *
     *  @throw std::bad_alloc if there is insufficient memory to create the
     *                        PIMPL instance. Strong throw guarantee.
     */
    ModuleInput();

    ModuleInput(const ModuleInput& rhs);

    ModuleInput& operator=(const ModuleInput& rhs);

    ModuleInput(ModuleInput&& rhs) noexcept;

    ModuleInput& operator=(ModuleInput&& rhs) noexcept;

    /** @brief Standard destructor.
     *
     *  This deletes the state of the instance meaning that all references to
     *  the internal state, notably the value and the description, are
     *  invalidated.
     *
     *  @throw none No throw guarantee.
     */
    ~ModuleInput() noexcept;

    /** @brief Has the type of this field been set yet?
     *
     *  In order for an input field to be used with a module it needs to know
     *  what type it is wrapping. This function is used to determine if that
     *  type has been set yet.
     *
     *  @return True if the type of this field has been set and false
     *          otherwise.
     *
     *  @throw none No throw guarantee.
     */
    bool has_type() const noexcept;

    /** @brief Does this input field have a value bound to it.
     *
     *  At any given time an input field either has a value bound to it or does
     *  not. This function is used to determine if a value has been bound to
     *  this field.
     *
     * @return True if a value has been bound to this field and false otherwise.
     *
     * @throw none No throw guarantee.
     */
    bool has_value() const noexcept;

    /** @brief Has the description of this input field been set?
     *
     *  Developers are encouraged to provide human-readable descriptions for
     *  each option detailing what that option is used for. These descriptions
     *  are then used as help strings.
     *
     *  @return True if the description of this input field has been set and
     *          false otherwise.
     *
     *  @throw None no throw guarantee.
     */
    bool has_description() const noexcept;

    /** @brief Must a value be bound to this input before it can be used?
     *
     *  Typically a module needs every input to be set to a value before the
     *  module can be run. Sometimes, however, the module is capable of running
     *  even if an input has not been set. Examples of when this can occur
     *  include inputs controlling printing levels and inputs that represent
     *  hints or seeds for an algorithm. The unifying theme among these
     *  scenarios are that the module can default to reasonable behavior without
     *  values for these options. While usually it is possible to say default
     *  the value to 0, sometimes we need a null state separate from the
     *  possible input values and that is where the optional trait comes in.
     *
     *  This function is used to determine if the current input is optional
     *
     *  @return True if the current input field is optional and false otherwise.
     *
     *  @throw none No throw guarantee.
     */
    bool is_optional() const noexcept;

    /** @brief Does this input influence memoization?
     *
     *  The SDE relies heavily on memoization in order to work. This is done
     *  automatically by hashing the inputs to a module. That said, sometimes
     *  an input to a module does not actually affect the result. A prototypical
     *  result is an input parameter controlling the debug print level. Inputs
     *  that do not affect memoization are termed "transparent". This function
     *  can be used to determine if an input is transparent or not.
     *
     *  @return True if the input is transparent and false otherwise.
     *
     *  @throw none No throw guarantee.
     */
    bool is_transparent() const noexcept;

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
    bool ready() const noexcept;

    /** @brief Determines if the provided value is a valid value.
     *
     * This function will run all of the bounds checks on a value and return
     * whether or not the provided value is a valid value, i.e., the value does
     * not fail any bounds checks.
     *
     * @tparam T The type of the value we are checking
     *
     * @param[in] new_value  The value we are checking.
     *
     * @return True if the provided value is a valid value and false otherwise.
     *
     * @throw std::runtime_error if the type of the input has not been set yet.
     *                           Strong throw guarantee.
     *
     * @throws ??? throws if any of the stored bounds checks through. Strong
     *             throw guarantee.
     */
    template<typename T>
    bool is_valid(T&& new_value) const;

    /** @brief Hashes the current instance and adds the hash to the provided
     *         hasher object.
     *
     * The current strategy for hashing in the SDE is to:
     *
     * - make a hasher object,
     * - pass it to everything you want to hash, and
     * - finalize the hasher
     *
     * Upon finalization the hasher returns the hash of the collective state of
     * objects passed to it. This function allows ModuleInput instances to be
     * hashed by the hasher object. The present input will only be hashed if
     * it is opaque.
     *
     * @param[in,out] h The hasher object that is in the process of hashing
     *                  stuff. If the current instance is opaque it will be
     *                  added to @p h's internal state.
     *
     * @throws ??? If the hash function for the wrapped value throws. Same
     *             guarantee.
     */
    void hash(type::hasher& h) const;

    /** @brief Sets the type of this input field to @p T.
     *
     *  Each input field for a module has a type associated with it. This is
     *  the type that the value a user provides must have. This function allows
     *  you to set the type of the field associated with this ModuleInput
     *  instance. The value can be:
     *
     *  - can be an unqualified type like `int` or `std::vector<double>`
     *  - a const-reference like `const std::vector<double>&`
     *
     *  The value should not be:
     *
     *  - A pointer (const or otherwise; use a const reference instead)
     *  - A non-const reference like `std::vector<double>&`
     *  - An rvalue reference like `std::vector<double>&&`
     *    - Just take the parameter by value and leave it up to the user to
     *      decide if they want to move the input.
     *
     *  To the extent feasible the restrictions on @p T are enforced at
     *  compile-time and will trip static assertions if violated. It is the
     *  nature of C++ that you can get around these checks, doing so is
     *  undefined behavior.
     *
     * @tparam T The type of the field. Must be an rvalue.
     *
     * @return The current object, with the type set to @p T, in order to
     *         support chaining.
     *
     * @throw std::runtime_error if you attempt to change the type after the
     *                           value has been set. Strong throw guarantee.
     */
    template<typename T>
    auto& set_type();

    /** @brief Used to change the value bound to this input field.
     *
     * @tparam T
     * @param[in] new_value The value to bind to this input field. Must satisfy
     *                      all bounds checks affiliated with this input field
     *                      in order for this function to succeed.
     *
     * @return The current ModuleInput instance with the value bound to
     *         @p new_value.
     *
     * @throw std::runtime_error if the type has not been set yet. Strong throw
     *                           guarantee.
     * @throw std::inavalid_argument if @p new_value fails any bounds checks.
     *                               Strong throw guarantee.
     */
    template<typename T>
    auto& change(T&& new_value);

    /** @brief Establishes the default value for the input field.
     *
     *  At the moment this function is nothing more than a thin wrapper around
     *  `change` meant to be more descriptive. The intent is to use this
     *  function to distinguish between a module developer setting the default
     *  value for a module and a user tweaking options to their liking (the
     *  latter of which should go through `change`).
     *
     * @tparam T The type of the input to bind.
     *
     * @param[in] new_value The  value to bind to the input as the default.
     *
     * @return The current ModuleInput instance with the default value set to
     *         @p new_value.
     *
     * @throw std::runtime_error if the type has not been set yet. Strong throw
     *                           guarantee.
     * @throw std::inavalid_argument if @p new_value fails any bounds checks.
     *                               Strong throw guarantee.
     */
    template<typename T>
    auto& set_default(T&& new_value);

    /** @brief Sets the human-readable description of this input field to
     *         @p desc.
     *
     *  Developers of modules are encouraged to associate with each input field
     *  a human-readable string, which will then be used as documentation for
     *  the input field.
     *
     *  @note Repeated calls to this function will overwrite the description and
     *        not append to it.
     *
     * @param[in] desc Description of how this input field will be used.
     *
     * @return The current ModuleInput instance modified so that it contains
     *         @p desc. The returned value is thus suitable for chaining.
     */
    ModuleInput& set_description(type::description desc) noexcept;

    /** @brief Overload for adding a pre-defined bounds check to the input
     *
     *  The SDE comes stocked with pre-defined bounds checks for a variety of
     *  situations. Developers are encouraged to use these bounds checks as they
     *  are self-documenting. This overload catches these pre-defined checks
     *  and ensures that appropriate documentation is generated for them.
     *  Developers can overwrite the autogenerated documentation by providing
     *  their own.
     *
     * @tparam T The type of the value the bounds check accepts. Must be
     *           implicitly convertible to the type of this input field.
     *
     * @param[in] check The functor that performs the bounds check.
     *
     * @param[in] desc The description of what this check is checking. If empty
     *                 a documentation string will be autogenerated. Default is
     *                 empty.
     *
     * @return The current ModuleInput with its state modified to include
     *         @p check and @p desc.
     *
     * @throw std::invalid_argument if the input field contains a value and
     *                              that value fails @p check. Strong throw
     *                              guarantee.
     * @throw ??? if the bounds check throws. Same throw guarantee as the bounds
     *            check.
     */
    template<typename T, typename U>
    auto& add_check(bounds_checking::CompareTo<T, U> check,
                    type::description desc = "");

    template<typename T>
    auto& add_check(bounds_checking::InRange<T> check,
                    type::description desc = "");

    /** @brief Overload for adding a custom bounds check to the input
     *
     *  The SDE comes stocked with pre-defined bounds checks for a variety of
     *  situations. Developers are encouraged to use these bounds checks as they
     *  are self-documenting. This overload catches developer-defined checks. No
     *  documentation for the check will be generated unless the developer
     *  provides it.
     *
     * @tparam T The type of the value the bounds check accepts. Must be
     *           implicitly convertible to the type of this input field.
     *
     * @param[in] check The functor that performs the bounds check.
     *
     * @param[in] desc The description of what this check is checking. Default
     *                 is empty.
     *
     * @return The current ModuleInput with its state modified to include
     *         @p check and @p desc.
     *
     * @throw std::invalid_argument if the input field contains a value and
     *                              that value fails @p check. Strong throw
     *                              guarantee.
     * @throw ??? if the bounds check throws. Same throw guarantee as the bounds
     *            check.
     */
    template<typename T>
    auto& add_check(validity_check<T> check, type::description desc = "");

    /** @brief Flags the current input field as optional.
     *
     *  An optional input field does not need to have a value bound to it before
     *  the module can run. Calling this function will flag this input field as
     *  optional.
     *
     * @return The current ModuleInput instance flagged as optional.
     *
     * @throw none No throw guarantee.
     */
    ModuleInput& make_optional() noexcept;

    /** @brief Flags the current input field as optional.
     *
     *  An optional input field does not need to have a value bound to it before
     *  the module can run. Calling this function will flag this input field as
     *  required. ModuleInput instances default to required so this function is
     *  only necessary in very limited circumstances.
     *
     * @return The current ModuleInput instance flagged as optional.
     *
     * @throw none No throw guarantee.
     */
    ModuleInput& make_required() noexcept;

    /** @brief Flags the current input field as opaque.
     *
     *  Input fields are opaque if their value influences memoization. This
     *  function will flag the input field as opaque. Since an input field is
     *  by default opaque this function is primarily of use when a package
     *  wants to ship a module that is based on a copy of another module.
     *
     *  @return The current instance flagged as opaque.
     *
     *  @throw None no throw guarantee.
     */
    ModuleInput& make_opaque() noexcept;

    /** @brief Flags the current input field as transparent.
     *
     *  Input fields are transparent if their value does not influence
     *  memoization. This function will flag the input field as transparent.
     *
     *  @return The current instance flagged as transparent.
     *
     *  @throw None no throw guarantee.
     */
    ModuleInput& make_transparent() noexcept;

    std::string str() const { return get_().str(); }

    /** @brief Returns the bound input as an instance of type @p T.
     *
     *  This function undoes the type-erasure on the input field. This
     *  particular overload allows the user to get the value back by reference
     *  assuming you set to bind the input by value.
     *
     * @tparam T The type to cast the type-erased input to.
     *
     * @return The value bound to this field as an instance of type @p T.
     *
     * @throw std::runtime_error if no value is bound to this input field.
     *                           Strong throw guarantee.
     * @throw std::bad_any_cast if the wrapped value can not be converted to
     *                          type @p T. Strong throw guarantee.
     */
    template<typename T>
    T value();

    /** @brief Returns the bound input as an instance of type @p T.
     *
     *  This function undoes the type-erasure on the input field. This
     *  particular overload allows you to get the value back as either a copy
     *  of the bound value or a read-only reference to the the bound value.
     *
     * @tparam T The type to cast the type-erased input to.
     *
     * @return The value bound to this field as an instance of type @p T.
     *
     * @throw std::runtime_error if no value is bound to this input field.
     *                           Strong throw guarantee.
     * @throw std::bad_any_cast if the wrapped value can not be converted to
     *                          type @p T. Strong throw guarantee.
     */
    template<typename T>
    T value() const;

    /** @brief Returns the description of the input field.
     *
     *  This function is used to retrieve the human-readable description of the
     *  input field (assuming one was set).
     *
     *  @return A read-only reference to the input field.
     *
     *  @throw std::bad_optional_access if the description was not set. Strong
     *                                  throw guarantee.
     */
    const type::description& description() const;

    /** @brief Returns the descriptions of the bounds checks this input is
     *         subject to.
     *
     *  This function is intended to be used primarily for documentation
     *  purposes. It will return human-readable descriptions of each bounds
     *  check. To see if a value passes these bounds checks use `is_valid`.
     *
     *  @return A container with the descriptions.
     *
     *  @throw std::bad_alloc if there is insufficent memory to make the return.
     *                        Strong throw guarantee.
     */
    bounds_check_desc_t check_descriptions() const;

    /** @brief Compares two ModuleInput instances for equality
     *
     *  Two ModuleInput instances are equivalent if their states are
     *  equivalent. This includes the input value they hold, as well as the
     *  meta-data such as their descriptions and whether they are transparent.
     * As a disclaimer, there is no feasible way to compare the checks so they
     * are omitted from this comparison.
     *
     *  @param[in] lhs The instance on the left side of the operator
     *  @param[in] rhs The instance on the right side of the operator
     *
     *  @return True if the requested comparison is true and false otherwise.
     *
     *  @throw none No throw guarantee.
     */
    bool operator==(const ModuleInput& rhs) const noexcept;

    /** @brief Determines if two ModuleInput instances are different
     *
     *  Two ModuleInput instances are equivalent if their states are
     *  equivalent. This includes the input value they hold, as well as the
     *  meta-data such as their descriptions and whether they are transparent.
     * As a disclaimer, there is no feasible way to compare the checks so they
     * are omitted from this comparison.
     *
     *  @param[in] lhs The instance on the left side of the operator
     *  @param[in] rhs The instance on the right side of the operator
     *
     *  @return False if the instances are equal and true otherwise.
     *
     *  @throw none No throw guarantee.
     */
    bool operator!=(const ModuleInput& rhs) const noexcept;

private:
    /// Retrieves the any from the PIMPL
    type::any& get_();

    /// Retrieves a read-only any from the PIMPL
    const type::any& get_() const;

    /// Forwards a new value to the PIMPL
    void change_(type::any new_value);

    /// Checks with the PIMPL if the value is valid
    bool is_valid_(const type::any& new_value) const;

    /// Tells the PIMPL to set the type
    void set_type_(const std::type_info& type);

    /// Adds a bounds check to the PIMPL
    ModuleInput& add_check_(any_check check, type::description desc);

    /// Generates a check that a value is of type @p T.
    template<typename T>
    auto& add_type_check_();

    /// Unwraps an SDEAny that contains a const reference
    template<typename T>
    static T unwrap_cref_(const type::any& the_value);

    /// Wraps a value into a reference_wrapper wrapped in an SDEAny
    template<typename T>
    static auto wrap_cref_(T&& new_value);

    /// Wraps a value in an SDEAny
    template<typename T>
    static type::any wrap_value_(T&& new_value);

    /// Keeps track of whether the user requested we store a const reference
    bool m_is_cref_ = false;

    /// Do we actually have a const reference (we may have had to take a copy)
    bool m_is_actually_cref_ = false;

    /// The object that stores the state of the class.
    std::unique_ptr<detail_::ModuleInputPIMPL> m_pimpl_;
};

//------------------------------Implementations---------------------------------

inline ModuleInput& ModuleInput::operator=(const ModuleInput& rhs) {
    return *this = std::move(ModuleInput(rhs));
}

template<typename T>
bool ModuleInput::is_valid(T&& new_value) const {
    constexpr bool is_value = std::is_same_v<std::decay_t<T>, T>;

    // If we got a value we can't directly forward it as it's basically a
    // temporary, this first case creates an object to avoid this.
    if constexpr(is_value) {
        std::decay_t<T> holder{std::forward<T>(new_value)};
        return is_valid_(wrap_cref_(holder));
    } else {
        return is_valid_(wrap_cref_(std::forward<T>(new_value)));
    }
}

template<typename T>
T ModuleInput::value() {
    // Did the user ask for the value back by copy?
    constexpr bool by_value = std::is_same_v<std::decay_t<T>, T>;

    // Want a const reference or a copy? Defer to ModuleInput::value() const
    if constexpr(detail_::IsConstRef<T>::value || by_value)
        return const_cast<const ModuleInput&>(*this).value<T>();

    // Know user wants it by read/write reference (okay if we copied the value)

    // Check how it's stored. If not a copy (i.e., it's a const ref) we can't
    // return read/write ref
    if(m_is_cref_) throw std::bad_any_cast();

    return detail_::SDEAnyCast<T>(get_());
}

template<typename T>
T ModuleInput::value() const {
    return m_is_actually_cref_ ? unwrap_cref_<T>(get_()) :
                                 detail_::SDEAnyCast<T>(get_());
}

template<typename T, typename U>
auto& ModuleInput::add_check(bounds_checking::CompareTo<T, U> check,
                             type::description desc) {
    if(desc.empty()) desc = print_bounds(check);

    validity_check<T> l{[check{std::move(check)}](const T& x) {
        return check(x);
    }}; // namespace sde
    return add_check(std::move(l), desc);
}

template<typename T>
auto& ModuleInput::add_check(bounds_checking::InRange<T> check,
                             type::description desc) {
    if(desc.empty()) desc = print_bounds(check);

    validity_check<T> l{
      [check{std::move(check)}](const T& x) { return check(x); }};
    return add_check(std::move(l), desc);
}

template<typename T>
auto& ModuleInput::add_check(validity_check<T> check, type::description desc) {
    any_check temp = [check{std::move(check)}](const type::any& new_value) {
        return check(new_value.is_convertible<T>() ?
                       new_value.cast<T>() :
                       ModuleInput::unwrap_cref_<T>(new_value));
    };
    return add_check_(std::move(temp), std::move(desc));
}

template<typename T>
auto& ModuleInput::set_type() {
    // Make sure the user isn't trying to set the input type to read/write
    // reference.
    constexpr bool is_ref   = std::is_reference_v<T>;
    constexpr bool is_c_ref = detail_::IsConstRef<T>::value;
    static_assert(is_c_ref || !is_ref,
                  "T should not be an lvalue or rvalue reference.");

    // Make sure it is not a pointer
    static_assert(!std::is_pointer_v<T>, "T should't be a pointer");

    // We need to clean-up the type for the SDEAny, basically we need to get it
    // down to just cv qualifified.
    using no_ref = std::remove_reference_t<T>;

    m_is_cref_ = is_c_ref;
    set_type_(typeid(no_ref));        // Sets type as seen by outside world
    return add_type_check_<no_ref>(); // Set
}

template<typename T>
auto& ModuleInput::change(T&& new_value) {
    if constexpr(detail_::IsCString<T>::value) {
        change_(wrap_value_(std::string(new_value)));
    } else { // needed to avoid this branch being compiled for string literals

        constexpr bool is_value = std::is_same_v<T, std::decay_t<T>>;
        constexpr bool is_rref  = std::is_rvalue_reference_v<T>;

        type::any da_any;

        /* Fun time. The sde supports modules taking arguments in one of two
         * ways:
         * 1. By const reference (read-only)
         * 2. By value
         *
         * When setting the actual value there's no stipulation requiring
         * the user to provide us that value in the same form. Specifically
         * they can provide us the value:
         * 1. In read/write mode (by reference)
         * 2. In read-only mode (by constant reference)
         * 3. By value (constructed it in place)
         *
         * Of these three possible inputs, number 3 is the easiest. We
         * basically just take ownership of it. For the other types of input
         * what we do depends on how it's going to be used. If the value
         * will be used in a read-only manner we just make a reference
         * wrapper around it, otherwise we copy it.
         */
        if constexpr(is_value || is_rref) { // User gave us the input by value
            da_any              = wrap_value_(std::forward<T>(new_value));
            m_is_actually_cref_ = false;
        } else {
            if(m_is_cref_) {
                da_any              = wrap_cref_(std::forward<T>(new_value));
                m_is_actually_cref_ = true;
            } else
                da_any = wrap_value_(std::forward<T>(new_value));
        }
        change_(std::move(da_any));
    }
    return *this;
}

template<typename T>
auto& ModuleInput::set_default(T&& new_value) {
    return change(std::forward<T>(new_value));
}

inline bool ModuleInput::operator!=(const ModuleInput& rhs) const noexcept {
    return !((*this) == rhs);
}

inline type::any& ModuleInput::get_() {
    const auto& temp = const_cast<const ModuleInput&>(*this).get_();
    return const_cast<type::any&>(temp);
}

template<typename T>
T ModuleInput::unwrap_cref_(const type::any& the_value) {
    return detail_::SDEAnyCast<cref_wrapper<T>>(the_value);
}

template<typename T>
auto ModuleInput::wrap_cref_(T&& new_value) {
    return detail_::make_SDEAny<cref_wrapper<T>>(
      std::cref(std::forward<T>(new_value)));
}

template<typename T>
type::any ModuleInput::wrap_value_(T&& new_value) {
    using clean_T = std::remove_reference_t<T>;
    return detail_::make_SDEAny<clean_T>(std::forward<T>(new_value));
}

template<typename T>
auto& ModuleInput::add_type_check_() {
    // When we run the checks we always wrap a const reference
    auto check = [](const type::any& new_value) {
        return new_value.is_convertible<T>() ||
               new_value.is_convertible<std::reference_wrapper<const T>>();
    };
    auto msg = std::string("Type == ") +
	       utilities::printing::Demangler::demangle(typeid(T));
    return add_check_(check, msg);
}

} // namespace sde
