#pragma once
#include <optional>
#include <typeindex>

namespace pluginplay::python {

/** @brief Wraps std::type_index in a class suitable for Python bindings.
 *
 *  To avoid potential compatibility issues with other Python bindings which
 *  may eventually export `std::type_index`/`std::type_info` we have written
 *  this small wrapper class. This class is really only intended for use with
 *  the C++/Python interface of PluginPlay as Python users should have no need
 *  to access the C++ type and C++ users should be using std::type_index.
 */
class PyTypeInfo {
public:
    /// Type used for storing the RTTI
    using value_type = std::type_index;

    /** @brief Creates a PyTypeInfo object wrapping no type.
     *
     *  The default PyTypeInfo instance does not wrap a type.
     *
     *  @throw None no throw guarantee.
     */
    PyTypeInfo() = default;

    /** @brief Creates a PyTypeInfo from the `typeid` operator.
     *
     *  This ctor is designed to work directly with the result of calling
     *  `typeid` on a type.
     *
     *  @param[in] rtti The RTTI for the type we want to wrap.
     *
     *  @throw None No throw guarantee.
     */
    explicit PyTypeInfo(const std::type_info& rtti) :
      PyTypeInfo(value_type(rtti)) {}

    /** @brief Creates a PyTypeInfo object from a std::type_index.
     *
     *  Since C++11, std::type_index is the preferred means of dealing with
     *  RTTI. This ctor wraps RTTI held by a std::type_index instance.
     *
     *  @param[in] rtti The RTTI for the type we want to wrap.
     *
     *  @throw None No throw guarantee.
     */
    explicit PyTypeInfo(value_type rtti) : m_rtti_(std::move(rtti)) {}

    /** @brief Used to obtain the RTTI in *this.
     *
     *  This is the "getter" of the RTTI held by *this.
     *
     *  @note This method is not exposed to Python.
     *
     *  @return The RTTI wrapped by *this.
     *
     *  @throw std::bad_optional_access if *this does not wrap a value. Strong
     *         throw guarantee.
     */
    value_type value() const { return *m_rtti_; }

    /** @brief Used to determine if *this holds RTTI.
     *
     *  If *this was default constructed (or moved from?) it will not contain
     *  any RTTI. This method is used to determine if *this holds RTTI.
     *
     *  @note If this method returns false then calling value() will throw.
     *
     *  @return True if *this holds RTTI and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool has_value() const noexcept { return m_rtti_.has_value(); }

    /** @brief Is the RTTI in @p rhs is value equal to the RTTI in *this?
     *
     *  This method checks if both *this and @p rhs hold RTTI, and if they do,
     *  is that RTTI value equal? The value equality comparison is done with
     *  `operator==` for `value_type`. If *this does not hold RTTI, then this
     *  method returns true only if @p rhs also does not hold RTTI.
     *
     *  @param[in] rhs The instance to compare against.
     *
     *  @return True if *this compares value equal to @p rhs and false
     *          otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator==(const PyTypeInfo& rhs) const noexcept {
        return m_rtti_ == rhs.m_rtti_;
    }

    /** @brief Is the RTTI in @p rhs different from the RTTI in *this?
     *
     *  This method defines different as not value equal. In turn, this method
     *  simply negates the result of calling operator==.
     *
     *  @param[in] rhs The instance we are comparing against.
     *
     *  @return False if *this compares value equal to @p rhs and true
     *          otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator!=(const PyTypeInfo& rhs) const noexcept {
        return !(*this == rhs);
    }

private:
    /// The RTTI wrapped by *this
    std::optional<value_type> m_rtti_;
};

} // namespace pluginplay::python
