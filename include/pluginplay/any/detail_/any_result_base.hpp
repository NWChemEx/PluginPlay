#pragma once
#include "pluginplay/any/any_result.hpp"
#include "pluginplay/any/detail_/any_field_base.hpp"

namespace pluginplay::detail_ {

/** @brief Extends the AnyFieldBase API to result-specific functions
 *
 *  In PluginPlay results are owned by AnyResult instances. The results can
 *  only be accessed by value and/or read-only reference. This class implements
 *  the retrieval of the wrapped object while enforcing these restrictions.
 *  Additionally, the AnyResultBase class defines functions which are needed to
 *  implement the AnyResult class, but not the AnyInput class.
 *
 *  N.B. Since PluginPlay always creates either an AnyInput or an AnyResult we
 *  have opted to put the clone method in the AnyResultBase/AnyInputBase classes
 *  to avoid AnyInput and AnyResult having to downcast from an AnyFieldBase
 *  object.
 */
class AnyResultBase : public AnyFieldBase {
private:
    /// Type of the base class
    using base_type = AnyFieldBase;

    /// Type we're going to get our member types from
    using any_result_type = AnyResult;

public:
    /// How polymorphic instances will be returned
    using result_base_pointer = typename any_result_type::result_base_pointer;

    /** @brief Creates a polymorphic deep copy of the current instance.
     *
     *  This function is implemented by the virtual function clone_. The most
     *  derived class is responsible for making the `clone_` function return a
     *  deep copy of the current instance, via a pointer to the base class.
     *
     *  @return A deep polymorphic copy of the current instance returned via a
     *          pointer to the base class.
     */
    result_base_pointer clone() const { return clone_(); }

    /** @brief Retrieves the wrapped object as an instance of type @p T
     *
     *  Results are owned by the AnyResult and can only be interacted with in a
     *  read-only manner. This function allows you to get the wrapped object
     *  back as an object of type @p T as long as @p T is consistent with the
     *  read-only restrictions and @p T only differs from the actual type of the
     *  stored object up to cv-qualifiers and a reference.
     *
     *  N.B. As the ctors/assignment operators are setup, it is not possible for
     *  an AnyResultBase instance to not hold a value. If an AnyResult doesn't
     *  hold a value it's because it has a null AnyResultBase pointer.
     *
     *  @tparam T either an unqualified type or a read-only reference. If @p T
     *            is not an allowed type a static assert will be raised.
     *
     *  @return The wrapped instance as an object of type @p T.
     *
     *  @throw std::runtime_error if is_convertible<T> is false. Strong throw
     *                            guarantee.
     */
    template<typename T>
    T cast() const;

    /** @brief Determines if the wrapped type can be converted to type @p T.
     *
     *  The wrapped object is convertible to a type @p T if:
     *  - @p T is consistent with the read-only nature of the held data, and
     *  - the type of the wrapped object differs from type @p T by at most a
     *    reference and/or cv qualifiers.
     *
     *  @return True if the wrapped object can be retrieved as an object of type
     *          @T and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    template<typename T>
    bool is_convertible() const noexcept;

protected:
    /// Same ctors as the base
    using AnyFieldBase::AnyFieldBase;

private:
    /// Is type @p T an allowed return type
    template<typename T>
    static constexpr bool is_allowed_return_type =
      base_type::is_by_value<T> || base_type::is_const_value<T> ||
      base_type::is_const_ref<T>;

    /// static_assert if @p T is unallowed, throws if the wrapped object can't
    /// be converted to @p T
    template<typename T>
    void assert_convertible_() const;

    /// To be implemented by the derived class to implement clone()
    virtual result_base_pointer clone_() const = 0;
};

// -- Inline implementations ---------------------------------------------------

template<typename T>
T AnyResultBase::cast() const {
    assert_convertible_<T>();
    return std::any_cast<T>(m_value_);
}

template<typename T>
bool AnyResultBase::is_convertible() const noexcept {
    // For cast to work must be by value or by const ref
    if(!is_allowed_return_type<T>) return false;
    return std::any_cast<std::decay_t<T>>(&m_value_) != nullptr;
}

template<typename T>
void AnyResultBase::assert_convertible_() const {
    static_assert(
      is_allowed_return_type<T>,
      "Results must be retrieved by value or by read-only reference.");
    if(is_convertible<T>()) return;
    std::string msg = "Can not cast AnyResult to type: ";
    msg += type().name();
    throw std::runtime_error(msg);
}

} // namespace pluginplay::detail_
