#pragma once
#include "sde/detail_/sde_any.hpp"
#include "sde/types.hpp"
#include "sde/utility.hpp"
#include <cereal/access.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/unordered_map.hpp>
#include <memory>
#include <string>

namespace sde {
namespace detail_ {
class ModuleResultPIMPL;
}

/** @brief Holds the results of calling a module's run function.
 *
 *  The results computed by a module are handled similar to the inputs to a
 *  module. The big difference here is that we store the results as `shared_ptr`
 *  instances. The reason for this is that most results are being used not only
 *  at the return site, but also by the cache (here "used" means someone has a
 *  handle to them, all results are read-only to protect their integrity). The
 *  class itself will hide this detail from the user, if the user would like, by
 *  automatically dereferencing the `shared_ptr` depending on the requested
 *  type.
 *
 */
class ModuleResult {
public:
    /// The type of a `shared_ptr` to a type-erased value
    using shared_any = std::shared_ptr<const type::any>;

    /** @brief Creates and empty ModuleResult instance.
     *
     *  A ModuleResult instance created with this ctor has no type, value, or
     *  description associated with it.
     *
     *  @throw std::bad_alloc if there is insufficient memory to make the pimpl.
     *                        Strong throw guarantee.
     */
    ModuleResult();

    /** @brief Creates and empty ModuleResult instance.
     *
     *  A ModuleResult instance created with this ctor has no type, value, or
     *  description associated with it.
     *
     *  @throw std::bad_alloc if there is insufficient memory to make the pimpl.
     *                        Strong throw guarantee.
     */
    ModuleResult(const ModuleResult& rhs);
    ModuleResult& operator=(const ModuleResult& rhs);
    ModuleResult(ModuleResult&& rhs) noexcept;
    ModuleResult& operator=(ModuleResult&& rhs) noexcept;

    /** @brief Standard destructor
     *
     *  Since values are ultimately stored within `shared_ptr` instances
     *  references to the wrapped value will continue to remain valid until the
     *  reference count goes to zero. References to metadata, however, will be
     *  invalidated once the dtor is called.
     *
     *  @throw none No throw guarantee.
     */
    ~ModuleResult() noexcept;

    /** @brief Has the type of this result field been set?
     *
     *  Each result field holds an object of a given type. This function can be
     *  used to determine if the type of the present field has been set.
     *
     *  @return True if this field's type has been set and false otherwise.
     *
     *  @throw none No throw gurantee.
     */
    bool has_type() const noexcept;

    /** @brief Has a value been bound to this field yet?
     *
     *  At any given time a result field either has a value bound to it or it
     *  does not. This function can be used to determine which state the field
     *  is in.
     *
     *  @return true if the this field has a value and false otherwise.
     *
     *  @throw none No throw guarantee.
     */
    bool has_value() const noexcept;

    /** @brief Does this result have a description?
     *
     *  This function is used to determine if the developer has provided a
     *  human-readable description of this result field or not.
     *
     *  @return True if this result has a description and false otherwise.
     *
     *  @throw none No throw guarantee.
     */
    bool has_description() const noexcept;

    /** @brief Sets the type this result field must have.
     *
     *  Result fields are always types akin to std::shared_ptr<const T>. This
     *  function sets the T in that type.
     *
     *  @tparam T The unqualified type of the result.
     *
     *  @return The current instance with the type set to @p T.
     *
     *  @throw std::runtime_error if the value has already been set and this
     *                            call would change the type. Strong throw
     *                            guarantee.
     */
    template<typename T>
    auto& set_type();

    /** @brief Binds a value to this result field
     *
     *  This function is used to bind @p new_value to the present field. This
     *  call will ensure that the field is ready (i.e., the type of the field
     *  is set) and that @p new_value is of the proper type. This function
     *  allows the result to either be set by forwarding the actual value or
     *  by providing a shared_ptr to a type-erased value. Either way the type
     *  of the result will be checked and guaranteed to be compatible with that
     *  set by set_type.
     *
     *  @tparam T The type of the object to forward to the PIMPL. Should be
     *            directly convertible to the type set by set_type, or a
     *            share_ptr to a type-erased instance of that type.
     *
     * @param[in] new_value The value to bind to this field.
     *
     * @throw std::bad_optional_access if the type has not been set. Strong
     *                                 throw guarantee.
     * @throw std::invalid_argument if @p new_value does not have the correct
     *                              type. Strong throw guarantee.
     */
    template<typename T>
    void change(T&& new_value);

    /** @brief Sets this result field's description.
     *
     *  This function is used to set the human-readable description of what this
     *  result field is. If the description has been previously set this call
     *  will overwrite the previous description.
     *
     *  @param[in] desc The description of this result field.
     *
     *  @return The current instance with the description set to @p desc.
     *
     *  @throw none No throw guarantee.
     */
    ModuleResult& set_description(type::description desc) noexcept;

