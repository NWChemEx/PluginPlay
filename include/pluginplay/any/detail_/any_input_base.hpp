#pragma once
#include "pluginplay/any/any_input.hpp"
#include "pluginplay/any/detail_/any_field_base.hpp"

namespace pluginplay::detail_ {

/** @brief Extends the AnyField API to include functions which are input
 *         specific
 *
 *  In addition to the API provided by AnyFieldBase, inputs additionally:
 *  - must be hashable
 *
 *  Further complicating matters is that for T satisfying
 *  std::is_same_v<T, std::decay_t<T>> we have three scenarios:
 *  - AnyInput<T> owns an instance of type T
 *    - Users can retrieve the value by T, const T, T&, or const T&
 *  - AnyInput<const T> owns an instance of type const T
 *    - User can retrieve the value by T, const T, or const T&
 *  - AnyInput<const T&> owns an instance of type std::reference_wrapper<T>
 *    - Users can retrieve the value by T, const T, or const T&
 *
 */
class AnyInputBase : public AnyFieldBase {
private:
    /// Type of the any this implements, used to get types
    using any_input_type = AnyInput;

    /// The type of the base class
    using base_type = AnyFieldBase;

public:
    /// Type of mutable Hasher instance
    using hasher_reference = typename any_input_type::hasher_reference;

    /// How polymorphic instances will be returned
    using input_base_pointer = typename any_input_type::input_base_pointer;

    /** @brief Public API for polymorphically copying the wrapped value.
     *
     *  This method is implemented by the clone_ function. The derived class is
     *  responsible for overriding the clone_ function so that it returns a new
     *  AnyInputBase instance wrapping a deep copy of value wrapped by this
     *  instance.
     *
     *  @return A pointer to another AnyInputBase instance which contains a
     *          polymorphic deep copy.
     *
     *  @throw ??? If there is a problem copying the value. Same throw guarantee
     *
     */
    input_base_pointer clone() const { return clone_(); }

    /** @brief Retrieves the value as an instance of type T.
     *
     *  @tparam T The exact type to retrieve the value as. @p T should include
     *            the desired cv-qualifiers and whether it is to be a reference
     *            or not.
     *
     *  Based on how the ctors for the AnyFieldBase class hierarchy work, each
     *  AnyInputBase instance must always wrap a value (if an AnyInput does not
     *  wrap a value it simply holds a nullptr). This function can be used to
     *  retrieve the wrapped value as an object of type @p T, assuming the
     *  wrapped object can be converted to an object of type @p T. See the
     *  is_convertible method for details on what conversions are allowed.
     *
     *  This overload is selected when the AnyInputBase is mutable.
     *
     *  @return The wrapped instance as instance of type @p T.
     *
     *  @throw std::runtime_error if the wrapped instance can not be retrieved
     *                            as an instance of type @p T as determined by
     *                            the is_convertible method.
     *
     */
    template<typename T>
    T cast();

    /** @brief Retrieves the value as an instance of type T.
     *
     *  @tparam T The exact type to retrieve the value as. @p T should include
     *            the desired cv-qualifiers and whether it is to be a reference
     *            or not.
     *
     *  Based on how the ctors for the AnyFieldBase class hierarchy work, each
     *  AnyInputBase instance must always wrap a value (if an AnyInput does not
     *  wrap a value it simply holds a nullptr). This function can be used to
     *  retrieve the wrapped value as an object of type @p T, assuming the
     *  wrapped object can be converted to an object of type @p T. See the
     *  is_convertible method for details on what conversions are allowed.
     *
     *  This overload is selected when the AnyInputBase is read-only
     *
     *  @return The wrapped instance as instance of type @p T.
     *
     *  @throw std::runtime_error if the wrapped instance can not be retrieved
     *                            as an instance of type @p T as determined by
     *                            the is_convertible method.
     *
     */
    template<typename T>
    T cast() const;

    /** @brief Determines if the wrapped value can be retrieved as type @p T
     *
     *  @param T The fully qualified (cv-qualifications and/or reference) type
     *           you would like to get the wrapped value back as.
     *
     *  This overload of `is_convertible` is invoked when the AnyInputBase is
     *  mutable. Whether we can convert the wrapped object to an object of type
     *  @p T depends on which of the following scenarios is true:
     *
     *  1. We wrap the object by mutable value
     *  2. We hold the object in a read-only state (either by value or ref)
     *
     *  For scenerio 1 we can return the object by value, const value,
     *  reference, or const reference. Scenario 2 is essentially the same as
     *  when AnyInputBase is itself read-only and we defer to the const overload
     *  of is_convertible for scenario 2.
     *
     *  @return True if the wrapped instance can be returned as type @p T and
     *          false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    template<typename T>
    bool is_convertible() noexcept;

    /** @brief Determines if the wrapped value can be retrieved as type @p T
     *
     *  @param T The fully qualified (cv-qualifications and/or reference) type
     *           you would like to get the wrapped value back as.
     *
     *  This overload of `is_convertible` is invoked when the AnyInputBase is
     *  read-only. Since the AnyInputBase is read-only it will only allow you
     *  to retrieve the wrapped value in a manner that does not allow you to
     *  modify it. Namely this function will return true if you request to get
     *  the value back by value, const value, or read-only reference (and if
     *  the wrapped value is actually of type `std::decay_t<T>`).
     *
     *  @return True if the wrapped instance can be returned as type @p T and
     *          false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    template<typename T>
    bool is_convertible() const noexcept;

    /** @brief Public API for hashing the wrapped value.
     *
     *  This method is implemented by the hash_ function. The derived class is
     *  responsible for overriding the hash_ function so that it hashes the
     *  wrapped value. Regardless of how the value is held it will be hashed as
     *  if it is held as a read-only reference. This is relevant for hashers
     *  which include the type of the object in the hashing process.
     *
     *  N.B. This function is named do_hash so that it is not automatically
     *  grabbed by the Hasher object. We don't want to grab the type of the
     *  wrapper in the PIMPL in the hash, we just want to hash the value.
     *
     *  @param[in,out] h The Hasher object to use for hashing. After calling
     *                   this function the internal buffer of @p h will be
     *                   updated with a hash of the wrapped value.
     */
    void do_hash(hasher_reference h) const { hash_(h); }

