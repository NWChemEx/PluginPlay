#pragma once
#include "pluginplay/any/any_field.hpp"
#include <any>
#include <iostream>

namespace pluginplay::detail_ {

/** @brief Code factorization for AnyInputBase and AnyResultBase.
 *
 *  This class defines the public API of AnyInputBase and AnyResultBase which is
 *  common to both classes. This class is code factorization.
 *
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

    void reset() noexcept { m_value_.reset(); }

    void swap(AnyFieldBase& other) noexcept { m_value_.swap(other.m_value_); }

    bool has_value() const noexcept { return m_value_.has_value(); }

    rtti_type type() const noexcept { return type_(); }

    bool are_equal(const AnyFieldBase& rhs) const noexcept {
        return are_equal_(rhs) && rhs.are_equal_(*this);
    }

    // std::ostream& print(std::ostream& os) const noexcept { return print_(os);
    // }

protected:
    /// AnyWrappers are always created via AnyResultWrapper/AnyInputWrapper
    ///@{
    AnyFieldBase() = default;
    AnyFieldBase(std::any da_any) : m_value_(std::move(da_any)) {}
    AnyFieldBase(const AnyFieldBase& other) = default;
    ///@}

    /// Deleted to avoid slicing
    ///@{
    AnyFieldBase(AnyFieldBase&& other) = delete;
    AnyFieldBase& operator=(const AnyFieldBase&) = delete;
    AnyFieldBase& operator=(AnyFieldBase&&) = delete;
    ///@}

    virtual bool are_equal_(const AnyFieldBase& rhs) const noexcept = 0;

    // virtual std::ostream& print_(std::ostream& os) const noexcept = 0;

    /// The type-erased value
    std::any m_value_;

private:
    virtual rtti_type type_() const noexcept = 0;
};

} // namespace pluginplay::detail_
