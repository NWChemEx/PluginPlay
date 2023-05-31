/*
 * Copyright 2023 NWChemEx-Project
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
#include <any>
#include <stdexcept>
#ifdef BUILD_PYBIND11
#include <memory>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace pluginplay::python {

/** @brief Wraps a Python object so it can be used with AnyField.
 *
 *  Using the AnyField class with pybind11 Python objects almost works out of
 *  the box, we just need a few redirections. The PythonWrapper class wraps a
 *  pybind11 Python object and gives it the expected API.
 *
 *  @note We actually hold the pybind11 object in a std::any. This is because
 *        Pybind11 has hidden symbols by default an trying to directly hold the
 *        Pybind11 object leads to compiler warnings.
 */
class PythonWrapper {
public:
    /// How we hold the object internally
    using value_type = std::any;

    /// The type of a Python object, as seen from C++
    using py_object_type = pybind11::object;

    /// Type of a reference to a Python object
    using py_reference = py_object_type&;

    /// Type of a read-only reference to a Python object
    using const_py_reference = const py_object_type&;

    /** @brief Creates a PythonWrapper which wraps @p py_value
     *
     *  This ctor takes the Python object to wrap by copy. That said, Python
     *  objects are essentially shared pointers, so the copy is itself a
     *  reference counted shallow copy.
     *
     *  @param[in] py_value The Python object to wrap
     *
     *  @throw ??? if making the internal std::any fails. Strong throw
     *             guarantee.
     */
    explicit PythonWrapper(py_object_type py_value) :
      m_value_(std::make_any<py_object_type>(py_value)) {}

    /** @brief Determines whether or not the Python object *this was
     *         constructed with actually holds a value.
     *
     *  Default constructed Python objects are essentially null shared pointers.
     *  This method is used to check whether the internal Python object
     *  actually is associated with a Python object or if it's null.
     *
     *  @return True if the internal Python object holds a value and false
     *          otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool has_value() const noexcept { return static_cast<bool>(unwrap_()); }

    /** @brief Can the wrapped Python object be converted to type @p T?
     *
     *  @tparam T The type we are trying to convert the wrapped object to.
     *
     *  @return True if *this wraps an object and that object can be converted
     *          to an object of type @p T, false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    template<typename T>
    bool is_convertible() noexcept;

    /** @brief Determines if the wrapped Python object can be turned into a
     *         C++ object of type @p T.
     *
     *  If *this does not hold a value this method will return false (one can
     *  not convert nothing to something...). If *this does hold a value this
     *  method will attempt to cast the held value to a C++ object of type
     *  @p T. The method then returns the result of whether or not the cast
     *  passed.
     *
     *  @tparam T The type we are attempting to cast to. @p T can be an
     *          unqualified type or cv-qualified reference type.
     *
     *  @return True if the wrapped object can be converted to an object of type
     *          @p T and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    template<typename T>
    bool is_convertible() const noexcept;

    /** @brief Converts the wrapped Python object to an object of type @p T.
     *
     *  @tparam T The C++ type of the object we want to get back. @p T can be
     *          an unqualified or cv-qualified reference.
     *
     *  @return The wrapped Python object converted to a C++ object of type
     *          @p T.
     *
     *  @throw ??? May throw if the conversion throws.
     */
    template<typename T>
    T unwrap();

    /** @brief Wraps the process of unwrapping *this to a C++ object.
     *
     *  @tparam T The C++ type we want to get the wrapped object back as. @p T
     *          can be an unqualified type, or a cv-qualified reference to a
     *          C++ type.
     *
     *  @return A C++ representation of the Python object held by *this.
     *
     */
    template<typename T>
    T unwrap() const;

    /** @brief Value comparison of the wrapped Python object.
     *
     *  This method will value compare the Python object wrapped by *this to
     *  that wrapped by @p rhs. Note that the internal comparison is by value,
     *  and not a comparison of the object handles.
     *
     *  @param[in] rhs The instance to compare against.
     *
     *  @return True if the wrapped values compare equal, or if *this and
     *          @p rhs both do not have a wrapped value. This method returns
     *          false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator==(const PythonWrapper& rhs) const noexcept;

    /** @brief Determines if *this is different from @p rhs.
     *
     *  This method simply negates operator==.
     *
     *  @param[in] rhs The instance to compare against.
     *
     *  @return False if *this is value equal to @p rhs and true otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator!=(const PythonWrapper& rhs) const noexcept {
        return !(*this == rhs);
    }

    /** @brief Returns a string representation of the object held by *this.
     *
     *  This method is intended for logging the value of an object, not for
     *  serialization.
     *
     *  @return A string representation of the object in *this.
     *
     *  @throw std::bad_alloc if creating the string representation fails.
     *                        Strong throw guarantee.
     */
    std::string as_string() const {
        // TODO: Use the Python __str__ method?
        return has_value() ? "Pybind11 Object" : std::string{};
    }

private:
    /// Code factorization for unwrapping the mutable std::any
    py_reference unwrap_() { return std::any_cast<py_reference>(m_value_); }

    /// Code factorization for unwrapping the read-only std::any
    const_py_reference unwrap_() const {
        return std::any_cast<const_py_reference>(m_value_);
    }

    /// The actual Python object, held in a std::any
    value_type m_value_;

    /// A buffer for holding the newly created C++ object
    mutable value_type m_buffer_;
};

/** @brief Ensures the PythonWrapper class can be passed to a std::ostream.
 *
 *  This method simply calls @p pywrap's `as_string` method and passes the
 *  result to @p os.
 *
 *  @param[in] os The stream to add @p pywrap to.
 *  @param[in] pywrap The PythonWrapper class to print.
 *
 *  @return @p os, after passing @p pywrap to it.
 *
 *  @throw std::bad_alloc if @p pywrap's `as_string` method throws. Strong
 *                        throw guarantee.
 */