    /** @brief Used to determine if we are holding a reference to the input
     *         value.
     *
     *  AnyInputs can be either own their value or hold a read-only reference to
     *  it. This function is used to determine if this particular AnyInput is
     *  wrapping a read-only reference. The derived class is responsible for
     *  implementing storing_const_ref_ to implement this function.
     *
     *  N.B. AnyInputs can't hold mutable references.
     *
     *  @return True if the derived class wraps a read-only reference and false
     *          otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool storing_const_ref() const noexcept { return storing_const_ref_(); }

    /** @brief Used to determine if we are holding a read-only value.
     *
     *  It's probably not a a common use case, but we do support taking
     *  ownership of a value and only allowing users to access it in a read-only
     *  manner. This function is used to determine if this is the present
     *  scenario for AnyInput. The storing_const_value function is actually
     *  implemented by the virtual function storing_const_value_. The derived
     *  class is responsible for implementing storing_const_value_ to return the
     *  correct answer.
     *
     *  @return True if the derived class wraps a read-only value and false
     *          otherwise.
     *
     *  @throw None No throw gurantee.
     */
    bool storing_const_value() const noexcept { return storing_const_value_(); }

protected:
    /// If we have to wrap a reference we wrap it as this type
    template<typename T>
    using reference_type = std::reference_wrapper<const T>;

    /// Same ctors as the base
    using AnyFieldBase::AnyFieldBase;

private:
    /// Codef factorization for throwing when is_convertible<T> fails
    template<typename T>
    void not_convertible_() const;

    /// Used by derived class to implement polymorphic copy
    virtual input_base_pointer clone_() const = 0;

    /// Used by derived class to signal if we are holding by read-only reference
    virtual bool storing_const_ref_() const noexcept = 0;

    /// Used by the derived class to signal if we are holding by read-only value
    virtual bool storing_const_value_() const noexcept = 0;

    /// Used by the derived class to implement hash
    virtual void hash_(hasher_reference h) const = 0;
};

// -- Inline Implementations ---------------------------------------------------

template<typename T>
T AnyInputBase::cast() {
    if(!is_convertible<T>()) not_convertible_<T>();

    using clean_type              = std::decay_t<T>;
    using mutable_ref             = clean_type&;
    constexpr bool by_mutable_ref = std::is_same_v<T, mutable_ref>;

    if constexpr(!by_mutable_ref) {
        if(storing_const_ref()) {
            // This is the only possible way we use a reference wrapper
            using ref_type = std::reference_wrapper<const clean_type>;
            return std::any_cast<ref_type>(m_value_).get();
        }
    } // else: assert_convertible will catch by_mutable_ref and
      // storing_const_ref

    return std::any_cast<T>(m_value_);
}

template<typename T>
T AnyInputBase::cast() const {
    if(!is_convertible<T>()) not_convertible_<T>();

    using clean_type              = std::decay_t<T>;
    using mutable_ref             = clean_type&;
    constexpr bool by_mutable_ref = std::is_same_v<T, mutable_ref>;
    static_assert(!by_mutable_ref, "Read-only can't be retrieved mutably");

    if(storing_const_ref()) {
        using ref_type = std::reference_wrapper<const clean_type>;
        return std::any_cast<ref_type>(m_value_).get();
    }
    return std::any_cast<T>(m_value_);
}

template<typename T>
bool AnyInputBase::is_convertible() noexcept {
    // Only differs from non-const overload if we hold a mutable value
    if(storing_const_ref() || storing_const_value())
        return std::as_const(*this).is_convertible<T>();

    // Getting here means it's stored by mutable value and we can return it
    // however the user wants (as long as the object's actually that type...)
    return std::any_cast<std::decay_t<T>>(&m_value_) != nullptr;
}

template<typename T>
bool AnyInputBase::is_convertible() const noexcept {
    // Value is read-only so can only be one of these:
    constexpr bool by_val  = base_type::is_by_value<T>;
    constexpr bool by_cval = base_type::is_const_value<T>;
    constexpr bool by_cref = base_type::is_const_ref<T>;

    if constexpr(by_val || by_cval || by_cref) {
        // If stored by cref we need to undo the fact it's in a ref wrapper
        // before comparing
        if(storing_const_ref()) {
            using ref_type = reference_type<std::decay_t<T>>;
            return std::any_cast<ref_type>(&m_value_) != nullptr;
        }
        // Otherwise just compare them
        return std::any_cast<std::decay_t<T>>(&m_value_) != nullptr;
    } else {
        return false;
    }
}

template<typename T>
void AnyInputBase::not_convertible_() const {
    std::string msg = "Can not cast AnyInput to type: ";
    msg += type().name();
    throw std::runtime_error(msg);
}

} // namespace pluginplay::detail_
