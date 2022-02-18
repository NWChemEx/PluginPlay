#pragma once
#include "pluginplay/any/any_field.hpp"
#include <memory>

namespace pluginplay {
namespace detail_ {
class AnyResultBase; // Forward declaration of the PIMPL
}

/** @brief Type-erases module results.
 *
 *  This class extends the AnyField class to type-erased results. AnyResults are
 *  intended to hold the results that a module has computed. Results are always
 *  owned by the AnyResult (as opposed to an AnyInput which may alias its
 *  wrapped value).
 *
 *  The wrapped values must satisfy:
 *  - Copyable
 *  - Value comparable
 *  - serializable (eventually)
 */
class AnyResult : public AnyField {
private:
    /// Type of the base class
    using base_type = AnyField;

public:
    /// Base type of PIMPLs holding result values
    using result_base = detail_::AnyResultBase;

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

    /** @brief Value ctor, creates a new AnyResult by wrapping @p pimpl.
     *
     *  AnyResult instances are made by calling the `make_any_result` free
     *  function. That function will prepare an AnyResultBase and pass it to the
     *  new AnyResult instance via this ctor.
     *
     *  @param[in] pimpl A pointer to an AnyResultBase instance.
     *
     *  @throw None No throw guarantee.
     */
    AnyResult(result_base_pointer pimpl) noexcept;

    /** @brief Creates a new AnyResult by deep copying another instance.
     *
     *  This ctor initializes a new AnyResult with a deep copy of another
     *  instance's state. In particular, if @p other does not contain a value,
     *  the resulting AnyResult will also not contain a value. If @p other does
     *  contain a value the resulting AnyResult will contain a deep copy of the
     *  value @p other wraps.
     *
     *  @param[in] other The AnyResult we are copying.
     *
     *
     *  @throw std::bad_alloc if there is a problem allocating the memory for
     *                        the PIMPL. Strong throw guarantee.
     *  @throw ??? If the wrapped object's copy ctor throws. Same throw
     *             guarantee.
     */
    AnyResult(const AnyResult& other);

    /** @brief Creates a new AnyResult by taking ownership of another instance's
     *         state.
     *
     *  This ctor initializes a new AnyResult by taking ownership of @p other's
     *  state. The actual ownership transfer occurs by transfering the PIMPL
     *  pointer, thus all references and pointers to the wrapped value remain
     *  valid after the transfer.
     *
     *  @param[in,out] other The instance whose state is being taken. After this
     *                       call @p other behaves as if it had been default
     *                       initialized.
     *
     *  @throw None No throw guarantee.
     */
    AnyResult(AnyResult&& other) noexcept;

    /** @brief Overrides the current instance's state with a deep copy of
     *         another AnyResult.
     *
     *  This method will replace (and hence delete) any state held by the
     *  current AnyResult. After the operation the present AnyResult will
     *  contain a deep copy of the state contained in @p rhs. For all intents
     *  and purposes it is as if the present instance had been copy constructed
     *  from @p rhs.
     *
     *  @param[in] rhs The AnyResult whose state is being copied.
     *
     *  @return The current instance, after overriding its state with a copy of
     *          @p rhs.
     *
     *  @throw std::bad_alloc if there is a problem allocating the memory for
     *                        the new PIMPL. Strong throw guarantee.
     *  @throw ??? If the copy ctor of the wrapped value throws. Same throw
     *             guarantee.
     */
    AnyResult& operator=(const AnyResult& rhs);

    /** @brief Overrides the current instance's state with the state of another
     *         AnyResult.
     *
     *  This method will replace (and hence delete) any state held by the
     *  current AnyResult. After the operation the present AnyResult will
     *  contain the state that was previously in @p rhs. The actual transfer of
     *  state occurs by transferring the PIMPL pointer and thus all references
     *  to @p rhs's state remain valid after this call.
     *
     *  @param[in,out] rhs The AnyResult whose state is being taken. After this
     *                     call AnyResult will be in a default initialized
     *                     state.
     *
     *  @return The current instanc, after taking ownership of @p rhs's state.
     *
     *  @throw None No throw guarantee.
     */
    AnyResult& operator=(AnyResult&& rhs) noexcept;

    /// Standard default dtor
    ~AnyResult() noexcept;

    /** @brief Exchanges this instance's state with that of @p other.
     *
     *  This function will swap the PIMPL stored in this instance with that in
     *  @p other. This works even if the PIMPL is a nullptr. After this call
     *  the present instance will contain the state of @p other and @p other
     *  will contain the state of this instance.
     *
     *  @param[in,out] other The AnyResult we are exchanging state with. After
     *                       this call @p other will contain the state
     *                       originally owned by this instance.
     *
     *  @throw None No throw guarantee.
     */
    void swap(AnyResult& other) noexcept { m_pimpl_.swap(other.m_pimpl_); }

protected:
    /// Allows any_cast free function to actually cast the wrapped value
    template<typename T, typename AnyType>
    friend T any_cast(AnyType&&);

    /// Type of a read-only reference to the base class's PIMPL
    using typename base_type::const_field_base_reference;

    /// Type of a mutable reference to the base class's PIMPL
    using typename base_type::field_base_reference;

    /// Type of a mutable reference to a result_base instance
    using result_base_reference = result_base&;

    /// Type of a read-only reference to a a result_base instance
    using const_result_base_reference = const result_base&;

    /// Implements are_equal by comparing types
    bool are_equal_(const base_type& rhs) const noexcept override;

    /// Implements reset(), by calling m_pimpl_.reset()
    void reset_() noexcept override;

    /// Implements has_value() by casting m_pimpl_ to bool
    bool has_value_() const noexcept override;

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
