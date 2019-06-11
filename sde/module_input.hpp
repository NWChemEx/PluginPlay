#pragma once
#include "sde/detail_/sde_any.hpp"
#include "sde/types.hpp"
#include "sde/utility.hpp"
#include <functional>
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
    /// Type of a functor that can be used to check a value
    template<typename T>
    using validity_check = std::function<bool(const T&)>;

    /// Type of a check that operates on a type-erased value
    using any_check = validity_check<type::any>;

    ///@{
    /** @name Ctors and assignment operators
     *
     *  These functions are designed to create a ModuleInput instance with
     *  either a default state, or a state copied/moved from another instance.
     *  Customization is accomplished using a factory pattern in which the state
     *  is specified by chaining the "setters". Note that all copies are deep
     *  copies.
     *
     *  @param rhs The instance to copy/move from. For move operations @p rhs is
     *         in a valid, but otherwise undefined state.
     *
     *  @throw std::bad_alloc 1, 2, and 3 throw if there is insufficient memory
     *         to create a new PIMPL. 2 and 3 additionally will throw if there
     *         is insufficient memory to copy the values in the PIMPL. Strong
     *         throw guarantee.
     *  @throw none All move operations are no throw guarantee.
     */
    ModuleInput();

    ModuleInput(const ModuleInput& rhs);

    ModuleInput& operator=(const ModuleInput& rhs) {
        return *this = std::move(ModuleInput(rhs));
    }

    ModuleInput(ModuleInput&& rhs) noexcept;

    ModuleInput& operator=(ModuleInput&& rhs) noexcept;
    ///@}

    /** @brief Standard destructor.
     *
     *  This deletes the state of the instance meaning that all references to
     *  the internal state, notably the value and the description, are
     *  invalidated.
     *
     *  @throw none No throw guarantee.
     */
    ~ModuleInput() noexcept;

    //@{
    /** @name Getters
     *
     *  These are functions for accessing the state of the instance.
     *  Respectively these getters:
     *
     *  - Retrieve the input's value in a possibly read/write form
     *  - Retrieve the input's value in a read-only form
     *  - Indicate whether the value must be set.
     *  - Indicate whether the value affects hashing
     *  - Indicate whether the input is ready (present, if non-optional)
     *  - Retrieve a human-readable description of what the input is used for.
     *
     *  @param T For `value`, the type you want the wrapped value back as.
     *
     *  @throw std::bad_cast if the value can not be converted to the type @p T
     *         provided to `value`. Strong throw guarantee.
     *  @throw std::runtime_error if the value has not been set (and is not
     *         optional). Strong throw guarantee.
     *
     *  @throw none With the exception of the `value` member function all
     *         getters are no throw guarantee.
     */
    template<typename T>
    T value() {
        if(!ready()) throw std::runtime_error("Value is not set.");
        constexpr bool by_value = std::is_same_v<std::decay_t<T>, T>;
        if constexpr(detail_::IsConstRef<T>::value || by_value)
            return const_cast<const ModuleInput&>(*this).value<T>();
        /* Want it by reference. Normally the any cast will catch this and
         * throw an error; however, we have one edge case to worry about:
         * we're supposedly storing a const reference, but the user gave us the
         * object by value (hence we're actually storing the object by value).
         * This
         * is an implementation detail, so we fake the cast.
         */
        if(is_cref_) throw std::bad_cast();
        return detail_::SDEAnyCast<T>(get_());
    }
    template<typename T>
    T value() const {
        if(!ready()) throw std::runtime_error("Value is not set.");
        return is_actually_cref_ ? unwrap_cref<T>(get_()) :
                                   detail_::SDEAnyCast<T>(get_());
    }

    bool is_optional() const noexcept;

    bool is_transparent() const noexcept;

    bool ready() const noexcept;

    const type::description& description() const noexcept;
    //@}

    //@{
    /** @name Setters
     *
     *  The functions in this section can be used to modify the value and the
     *  metadata associated with the value. Note that "optional" means that the
     *  module will operate without the value being set and "transparent" means
     *  that the value does not affect the outcome of the module. Values are
     *  required and opaque by default.
     *
     *  Respectively these functions:
     *
     * - Sets the type the value will be used as.
     * - Adds additional bounds checks
     * - Changes the value
     * - Sets the default value
     * - Sets the human-readable description
     * - Labels the input as optional (undoes setting it as required)
     * - Labels the input as required (undoes setting it as optional)
     * - Labels the input as transparent (undoes setting it as opaque)
     * - Labels the input as opaque (undoes setting it as transparent)
     *
     * @param T The type of the wrapped value.
     *
     * @return All setters return the current object, with the state updated
     *         according to the called function.
     */
    template<typename T>
    auto& set_type() {
        constexpr bool is_ref   = std::is_reference_v<T>;
        constexpr bool is_c_ref = detail_::IsConstRef<T>::value;
        static_assert(is_c_ref || !is_ref,
                      "Inputs should be read-only references or by value");
        is_cref_ = is_c_ref;
        set_type_(typeid(T));
        return add_type_check<T>();
    }

    template<typename T>
    auto& add_check(validity_check<T> check, type::description desc = "") {
        any_check temp = [check{std::move(check)}](const type::any& new_value) {
            return check(ModuleInput::unwrap_cref<T>(new_value));
        };
        return add_check_(std::move(temp), std::move(desc));
    }

    template<typename T>
    auto& change(T&& new_value) {
        if constexpr(detail_::IsCString<T>::value) {
            change_(wrap_value(std::string(new_value)));
        } else { // needed to avoid this branch being compiled for string
                 // literals
            constexpr bool is_value = std::is_same_v<T, std::decay_t<T>>;
            if(!is_valid(new_value))
                throw std::invalid_argument(
                  "Value has failed one or more checks");
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
            if constexpr(is_value) { // User gave us the input by value
                da_any            = wrap_value(std::forward<T>(new_value));
                is_actually_cref_ = false;
            } else {
                if(is_cref_) {
                    da_any            = wrap_cref(std::forward<T>(new_value));
                    is_actually_cref_ = true;
                } else
                    da_any = wrap_value(std::forward<T>(new_value));
            }
            change_(da_any);
        }
        return *this;
    }

    template<typename T>
    auto& set_default(T&& new_value) {
        return change(std::forward<T>(new_value));
    }

    ModuleInput& set_description(type::description desc) noexcept;

    ModuleInput& make_optional() noexcept;

    ModuleInput& make_required() noexcept;

    ModuleInput& make_transparent() noexcept;

    ModuleInput& make_opaque() noexcept;
    //@}

    /** @brief Determines if the provided value is a valid value.
     *
     * This function will run all of the bounds checks on a value and return
     * whether or not the provided value is a valid value.
     *
     * @tparam T The type of the value we are checking
     * @param new_value  The value we are checking
     * @return True if the provided value is a valid value and false otherwise.
     *
     * @throws ??? throws if any of the stored bounds checks through. Strong
     *         throw guarantee.
     */
    template<typename T>
    bool is_valid(T&& new_value) const {
        return is_valid_(wrap_cref(std::forward<T>(new_value)));
    }

    /** @brief Hashes the current instance and adds the hash
     *
     * The current strategy for hashing in the SDE is to:
     *
     * - make a hasher object,
     * - pass it to everything you want to hash, and
     * - finalize the hasher
     *
     * Upon finalization the hasher returns the hash of the collective state of
     * objects passed to it. This function allows ModuleInput instances to be
     * hashed by the hasher object.
     *
     * @param h The hasher object that is in the process of hashing stuff.
     */
    void hash(type::hasher& h) const;

    ///@{
    /** @name Equality comparisons
     *
     * @param rhs
     * @return
     */
    bool operator==(const ModuleInput& rhs) const;
    bool operator!=(const ModuleInput& rhs) const { return !((*this) == rhs); }
    ///@}

