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
 *    - Users can retrieve the value by T, T&, or const T&
 *  - AnyInput<const T> owns an instance of type const T
 *    - User can retrieve the value by T or const T&
 *  - AnyInput<const T&> owns an instance of type std::reference_wrapper<T>
 *    - Users can retrieve the value by T or const T&
 *
 */
class AnyInputBase : public AnyFieldBase {
private:
    /// Type of the any this implements, used to get types
    using any_input_type = AnyInput;

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

    template<typename T>
    T cast() const;

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

    bool storing_const_value() const noexcept { return storing_const_value_(); }

protected:
    /// Same ctors as the base
    using AnyFieldBase::AnyFieldBase;

    template<typename T>
    void assert_convertible_() const;

private:
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
T AnyInputBase::cast() const {
    assert_convertible_<T>();
    using clean_type = std::decay_t<T>;
    if(storing_const_ref()) {
        using ref_type = std::reference_wrapper<const clean_type>;
        return std::any_cast<ref_type>(m_value_).get();
    }
    return std::any_cast<T>(m_value_);
}

template<typename T>
bool AnyInputBase::is_convertible() const noexcept {
    using clean_type      = std::decay_t<T>;
    constexpr bool by_ref = std::is_same_v<clean_type&, T>;

    // Short circuit if trying to get by value, but it's an unsupported type
    if(by_ref) {
        if(storing_const_ref() || storing_const_value())
            return false; // Can't get back by reference
    }
    if(storing_const_ref()) {
        using ref_type = std::reference_wrapper<const clean_type>;
        return std::any_cast<ref_type>(&m_value_) != nullptr;
    }

    // Storing by value or const value (ruled out by_ref for latter)
    return std::any_cast<clean_type>(&m_value_) != nullptr;
}

template<typename T>
void AnyInputBase::assert_convertible_() const {
    if(is_convertible<T>()) return;
    std::string msg = "Can not cast AnyInput to type: ";
    msg += type().name();
    throw std::runtime_error(msg);
}

} // namespace pluginplay::detail_
