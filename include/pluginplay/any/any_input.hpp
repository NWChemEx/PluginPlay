#pragma once
#include "pluginplay/any/any_field.hpp"
#include "pluginplay/hasher.hpp"

namespace pluginplay {
namespace detail_ {
class AnyInputBase; // Forward declare PIMPL
}

/** @brief Extends AnyField to Module inputs.
 *
 *  AnyInput instances type-erase inputs to modules. Module inputs are
 *  restricted to being strictly inputs (i.e. they can not be mutable
 *  references). PluginPlay allows inputs to be passed into a module in three
 *  ways: by value, by const value, and by const reference. In the former two
 *  scenarios the value is owned by the AnyInput instance, whereas in the latter
 *  scenario the value is only aliased.
 *
 *  Retrieving a value from an AnyInput can always be done by copy, const copy,
 *  or by const reference. Additionally, if the AnyInput owns a mutable copy of
 *  the initial input value, users may retrieve the value from the AnyInput as a
 *  mutable reference.
 *
 *  In addition to the methods provided by the AnyField class, the AnyInput
 *  class also adds hashing capabilities. Consequently, values which are wrapped
 *  in AnyInput instances must satisfy:
 *  - copyable
 *  - comparable (via operator==)
 *  - hashable
 */
class AnyInput : public AnyField {
public:
    /// Unqualified type of the object used for hashing
    using hasher_type = pluginplay::Hasher;

    /// Mutable reference to a hasher_type
    using hasher_reference = hasher_type&;

    /// Unqualified type of the PIMPL
    using input_base = detail_::AnyInputBase;

    /// How we store the PIMPL
    using input_base_pointer = std::unique_ptr<input_base>;

    /** @brief Creates an empty AnyInput
     *
     *  The default ctor creates an AnyInput which does not hold a value. The
     *  only way to make the resulting instance hold a value is by assignment
     *  or by swapping state with an AnyInput that does hold a value. The object
     *  resulting from the default ctor is meant for use primarily as a
     *  placeholder.
     *
     *  @throw None No throw guarantee.
     */
    AnyInput() noexcept;

    /** @brief Value Ctor
     *
     *  This ctor creates a new AnyInput which wraps the provided value. The
     *  value is provided to the AnyInput already wrapped up in a PIMPL
     *  instance. Users who want to create a new AnyInput should use the
     *  make_any_input free function, which wraps the process of creating the
     *  PIMPL and forwarding it to this ctor.
     *
     *  @param[in] value The PIMPL this instance should use. Already contains
     *                    the wrapped value.
     *
     *  @throw None No throw guarantee.
     */
    AnyInput(input_base_pointer value) noexcept;

    /** @brief Creates a new AnyInput by copying another instance.
     *
     *  This ctor creates a new AnyInput by copying the state of another
     *  instance. If the other instance owns the value (i.e., holds it by either
     *  value or const value) the copy will be a deep copy (the exact definition
     *  of what a deep copy is will depend on the copy ctor of wrapped object).
     *  If the other instance aliases the wrapped value the resulting instance
     *  will also alias the wrapped value. If the other instance does not wrap a
     *  value, the newly created AnyInput will also not wrap a value.
     *
     *  @param[in] other The AnyInput whose state is being copied.
     *
     *  @throw std::bad_alloc if there is a problem allocating the new PIMPL.
     *                        Strong throw guarantee.
     *  @throw ??? If the wrapped value's ctor throws. Same throw guarantee.
     */
    AnyInput(const AnyInput& other);

    /** @brief Creates a new AnyInput instance by taking another instance's
     *         state.
     *
     *  This ctor creates a new AnyInput instance by taking ownership of another
     *  instance's state. The actual move is done by taking the PIMPL pointer
     *  from @p other. Consequently, any references to the wrapped instance will
     *  continue to remain valid after this method is called.
     *
     *  @param[in,out] other The instance we are taking the state from. After
     *                       this call @p other will be in a state consistent
     *                       with default initialization.
     *
     * @throw None No throw guarantee.
     */
    AnyInput(AnyInput&& other) noexcept;

    /** @brief Replaces the current instance's state with a copy of another
     *         instance's state.
     *
     *  This method will overide the state of the current instance (in turn
     *  deleting it) with a copy of another instance's state. The copy semantics
     *  follow the AnyInput copy ctor (deep copy if @p rhs owns its value, and
     *  shallow copy if @p rhs alisases its value).
     *
     *  @param[in] rhs The instance whose state is being copied.
     *
     *  @return The current instance, after replacing its internal state with a
     *          copy of @p rhs's state.
     *
     *  @throw ??? If calling the AnyInput copy ctor on @p rhs throws. Same
     *             throw guarantee.
     */
    AnyInput& operator=(const AnyInput& rhs);

    /** @brief Overwrites the state of the current instance with the state of
     *         another instance.
     *
     *  This method can be used to replace the state of the current instance
     *  with the state of another instance. The state the present instance owns
     *  prior to the call will be released (resulting in the wrapped object
     *  being deleted if the AnyInput owned the value) before taking ownership
     *  of @p rhs's state. The actual state is transferred by taking the PIMPL
     *  pointer from @p rhs and thus all references to the value wrapped by
     *  @p rhs remain valid after this operation.
     *
     *  @param[in,out] rhs The instance whose state is being taken. After this
     *                     method is caleld @p rhs will be in a state consistent
     *                     with being default initialized.
     *
     *  @return The current instance after taking ownership of @p rhs's state.
     *
     *  @throw None No throw guarantee.
     */
    AnyInput& operator=(AnyInput&& rhs) noexcept;

    /// Default dtor
    ~AnyInput() noexcept;

    /** @brief Exchanges this instance's state with that of @p other.
     *
     *  This method is used to exchange the state of two AnyInput instances.
     *  This is done by swapping the pointers to the PIMPLs. Consequently, all
     *  references to the wrapped object remain valid after the operation.
     *
     *  @param[in,out] other The instance whose state is being swapped with the
     *                       present instance's state. After this call @p other
     *                       will contain the present instance's state.
     *
     *  @throw None No throw guarantee.
     */
    void swap(AnyInput& other) noexcept;

    /** @brief Hashes the current instance.
     *
     *  Assuming the current instance wraps a value of qualified type T (i.e.
     *  T includes cv-qualifiers and references). The wrapped object will be
     *  hashed as an object of type `const std::decay_t<T>&` (the type being
     *  relevant for hashers which also hash the type of the object).
     *
     *  This method is a no-op if the current instance does not wrap a value.
     *
     *  @param[in,out] h The object to use for hashing. After a call to this
     *                   method the internal state of @p h will be updated to
     *                   include a hash of the wrapped value.
     */
    void hash(hasher_reference h) const;

protected:
    /// Allows any_cast free function to actually cast the wrapped value
    template<typename T, typename AnyType>
    friend T any_cast(AnyType&&);

    /// Type of the base class
    using base_type = AnyField;

    /// Type of a read-only reference to the base class's PIMPL
    using typename base_type::const_field_base_reference;

    /// Type of a mutable reference to the base class's PIMPL
    using typename base_type::field_base_reference;

    /// Type of a mutable reference to a input_base instance
    using input_base_reference = input_base&;

    /// Type of a read-only reference to an input_base instance
    using const_input_base_reference = const input_base&;

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
    input_base_reference pimpl_();

    /// Returns a read-only reference to the PIMPL, throws if no value
    const_input_base_reference pimpl_() const;

private:
    /// The actual wrapped value
    input_base_pointer m_pimpl_;
};

} // namespace pluginplay