private:
    ///@{
    /** @name Type-erased APIs
     *
     *  The functions in this section bridge the gap between the templated
     *  member functions that are part of the public API and the PIMPL. They
     *  Typically work by accepting/returning SDEAny instances and leaving it
     *  up to the template member functions to wrap/unwrap the SDEAny.
     *
     *  Respectively they:
     *
     *  - Get the value in a read/write state
     *  - Get the value in a read-only state
     *  - Change the value
     *  - Determine if a value is valid
     *  - Sets the type the value must have.
     *  - Adds another bounds check
     *
     *  @throw std::runtime_error
     */
    type::any& get_() {
        const auto& temp = const_cast<const ModuleInput&>(*this).get_();
        return const_cast<type::any&>(temp);
    }

    const type::any& get_() const;

    void change_(type::any new_value);

    bool is_valid_(const type::any& new_value) const;

    void set_type_(const std::type_info& type);

    ModuleInput& add_check_(any_check check, type::description desc);
    ///@}

    /// Generates a check that a value is of type @p T.
    template<typename T>
    auto& add_type_check() {
        any_check check = [](const type::any& new_value) {
            return new_value.type() == typeid(cref_wrapper<T>);
        };
        return add_check_(std::move(check), "Type Check");
    }

    ///@{
    /** @name Helper functions for making/unwrapping SDEAny instances
     *
     * @tparam T The type of the value we are wrapping/unwrapping to
     * @param the_value The wrapped value that we are unwrapping.
     * @param new_value The unwrapped value that we are wrapping
     * @return The unwrapped/wrapped value.
     * @throw std::bad_cast Thrown by 1 if the wrapped value is not of type
     *        @p T. Strong throw guarantee.
     * @throw std::bad_alloc Thrown by 2 and 3 if there is insufficient memory
     *        to create the SDEAny. Strong throw guarantee.
     */
    template<typename T>
    static T unwrap_cref(const type::any& the_value) {
        return detail_::SDEAnyCast<cref_wrapper<T>>(the_value);
    }

    template<typename T>
    static auto wrap_cref(T&& new_value) {
        return detail_::make_SDEAny<cref_wrapper<T>>(
          std::cref(std::forward<T>(new_value)));
    }

    template<typename T>
    static type::any wrap_value(T&& new_value) {
        using clean_T = std::decay_t<T>;
        return detail_::make_SDEAny<clean_T>(std::forward<T>(new_value));
    }
    ///@}

    /// Keeps track of whether the user requested we store a const reference
    bool is_cref_ = false;

    /// Do we actually have a const reference (we may have had to take a copy)
    bool is_actually_cref_ = false;

    /// The object that stores the state of the class.
    std::unique_ptr<detail_::ModuleInputPIMPL> pimpl_;
};

std::ostream& print_as_rst(std::ostream& os, const ModuleInput& input,
                           std::string indent = "");

} // namespace sde
