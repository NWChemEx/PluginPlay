#pragma once
#include "pluginplay/any/any_field.hpp"
#include <any>
#include <iostream>

namespace pluginplay::detail_ {

/** @brief Code factorization for AnyInputBase and AnyResultBase.
 *
 *  This class defines the public API of AnyInputBase and AnyResultBase which is
 *  common to both classes. This class serves primarily as code factorization.
 */
class AnyFieldBase {
private:
    /// The base class of AnyInput/AnyResult, used to get types
    using any_field_type = AnyField;

public:
    /// The type the Any class hierarchy uses for RTTI purposes
    using rtti_type = typename any_field_type::rtti_type;

    /// Standard polymorphic dtor
    virtual ~AnyFieldBase() noexcept = default;

    /** @brief Returns the RTTI for the wrapped object.
     *
     *  This function is actually implemented by calling the virtual type_
     *  function. The derived class (AnyFieldWrapper) is templated on the type
     *  of the wrapped object and implements type_ by simply returning the
     *  typeid of the template parameter.
     *
     *  Note the type that the derived class can wrap is fixed by the derived
     *  class's type, hence the type that this function can return can not
     *  change at runtime.
     *
     *  @return The RTTI for the wrapped object.
     *
     *  @throw None No throw guarantee.
     */
    rtti_type type() const noexcept { return type_(); }

    /** @brief Polymophically compares two objects derived from AnyFieldBase.
     *
     *  This function is actually implemented by symmetrically calling
     *  `are_equal_` (*i.e.* checking
     *  `this->are_equal_(rhs) && rhs.are_equal_(*this)`). `are_equal_` is a
     *  virtual function which the derived class implements to check if:
     *
     *  1. The input instance can be downcast to the derived class, and
     *  2. that the states in the derived classes are the same.
     *
     *  By calling it symmetrically we guarantee that both this instance and
     *  @p rhs have the same most derived class. Of note for our purposes this
     *  guarantees that both this instance and @p rhs are both wrappers for an
     *  input or are both wrappers for a result.
     *
     *  @param[in] rhs The instance we are comparing to.
     *
     *  @return True if the most derived type of this instance is the same as
     *               the most derived type of @p rhs, and if the state of each
     *               type in this instance's hierarchy compares equal to the
     *               state of teh corresponding type in @p rhs's hierarchy.
     *
     * @throw None No throw guarantee.
     *
     */
    bool are_equal(const AnyFieldBase& rhs) const noexcept {
        return are_equal_(rhs) && rhs.are_equal_(*this);
    }

    /** @brief Adds a text representation of the wrapped object to @p os
     *
     *  This function is actually implemented by calling the virtual function
     *  print_. The derived class then uses basic template meta-programming to
     *  determine if the wrapped type is capable of being printed to a
     *  std::ostream. If it is, the wrapped type is inserted into the stream. If
     *  it is not, the type of the object and its address are inserted into the
     *  stream. Either way the modified stream is returned to support chaining.
     *
     *  @param[in,out] os The stream we are adding a text representation to.
     *                    After this call @p os will contain a text
     *                    representation of the wrapped object.
     *
     *  @return @p os After adding the text representation of the current object
     *                to it.
     *
     *  @throw std::ios_base::failure if there is a problem writing to the
     *                                stream. Weak throw guarantee (the wrapped
     *                                object is still in the same state, but
     *                                @p os is a potentially different state).
     *  @throw ??? If calling `std::ostream::operator<<` on the wrapped type
     *             throws. Same throw guarantee.
     */
    std::ostream& print(std::ostream& os) const { return print_(os); }

protected:
    /// Trait determining if @p T is unqualified (i.e. no reference or cv)
    template<typename T>
    static constexpr bool is_by_value = std::is_same_v<std::decay_t<T>, T>;

    /// Trait determining if @p T is by const value
    template<typename T>
    static constexpr bool is_const_value =
      std::is_same_v<const std::decay_t<T>, T>;

    /// Trait determining if @p T is the type of a const reference
    template<typename T>
    static constexpr bool is_const_ref =
      std::is_same_v<const std::decay_t<T>&, T>;

    /// AnyWrappers are always created via AnyResultWrapper/AnyInputWrapper
    ///@{
    AnyFieldBase(std::any da_any) : m_value_(std::move(da_any)) {}
    AnyFieldBase(const AnyFieldBase& other) = default;
    ///@}

    /// Deleted to avoid slicing
    ///@{
    AnyFieldBase(AnyFieldBase&& other) = delete;
    AnyFieldBase& operator=(const AnyFieldBase&) = delete;
    AnyFieldBase& operator=(AnyFieldBase&&) = delete;
    ///@}

    /// Protected as they may need to be overridden by multiple derived classes
    ///@{

    /// To be overridden by derived class to implement polymorphic equality
    virtual bool are_equal_(const AnyFieldBase& rhs) const noexcept = 0;

    /// To be overridden by derived class to implement printing
    virtual std::ostream& print_(std::ostream& os) const = 0;
    ///@}

    /// The type-erased value
    std::any m_value_;

private:
    /// The derived class should implement this so it returns the RTTI of the
    /// wrapped object
    virtual rtti_type type_() const noexcept = 0;
};

} // namespace pluginplay::detail_
