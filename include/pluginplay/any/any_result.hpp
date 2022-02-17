#pragma once
#include "pluginplay/any/any_field.hpp"
#include <memory>

namespace pluginplay {
namespace detail_ {
class AnyResultBase; // Forward declaration of the PIMPL
}

/** @brief Type-erases module results.
 *
 *
 *
 */
class AnyResult : public AnyField {
private:
    /// Type of the base class
    using base_type = AnyField;

public:
    /// Base type of PIMPLs holding result values
    using result_base = detail_::AnyResultBase;

    /// Type of a mutable reference to a result_base instance
    using result_base_reference = result_base&;

    /// Type of a read-only reference to a a result_base instance
    using const_result_base_reference = const result_base&;

    /// Type of a mutable pointer to a result_base instance
    using result_base_pointer = std::unique_ptr<detail_::AnyResultBase>;

    /** @brief Creates an empty AnyResult instance.
     *
     *  The AnyResult instance created by this ctor is more or less a
     *  placeholder. It does not wrap a value. It can be made to wrap a value
     *  by either assigning from an AnyResult which holds a value or by swapping
     *  state with another AnyResult that holds a value.
     *
     *  @throw None No throw guarantee.
     */
    AnyResult() noexcept;

    AnyResult(result_base_pointer pimpl) noexcept;
    ~AnyResult() noexcept;

protected:
    /// Allows any_cast free function to actually cast the wrapped value
    template<typename T, typename AnyType>
    friend T any_cast(AnyType&&);

    /// Type of a read-only reference to the base class's PIMPL
    using typename base_type::const_field_base_reference;

    /// Type of a mutable reference to the base class's PIMPL
    using typename base_type::field_base_reference;

    /// Implements has_value by checking if the PIMPL holds allocated memory
    bool has_value_() const noexcept override {
        return static_cast<bool>(m_pimpl_);
    }

    /// Upcasts the PIMPL to a mutable base class PIMPL, throws if no value
    field_base_reference base_pimpl_() override;

    /// Upcasts the PIMPL to a read-only base class PIMPL, throws if no value
    const_field_base_reference base_pimpl_() const override;

    /// Returns a mutable reference to the PIMPL, throws if no value
    result_base_reference pimpl_();

    /// Returns a read-only reference to the PIMPL, throws if no value
    const_result_base_reference pimpl_() const;

private:
    /// The actual wrapped value
    result_base_pointer m_pimpl_;
};

} // namespace pluginplay
