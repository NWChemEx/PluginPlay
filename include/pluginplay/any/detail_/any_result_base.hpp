#pragma once
#include "pluginplay/any/any_result.hpp"
#include "pluginplay/any/detail_/any_field_base.hpp"

namespace pluginplay::detail_ {

class AnyResultBase : public AnyFieldBase {
private:
    using any_result_type = AnyResult;

public:
    using AnyFieldBase::rtti_type;
    using result_base_pointer = typename any_result_type::result_base_pointer;

    using AnyFieldBase::AnyFieldBase;

    result_base_pointer clone() const { return clone_(); }

    template<typename T>
    T cast() const;

    template<typename T>
    bool is_convertible() const noexcept;

private:
    /// Trait determining if @p T is unqualified (i.e. no reference or cv)
    template<typename T>
    static constexpr bool is_by_value = std::is_same_v<std::decay_t<T>, T>;

    /// Trait determining if @p T is the type of a const reference
    template<typename T>
    static constexpr bool is_const_ref =
      std::is_same_v<const std::decay_t<T>&, T>;

    template<typename T>
    void assert_convertible_() const;

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
    if(!is_by_value<T> && !is_const_ref<T>) return false;
    return std::any_cast<std::decay_t<T>>(&m_value_) != nullptr;
}

template<typename T>
void AnyResultBase::assert_convertible_() const {
    static_assert(
      is_by_value<T> || is_const_ref<T>,
      "Results must be retrieved by value or by read-only reference.");
    if(is_convertible<T>()) return;
    std::string msg = "Can not cast AnyResult to type: ";
    msg += type().name();
    throw std::runtime_error(msg);
}

} // namespace pluginplay::detail_
