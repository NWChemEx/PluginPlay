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
#include <optional>
#include <utility>

namespace pluginplay::cache::database {

/** @brief Wraps a database value so users don't need to worry about memory
 *
 *  When a user gets a value back from a database there can end up being very
 *  different memory semantics. For example, if the value is in memory the
 *  database can give it back by reference to avoid a copy. In other scenarios
 *  the database may actually give the user a value (for example because it was
 *  pulled from disk) and expect the user to manage the lifetime. The Value
 *  class hides the return semantics of the database in a uniform API, which
 *  allows the user to always interact with the value as if it was given to them
 *  by reference.
 *
 *  N.B. Since enteries in the cache are immutable there is no fear of users
 *  trying to modify a temporary owned by the Value class and expecting the
 *  changes to propagate back.
 *
 *  N.B. Right now this uses raw pointers. When the upstream classes are written
 *       this decision will be revisited to better manage the lifetime. Most
 *       likely by also templating on the pointer type.
 *
 *  @tparam T The type of the object being wrapped. Expected to be a non
 *            qualified type or a const qualified type.
 *
 */
template<typename T>
class DBValue {
public:
    /// The type being wrapped, typedef of @p T
    using value_type = T;

    /// Type of a (possibly) read/write reference to the wrapped object
    using reference = value_type&;

    /// Type of a (possibly) read/write pointer to the wrapped object
    using pointer = value_type*;

    /// Type of a read-only reference to the wrapped object
    using const_reference = const value_type&;

    /** @brief Value ctor
     *
     *  This ctor will store a copy of the provided object. To avoid the copy
     *  @p v can be moved into the ctor as long as the caller is okay
     *  transferring ownership.
     *
     *  @param[in] v The value to be wrapped.
     *
     *  @throw ??? Throws if @p T 's move ctor throws. Same throw guarantee.
     */
    explicit DBValue(value_type v) : m_value_(std::move(v)), m_ptr_(nullptr) {}

    /** @brief Aliasing ctor and default ctor.
     *
     *  If the user provides a pointer, this ctor will wrap it, otherwise the
     *  resulting instance will wrap the nullptr. If the caller provides a
     *  pointer, it is the responsability of the caller to manage the lifetime
     *  of the value.
     *
     *  @param[in] p The address of the object being wrapped in the Value.
     *               Defaults to the null pointer.
     *
     *  @throw None No throw guarantee.
     */
    explicit DBValue(pointer p = nullptr) : m_ptr_(p) {}

    bool has_value() const noexcept;

    /** @brief Returns the wrapped object.
     *
     *  This method allows you to get a (possibly) read/write reference to the
     *  wrapped object. Whether the reference is read/write or read-only
     *  depends on @p T. If @p T is a const value, then the reference is
     *  read-only otherwise it's read/write.
     *
     *  @return A (possibly) read/write reference to the wrapped object.
     *
     *  @throw None No throw guarantee.
     */
    reference get();

    /** @brief Returns the wrapped object.
     *
     *  This method is the same as the other overload of get() except that the
     *  returned value can not be modified.
     *
     *  @return A read-only reference to the wrapped value.
     *
     *  @throw None No throw guarantee.
     */
    const_reference get() const;

private:
    /// If we own the value it's in the optional
    std::optional<T> m_value_;

    /// If we alias it this is it's address
    pointer m_ptr_;
};

/// Type of a Value which wraps an immutable object of type @p T
template<typename T>
using ConstDBValue = DBValue<const T>;

// -----------------------------------------------------------------------------
// -- Inline Implementations
// -----------------------------------------------------------------------------

template<typename T>
bool DBValue<T>::has_value() const noexcept {
    return m_value_ || m_ptr_;
}

template<typename T>
typename DBValue<T>::reference DBValue<T>::get() {
    return m_value_ ? *m_value_ : *m_ptr_;
}

template<typename T>
typename DBValue<T>::const_reference DBValue<T>::get() const {
    return m_value_ ? *m_value_ : *m_ptr_;
}

} // namespace pluginplay::cache::database
