#pragma once
#include "pluginplay/any/detail_/any_field_base.hpp"
#include <any>

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
 *  @tparam BaseType Expected to be either `AnyResultBase` or `AnyInputBase`.
 */
template<typename T, typename BaseType>
class AnyFieldWrapper : public BaseType {
private:
    using my_type = AnyFieldWrapper<T, BaseType>;

public:
    using rtti_type = typename BaseType::rtti_type;

protected:
    AnyFieldWrapper(std::any da_any) : BaseType(std::move(da_any)) {}

    bool are_equal_(const AnyFieldBase& rhs) const noexcept override;

private:
    /// Implements type() for both AnyResultWrapper and AnyInputWrapper
    rtti_type type_() const noexcept override { return {typeid(T)}; }
};

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

} // namespace pluginplay::detail_