    /** @brief Returns the RTTI associated with the bound value
     *
     *  Each field must be of a well defined type. This function is used to
     *  retrieve the RTTI of that type.
     *
     *  @return The RTTI of the bound value.
     *
     *  @throw std::bad_optional_access if the type of this field has not been
     *                                  set. Strong throw guarantee.
     */
    type::rtti type() const;

    /** @brief Retrieves the bound value.
     *
     *  This function can be used to retrieve the value bound to this result
     *  field assuming the caller knows the type. Assuming this result is of
     *  type U. The caller may request the value back as an object of any of
     *  the following types:
     *
     *  - U
     *  - const U&
     *  - std::shared_ptr<const sde::any>
     *
     *  If no value is currently bound then an error will be raised.
     *
     *  @param T The type to retrieve the wrapped result as.
     *
     *  @return the result as the requested type.
     *
     *  @throw std::runtime_error if no value is bound to this field. Strong
     *                            throw guarantee.
     *  @throw std::bad_any_cast if the wrapped value is not convertible to type
     *                           @p T. Strong throw guarantee.
     */
    template<typename T>
    T value() const;

    /** @brief Retrieves the human-readable description of the bound value.
     *
     *  Developers are encouraged to write descriptions of what a result
     *  represents. These strings are purely for edification. This function
     *  can be used to retrieve the description, if the developer wrote one.
     *
     *  @return The human-readable description of the field.
     *
     *  @throw std::bad_optional_access if this field does not have a
     *                                  description. Strong throw guarantee.
     */
    const type::description& description() const;

    /** @brief Compares two ModuleResult instances for equality
     *
     *  Two ModuleResult instances are equivalent if they hold the same
     *  value, have the same description of that value, and require the same
     *  type.
     *
     *  @param[in] rhs The instance to compare to.
     *
     *  @return True if the two instances are equal and false otherwise.
     *
     *  @throw ??? Throws if the value's comparison throws. Same guarantee.
     */
    bool operator==(const ModuleResult& rhs) const;

    /** @brief Determines if two ModuleResul instances are different
     *
     *  Two ModuleResult instances are equivalent if they hold the same
     *  value, have the same description of that value, and require the same
     *  type.
     *
     *  @param[in] rhs The instance to compare to.
     *
     *  @return False if the two instances are equal and true otherwise.
     *
     *  @throw ??? Throws if the value's comparison throws. Same guarantee.
     */
    bool operator!=(const ModuleResult& rhs) const { return !((*this) == rhs); }

private:
    /// Implements set_type by deferring to PIMPL
    ModuleResult& set_type_(const std::type_info& type);

    /// Retrieves the type-erased value from the PIMPL
    const shared_any& at_() const;

    // Implements change when we are only given a value
    void change_(type::any new_value);

    // Implements change when we are given a shared_ptr to a value
    void change_(shared_any new_value);

    /// Helper function for wrapping a value of type @p T in an SDEAny
    template<typename T>
    static type::any wrap_value_(T&& new_value);

    /// The object that holds the actual state of the instance.
    std::unique_ptr<detail_::ModuleResultPIMPL> pimpl_;

    friend class cereal::access;
    template<class Archive>
    void serialize(Archive& ar);
}; // class ModuleResult

//------------------------------Implementations---------------------------------

inline ModuleResult& ModuleResult::operator=(const ModuleResult& rhs) {
    return *this = std::move(ModuleResult(rhs));
}

template<typename T>
auto& ModuleResult::set_type() {
    constexpr bool is_clean = std::is_same_v<std::decay_t<T>, T>;
    static_assert(is_clean, "Results must be unqualified types.");
    return set_type_(typeid(T));
}

template<typename T>
void ModuleResult::change(T&& new_value) {
    using clean_T = std::decay_t<T>;
    constexpr bool is_shared_any =
      std::is_same_v<clean_T, shared_any> || // is shared_ptr<const any>
      std::is_same_v<clean_T, std::shared_ptr<type::any>>; // no const
    if constexpr(is_shared_any)
        change_(new_value);
    else
        change_(std::move(wrap_value_(std::forward<T>(new_value))));
}

template<typename T>
T ModuleResult::value() const {
    using clean_T = std::decay_t<T>;
    if constexpr(std::is_same_v<shared_any, clean_T>)
        return at_();
    else if constexpr(detail_::IsSharedPtr<clean_T>::value) {
        using type = typename clean_T::element_type;
        return T(at_(), &value<type&>());
    } else
        return detail_::SDEAnyCast<T>(*at_());
}

template<typename T>
type::any ModuleResult::wrap_value_(T&& new_value) {
    using clean_T = std::decay_t<T>;
    return detail_::make_SDEAny<clean_T>(std::forward<T>(new_value));
}

} // namespace sde
