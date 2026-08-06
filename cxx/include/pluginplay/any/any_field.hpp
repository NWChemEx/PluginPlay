/*
 * Copyright 2022 NWChemEx-Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once
#include "detail_/any_field_base.hpp"

namespace pluginplay::any {

/** @brief Wraps either an input or a result to a module.
 *
 *  The AnyField class is responsible for type-erasure of inputs and results
 *  to/from modules. For the most part users of PluginPlay shouldn't need to
 *  deal directly with AnyField; however, it indirectly affects users on account
 *  of the restrictions it places on inputs/results. We have tried to keep those
 *  restrictions to a minimum. Presently the required properties in order for a
 *  type to be wrappable are:
 *
 *  - must be copyable
 *  - overload operator== to compare two instances for value equality
 *  - overlaod operator< to define an ordering of two instances
 *
 *  Optionally the type may:
 *
 *  - overload std::ostream::operator<< for printing the value.
 *
 *  AnyField defines default implementations for any optional properties the
 *  type does not satisfy.
 */
class AnyField {
public:
    /// Type of the object actually implementing the AnyField class
    using pimpl_type = detail_::AnyFieldBase;

    /// Type used for runtime type information (RTTI) purposes
    using rtti_type = typename pimpl_type::rtti_type;

    /// Type of the smart pointer holding a PIMPL, typedef of unique_ptr
    using pimpl_pointer = typename pimpl_type::field_base_pointer;

    /** @brief Creates an AnyField wrapping the provided value.
     *
     *  This ctor serves as both the default and value ctors. By default it will
     *  wrap a null value. If a user supplies a PIMPL containing a wrapped value
     *  then the AnyField will instead wrap that value.
     *
     *  @param[in] pimpl A pointer to the PIMPL which holds the wrapped value.
     *                   Defaults to a null pointer.
     *
     *  @throw None No throw guarantee.
     */
    explicit AnyField(pimpl_pointer pimpl = nullptr) noexcept;

    /** @brief Creates a new AnyField by deep copying an existing instance.
     *
     *  This ctor will deep copy the value held inside @p other. In particular,
     *  this means that if @p other aliases its value the new AnyField will
     *  hold a copy of the wrapped value, NOT an alias. If you want to avoid
     *  deep copying the wrapped value work with references/pointers to an
     *  AnyField and/or use move construction.
     *
     *  @param[in] AnyField The instance we are copying.
     *
     *  @throw std::bad_alloc if there is a problem copying. Strong throw
     *                        guarantee.
     */
    AnyField(const AnyField& other);

    /** @brief Takes the state from an already existing AnyField instance.
     *
     *  This ctor creates a new instance whose state is the state of an already
     *  existing instance.
     *
     *  @param[in,out] other The AnyField instance whose state is being taken.
     *                       After this call AnyField will be in a default
     *                       initialized state.
     *
     *  @throw None No throw guarantee.
     *
     */
    AnyField(AnyField&& other) noexcept;

    /** @brief Replaces the existing state with a deep copy of another AnyField
     *
     *  This method deep copies the state of @p rhs, sets the current instance
     *  to the copied state, and then releases the instance's old state. After
     *  this call any references/pointers to the previous state are invalid.
     *
     *  @param[in] AnyField The instance we are copying.
     *
     *  @return The current instance, after overwriting its state.
     *
     *  @throw std::bad_alloc if there is a problem copying. Strong throw
     *                        guarantee.
     */
    AnyField& operator=(const AnyField& rhs);

    /** @brief Replaces the existing state with another AnyField's state
     *
     *  This method takes the state from @p rhs, sets the current instance
     *  to the taken state, and then releases the instance's old state. After
     *  this call any references/pointers to this instance's previous state are
     *  invalid.
     *
     *  @param[in,out] AnyField The instance we are taking the state from.
     *                          After this call AnyField will be in a default
     *                          constructed state.
     *
     *  @return The current instance, after overwriting its state.
     *
     *  @throw std::bad_alloc if there is a problem copying. Strong throw
     *                        guarantee.
     */
    AnyField& operator=(AnyField&& rhs) noexcept;

    /// Default non-throw dtor
    ~AnyField() noexcept;

    /// Standard swap function, just exchanges PIMPL instances
    void swap(AnyField& other) noexcept;

    /** @brief Restores the AnyField to a default initialized state.
     *
     *  This method can be used to release the held pointer. For instances which
     *  own their wrapped value, this will delete the wrapped value. For
     *  instances which only alias their value, this will only release the
     *  handle to the original value, i.e. the original value will continue to
     *  exist. If the current AnyField instance does not hold a value this is a
     *  no-op.
     *
     *  @throw None No throw guarantee.
     */
    void reset() noexcept;

