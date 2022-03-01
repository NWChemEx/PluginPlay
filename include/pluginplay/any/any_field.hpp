#pragma once
#include <exception>
#include <memory>
#include <ostream>
#include <typeindex>

namespace pluginplay {
namespace detail_ {
class AnyFieldBase; // Forward declare the PIMPL
}

/** @brief Base class of Any hierarchy.
 *
 *  This class holds methods common to both the AnyInput and AnyResult class.
 *  For the most part these methods are utility methods (e.g., comparisons,
 *  printing) or related to general inquiries (e.g., is it currently wrapping a
 *  value, what is the RTTI of the wrapped value).
 *
 *  Generally speaking this class serves as code factorization, meaning users
 *  for the most part will not be dealing with AnyField instances directly.
 *  Instead most code will either take AnyInput or AnyResult instances.
 */
class AnyField {
public:
    /// Type used for runtime type information (RTTI) purposes
    using rtti_type = std::type_index;

    /// Default polymorphic dtor
    virtual ~AnyField() noexcept = default;

    /** @brief Restores the AnyField to a default initialized state.
     *
     *  This method can be used to release the held pointer. For AnyResult
     *  instances, and AnyInput's which own their wrapped value, this will
     *  delete the wrapped value. For AnyInput instances which only alias there
     *  value this will only stop the current instance from aliasing the
     *  original value, i.e. the original value will continue to exist. If the
     *  current AnyField instance does not hold a value this is no-op.
     *
     *  To avoid storing the PIMPL in this class (and thus the derived class
     *  having to downcast to use it), this function is implemented by calling a
     *  virtual function reset_. Derived classes are responsible for
     *  implementing `reset_` so that it releases the PIMPL.
     *
     *  @throw None No throw guarantee.
     */
    void reset() noexcept { reset_(); }

    /** @brief Retrieves the RTTI of the wrapped type.
     *
     *  At any given time an AnyField either wraps a value or does not. If it
     *  wraps a value this function will return the RTTI of the wrapped value.
     *  If it does not wrap a type, it will return the RTTI of a nullptr.
     *
     *  @return The RTTI of the wrapped value, if has_value is true. Otherwise
     *          the RTTI of a nullptr will be returned.
     *
     *  @throw None No throw guarantee.
     */
    rtti_type type() const noexcept;

    /** @brief Polymorphically compares two AnyField objects for value equality.
     *
     *  Two AnyField instances are value equal if they both:
     *  - have the same most derived class
     *  - agree on the result of `has_value`
     *  - wrap values of the same type (assuming they wrap a value)
     *  - wrap values which compare value equal (assuming they wrap a value)
     *
     *  @return True if this AnyField instance is polymorphically value equal to
     *          @p rhs and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool are_equal(const AnyField& rhs) const noexcept;

    /** @brief Adds a string representation of the wrapped object to the stream
     *
     *  Sometimes it's useful to have string representations of objects. If the
     *  wrapped object overloads std::ostream::operator<< this function will
     *  use std::ostream::operator<< to add a string representation of the
     *  wrapped object to @p os. If no such overload is present, the type and
     *  address of the object will be printed as a proxy representation.
     *  Finally, if this AnyField is not presently holding a value @p os will be
     *  returned unchanged.
     *
     *  @param[in,out] os The stream we are printing to. After this operation
     *                    @p os will contain a string representation of the
     *                    wrapped value.
     *
     *  @return @p os after adding the string representation to it.
     *
     *  @throw ??? If the wrapped object's operator<< throws. Same throw
     *             guarantee.
     */
    std::ostream& print(std::ostream& os) const;

    /** @brief Does this AnyField currently wrap a value?
     *
     *  At any time an AnyField either wraps a value or does not. This function
     *  is used to determine if the AnyField wraps a value.
     *
     *  Like `reset`, this function relies on a virtual function `has_value_` to
     *  determine if the derived class actually wraps a value. Derived classes
     *  are responsible for implementing `has_value_` so that it returns true
     *  when the derived class holds a value and false otherwise.
     *
     *  @return True if the derived class holds a value (even if that value is
     *          a defaulted value) and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool has_value() const noexcept { return has_value_(); }

protected:
    /// Type of the PIMPL providing the API to an AnyFieldWrapper
    using field_base = detail_::AnyFieldBase;

    /// Mutable pointer to the base class common to the wrappers
    using field_base_reference = field_base&;

    /// Read-only reference to the base class common to the wrappers
    using const_field_base_reference = const field_base&;

    /// Users shouldn't be creating AnyField instances directly
    ///@{
    AnyField() noexcept           = default;
    AnyField(const AnyField&)     = default;
    AnyField(AnyField&&) noexcept = default;
    AnyField& operator=(const AnyField&) = default;
    AnyField& operator=(AnyField&&) noexcept = default;
    ///@}

    /// Throws if there's no wrapped value
    void assert_value_() const;

    /// Derived classes override this to compare derived state (including type)
    virtual bool are_equal_(const AnyField& rhs) const noexcept;

    /// Derived classes implement this to reset the PIMPL
    virtual void reset_() noexcept = 0;

    /// Derived classes implement this to indicate if there is a PIMPL
    virtual bool has_value_() const noexcept = 0;

    /// Derived classes implement it to return the mutable wrapped value
    virtual field_base_reference base_pimpl_() = 0;

    /// Derived classes implement it to return the read-only wrapped value
    virtual const_field_base_reference base_pimpl_() const = 0;
};

/** @brief Allows stream insertion of AnyField instances.
 *
 *  @relates AnyField
 *
 *  This function overloads std::ostream::operator<< so that AnyField instances
 *  can be inserted into std::ostream instances. The actual implementation
 *  simply defers to AnyField::print. More details on the insertion behavior
 *  can be found in the documentation for AnyField::print.
 *
 *  @param[in,out] os The stream we are inserting @p any in to. After this call
 *                    @p os will contain a string representation of @p any.
 *  @param[in] any The AnyField instance we are inserting into @p os.
 *
 *  @return @p os after inserting @p any in to it.
 *
 *  @throw ??? If AnyField::print throws. Same throw guarantee.
 */
inline std::ostream& operator<<(std::ostream& os, const AnyField& any) {
    return any.print(os);
}

// -- Inline Implementations ---------------------------------------------------

inline void AnyField::assert_value_() const {
    if(has_value()) return;
    throw std::runtime_error("AnyField does not hold a value");
}

} // namespace pluginplay