inline std::ostream& operator<<(std::ostream& os, const PythonWrapper& pywrap) {
    return os << pywrap.as_string();
}

/** @brief Wraps the process of making a PythonWrapper object around a
 *         C++ object.
 *
 *  This method provides a uniform API for creating a PythonWrapper object from
 *  an arbitrary C++ object. The object wrapped by the PythonWrapper will be a
 *  Python object. Meaning that C++ objects are first converted to Python
 *  objects before constructing the PythonWrapper.
 *
 *  @tparam T The type of the object to wrap. Deduced automatically by the
 *          compiler.
 *
 *  @param[in] cxx_value The object to wrap.
 *
 *  @return The PythonWrapper which wraps the provided object.
 *
 *  @throw ??? If wrapping the object throws. Same throw guarantee.
 */
template<typename T>
PythonWrapper make_python_wrapper(T&& cxx_value) {
    using py_object_type    = typename PythonWrapper::py_object_type;
    py_object_type py_value = pybind11::cast(std::forward<T>(cxx_value));
    return PythonWrapper(std::move(py_value));
}

// -----------------------------------------------------------------------------
// -- Inline Implementations
// ---------------------------------------------------
// -----------------------------------------------------------------------------

inline bool PythonWrapper::operator==(const PythonWrapper& rhs) const noexcept {
    if(has_value() != rhs.has_value()) return false;
    if(!has_value()) return true;
    return unwrap_().equal(rhs.unwrap_());
}

template<typename T>
bool PythonWrapper::is_convertible() noexcept {
    // If we don't have a value we're not convertible
    if(!has_value()) return false;

    using clean_type = std::decay_t<T>;
    // Does the user want a pybind11::object, or a reference to one?
    if constexpr(std::is_same_v<clean_type, py_object_type>) {
        return true;
    }
    // Does the user want a PythonWrapper, or a reference to one?
    else if constexpr(std::is_same_v<clean_type, PythonWrapper>) {
        return true;
    }
    // User wants us to try converting the Python object to a C++ object
    else {
        try {
            if constexpr(std::is_copy_constructible_v<clean_type>) {
                unwrap_().cast<clean_type>();
                return true;
            } else
                return false;
        } catch(...) { return false; }
    }
}

template<typename T>
bool PythonWrapper::is_convertible() const noexcept {
    using clean_type              = std::decay_t<T>;
    constexpr bool to_mutable_ref = std::is_same_v<T, clean_type&>;

    // Rule out mutable references
    if constexpr(to_mutable_ref) { return false; }

    // Now it's the same as non-const is_convertible
    return const_cast<PythonWrapper*>(this)->is_convertible<T>();
}

template<typename T>
T PythonWrapper::unwrap() {
    if(!has_value())
        throw std::runtime_error("Does not hold a value to unwrap");

    using clean_type = std::decay_t<T>;
    auto& py_value   = unwrap_();

    // Want a pybind11::object, pybind11::object&, or const pybind11::object&?
    if constexpr(std::is_same_v<clean_type, py_object_type>) {
        return py_value;
    }
    // Want a PythonWrapper, PythonWrapper&, or const PythonWrapper&?
    else if constexpr(std::is_same_v<clean_type, PythonWrapper>) {
        return *this;
    }
    // Want the Python object converted to a C++ value?
    else if constexpr(std::is_same_v<clean_type, T>) {
        return py_value.cast<clean_type>();
    }
    // User wants the converted C++ value by reference or const reference
    else {
        if(!m_buffer_.has_value()) {
            auto cxx_value = py_value.cast<clean_type>();
            m_buffer_      = std::make_any<clean_type>(std::move(cxx_value));
        }
        return std::any_cast<T>(m_buffer_);
    }
}

template<typename T>
T PythonWrapper::unwrap() const {
    using clean_type              = std::decay_t<T>;
    constexpr bool is_mutable_ref = std::is_same_v<T, clean_type&>;
    static_assert(!is_mutable_ref, "Can not unwrap to mutable reference");

    // okay to use non-const version since we know user doesn't want a mutable
    // reference
    return const_cast<PythonWrapper*>(this)->unwrap<T>();
}

} // namespace pluginplay::python

#else

namespace pluginplay::python {

/** @brief Stub class so PluginPlay can compile w/o Pybind11 support.
 *
 *  This class simply defines the pieces of the PythonWrapper class needed by
 *  the AnyField so that we don't have to #ifdef off code in the AnyField.
 *  Trying to call any of the methods in this version of PythonWrapper will
 *  result in runtime errors.
 */
class PythonWrapper {
public:
    PythonWrapper() { error_(); }

    template<typename T>
    PythonWrapper(T&&) {
        error_();
    }

    template<typename T>
    T unwrap() {
        error_();
        return std::any_cast<T>(m_value_); // Won't actually get here...
    }

    template<typename T>
    T unwrap() const {
        error_();
        return std::any_cast<T>(m_value_); // Won't actually get here...
    }

    template<typename T>
    bool is_convertible() const noexcept {
        return false;
    }

    bool operator==(const PythonWrapper& rhs) const noexcept { return true; }

    bool operator!=(const PythonWrapper& rhs) { return false; }

private:
    std::any m_value_;
    void error_() const {
        throw std::runtime_error("PluginPlay was not configured with Pybind11 "
                                 "support");
    }
};

template<typename T>
PythonWrapper make_python_wrapper(T&& cxx_value) {
    return PythonWrapper();
}

} // namespace pluginplay::python
#endif
