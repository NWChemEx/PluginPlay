#pragma once
#include <stdexcept>
#ifdef BUILD_PYBIND11
#include <any>
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
     *          unqualified or reference type with optional cv-qualifications.
     *
     *  @return True if the wrapped object can be converted to an object of type
     *          @p T and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    template<typename T>
    bool is_convertible() const noexcept;

    /** @brief Wraps the process of unwrapping *this to a C++ object.
     *
     *  @tparam T The C++ type we want to get the wrapped object back as. @p T
     *          can be an unqualified type, or a cv-qualified reference to a
     *          C++ type. Note that if @p T is the same as py_object_type
     *          this method will return the wrapped Python object and not
     *          attempt to convert it to C++.
     *
     *  @return A C++ representation of the Python object held by *this.
     *
     */
    template<typename T>
    T unwrap() const;

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
    /// Code factorization for unwrapping the std::any
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

// -----------------------------------------------------------------------------
// -- Inline Implementations ---------------------------------------------------
// -----------------------------------------------------------------------------

inline bool PythonWrapper::operator==(const PythonWrapper& rhs) const noexcept {
    if(has_value() != rhs.has_value()) return false;
    if(!has_value()) return true;
    return unwrap_().equal(rhs.unwrap_());
}

template<typename T>
bool PythonWrapper::is_convertible() const noexcept {
    using clean_type = std::decay_t<T>;
    if(!has_value()) return false;
    if constexpr(std::is_same_v<clean_type, py_object_type>) { return true; }
    try {
        auto cxx_value = unwrap_().cast<clean_type>();
        return true;
    } catch(...) { return false; }
}

template<typename T>
T PythonWrapper::unwrap() const {
    using clean_type     = std::decay_t<T>;
    const auto& py_value = unwrap_();
    if constexpr(std::is_same_v<clean_type, py_object_type>) {
        return py_value;
    } else if constexpr(std::is_same_v<clean_type, T>) {
        return py_value.cast<clean_type>();
    } else {
        if(!m_buffer_.has_value()) {
            auto cxx_value = py_value.cast<clean_type>();
            m_buffer_      = std::make_any<clean_type>(std::move(cxx_value));
        }
        return std::any_cast<T>(m_buffer_);
    }
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
    template<typename T>
    T unwrap() const {
        error_();
    }

    template<typename T>
    bool is_convertible() const {
        error_();
    }

private:
    void error_() {
        throw std::runtime_error("PluginPlay was not configured with Pybind11 "
                                 "support");
    }
};

} // namespace pluginplay::python
#endif