    /** @brief Retrieves the RTTI of the wrapped type.
     *
     *  At any given time an AnyField either wraps a value or does not. If it
     *  wraps a value this function will return the RTTI of the wrapped value.
     *  If it does not wrap a type, it will return the RTTI of a nullptr.
     *
     *  N.B. the RTTI does not take into account cv-qualifiers or references.
     *
     *  @return The RTTI of the wrapped value, if has_value is true. Otherwise
     *          the RTTI of a nullptr will be returned.
     *
     *  @throw None No throw guarantee.
     */
    rtti_type type() const noexcept;

    /** @brief Determines if the wrapped value can be converted to the specified
     *         type.
     *
     *  Assuming the present instance wraps a value (if it does not this method
     *  will always return false) this method determines if a call to
     *  `any_cast<T>(*this)` will succeed. It should be noted that @p T should
     *  be cv and reference qualified, i.e., this method takes into account the
     *  `const`-ness and reference-semantics of @p T.
     *
     *  @tparam T A cv and reference qualified type.
     *
     *  @return True if it is possible to convert the wrapped object to an
     *          instance of type @p T and false otherwise. Notably, if this
     *          instance does not contain a PIMPL this method returns false.
     *
     *  @throw None No throw guarantee.
     */
    template<typename T>
    bool is_convertible() const noexcept;

    /** @brief Performs a value comparison on two AnyField instances.
     *
     *  This method first compares whether the two instances both wrap values,
     *  then (assumign they do wrap values) comparse the wrapped values. These
     *  comparisons ignore how the value is held. So for example if one instance
     *  owns an instance of the integer 42 and another references the integer
     *  42, this method will return true even though one holds it by value and
     *  the other wraps a reference.
     *
     *  @param[in] rhs The AnyField instance we ware comparing against.
     *
     *  @return True if the wrapped values are value equal and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator==(const AnyField& rhs) const noexcept;

    /** @brief Polymorphically compares two AnyField objects.
     *
     *  While AnyField is not polymorphic, it's PIMPL is. The internal
     *  polymorphic comparison compares more than just the wrapped values it
     *  also compares how the values are held. So for example if one instance
     *  owns an instance of the integer 42 and another references the integer
     *  42, this method will return false because one is wrapped by value and
     *  the other is wrapped by reference.
     *
     *  In general this function returns true if two AnyField instances:
     *  - agree on the result of `has_value`
     *  - hold their wrapped values the same way (e.g. by ref, by const ref)
     *  - the wrapped values compare equal
     *
     *  @return True if this AnyField instance is "polymorphically" value equal
     *          to @p rhs and false otherwise.
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
     *  is used to determine if the AnyField wraps a value. Whether the AnyField
     *  owns the value is not considered.
     *
     *  @return True if the PIMPL is non-null and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool has_value() const noexcept;

    /** @brief Does the AnyField own the wrapped value?
     *
     *  When `has_value()` is true this instance wraps a value. The instance
     *  may actually own the value it wraps (for example it copied the value at
     *  construction) or it may alias it (was given a reference to the value).
     *  This method will return true if this instance owns the value it wraps.
     *  In particular, `owns_value()` equals true means that the wrapped object
     *  will exist as long as this instance exists, and conversely that if the
     *  aliased value goes out of scope, this instance will contain a dangling
     *  reference.
     *
     *  To create an owning instance from one that does not own, copy it.
     *
     *  @note Instances holding Python objects own them. This is because
     *        unwrapping Python objects requires creating a C++ object in a
     *        buffer and the buffer will be owned by *this. Even if the Python
     *        object is not unwrapped, we hold a reference counted instance
     *        and thus it will not go out of scope, consistent with the C++
     *        behavior of this method.
     *
     *  @return True if this instance wraps a value, and it owns that value.
     *          False otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool owns_value() const noexcept;

    template<typename Archive>
    void save(Archive& ar) const {
        std::runtime_error("NYI");
    }

    template<typename Archive>
    void load(Archive& ar) {
        std::runtime_error("NYI");
    }

private:
    /// Allows any_cast to actually cast the AnyField
    template<typename T, typename AnyType>
    friend T any_cast(AnyType&&);

    /// The actual PIMPL
    pimpl_pointer m_pimpl_;
};

template<typename T>
bool AnyField::is_convertible() const noexcept {
    if(!m_pimpl_) return false;
    return m_pimpl_->is_convertible<T>();
}

/** @brief Determines if two AnyField instances are different.
 *
 *  @relates AnyField
 *
 *  Two AnyField instances are different if operator== returns false. This
 *  function simply negates the result of operator==. See operator== for the
 *  definition of equality.
 *
 *  @param[in] lhs The AnyField on the left of the operator.
 *  @param[in] rhs The AnyField on the right of the operator.
 *
 *  @return False if lhs == rhs, and true otherwise.
 *
 *  @throw None No throw guarantee.
 */
inline bool operator!=(const AnyField& lhs, const AnyField& rhs) {
    return !(lhs == rhs);
}

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

} // namespace pluginplay::any
