#pragma once
#include "pluginplay/any/detail_/any_field_wrapper.hpp"
#include "pluginplay/any/detail_/any_result_base.hpp"

namespace pluginplay::detail_ {

template<typename T>
class AnyResultWrapper : public AnyFieldWrapper<T, AnyResultBase> {
private:
    using base_type = AnyFieldWrapper<T, AnyResultBase>;
    using my_type   = AnyResultWrapper<T>;

public:
    using result_base_pointer = AnyResultBase::result_base_pointer;

    template<typename U>
    AnyResultWrapper(U&& u);

protected:
    /// Adds type check to AnyFieldWrapper's implementation
    bool are_equal_(const AnyFieldBase& rhs) const noexcept override;

private:
    result_base_pointer clone_() const override {
        return std::make_unique<my_type>(*this);
    }
};

template<typename T>
template<typename U>
AnyResultWrapper<T>::AnyResultWrapper(U&& u) :
  base_type(std::make_any<T>(std::forward<U>(u))) {
    constexpr bool is_clean_t = std::is_same_v<T, std::decay_t<T>>;
    static_assert(is_clean_t,
                  "Template parameter for results must be unqualified");
}

template<typename T>
bool AnyResultWrapper<T>::are_equal_(const AnyFieldBase& rhs) const noexcept {
    // As long as no state is added after AnyFieldWrapper the base check is
    // sufficient
    if(!base_type::are_equal_(rhs)) return false;

    // but to be on the safe side, we make sure the types are the same here too
    auto prhs = dynamic_cast<const my_type*>(&rhs);
    return prhs != nullptr;
}

} // namespace pluginplay::detail_
