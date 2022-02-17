#pragma once
#include "pluginplay/any/detail_/any_field_base.hpp"
#include <any>
#include <utilities/printing/print_stl.hpp>

namespace pluginplay::detail_ {

/** @brief Implements functions common to AnyInputWrapper and AnyResultWrapper
 *
 *  This class implements functions which are common to the AnyInputWrapper and
 *  AnyResultWrapper classes. It is templated on the class it should derive from
 *  (`AnyResultBase` or `AnyInputBase` for `AnyResultWrapper` and
 *  `AnyInputWrapper` respectively) to avoid multiple inheritance in the derived
 *  classes. More specifically if `AnyFieldWrapper` inherited directly from
 *  `AnyFieldBase` then say `AnyResultWrapper` would have to inherit from both
 *  `AnyFieldWrapper` and `AnyResultBase`.
 *
 *  @tparam T The type of the object being wrapped. The exact concepts that the
 *            type must satisfy are enforced by the derived class. This class
 *            only assumes that instances of type T can be compared with
 *            `operator==`.
 *  @tparam BaseType Expected to be either `AnyResultBase` or `AnyInputBase`.
 */
template<typename T, typename BaseType>
class AnyFieldWrapper : public BaseType {
private:
    /// Type of this class, including the template parameters
    using my_type = AnyFieldWrapper<T, BaseType>;

public:
    /// Type used for expressing the runtime type information
    using rtti_type = typename BaseType::rtti_type;

protected:
    /** @brief Value-wrapping ctor.
     *
     *  The derived classes take a value, wrap it in an `std::any` and then
     *  forward it to this ctor. This ctor is responsible for forwarding the
     *  `std::any` to the base class on behalf of the derived class.
     *
     *  @param[in] da_any The type-erased value being wrapped by this field.
     *
     *  @throw None No throw guarantee.
     */
    AnyFieldWrapper(std::any da_any) : BaseType(std::move(da_any)) {}

    /** @brief Implements AnyFieldBase::are_equal
     *
     *  This function contains the majority of the implementation for
     *  AnyFieldBase::are_equal for both `AnyInputWrapper` and
     *  `AnyResultWrapper`. In particular it first ensures that @p rhs can be
     *  cast to an `AnyFieldWrapper<T, BaseType>` instance (*i.e.*, the type of
     *  the current instance). In doing so it has checked that the anys wrap
     *  objects of the same type and the objects are being wrapped in the same
     *  capacity (as inputs or as results). After confirming type compatability,
     *  the function retrieves const references to the values, and compares the
     *  values. The only thing left for the derived classes to do is to check
     *  any state added in the derived class.
     *
     *  @param[in] rhs The instance we are comparing to.
     *
     *  @return True if this instance compares equal and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool are_equal_(const AnyFieldBase& rhs) const noexcept override;

    /** @brief Implements AnyFieldBase::print
     *
     *  This function implements AnyFieldBase::print by determining if
     *  `std::ostream::operator<<` is defined for type @p T. If it has it simply
     *  calls the stream insertion operator on the wrapped value. If it has not
     *  been defined then this function will print the name of the type and the
     *  address of the wrapped object.
     *
     *  @param[in,out] os The stream we are printing to. After this call @p os
     *                    will contain the string representation of the wrapped
     *                    object.
     *  @return @p os after adding the wrapped value to it.
     */
    std::ostream& print_(std::ostream& os) const override;

private:
    /// Implements type() for both AnyResultWrapper and AnyInputWrapper
    rtti_type type_() const noexcept override { return {typeid(T)}; }
};

// -- inline implementations ---------------------------------------------------

template<typename T, typename BaseType>
bool AnyFieldWrapper<T, BaseType>::are_equal_(
  const AnyFieldBase& rhs) const noexcept {
    auto prhs = dynamic_cast<const my_type*>(&rhs);
    if(prhs == nullptr) return false; // Not the same type
    // Compare the values
    const auto& lhs_value = this->BaseType::template cast<const T&>();
    const auto& rhs_value = prhs->BaseType::template cast<const T&>();
    return lhs_value == rhs_value;
}

template<typename T, typename BaseType>
std::ostream& AnyFieldWrapper<T, BaseType>::print_(std::ostream& os) const {
    using utilities::printing::operator<<;
    if constexpr(utilities::type_traits::is_printable_v<T>) {
        os << this->BaseType::template cast<const T&>();
    } else {
        const auto* pvalue = &(this->BaseType::template cast<const T&>());
        os << "<" << typeid(T).name() << " " << pvalue << ">";
    }
    return os;
}

} // namespace pluginplay::detail_
