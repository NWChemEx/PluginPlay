#include <pluginplay/config/config.hpp>
#include <pluginplay/python/python_wrapper.hpp>
#include <stdexcept>
#ifdef ENABLE_PYBIND11
#include <pybind11/pybind11.h>
#else
// Declare a facade object so that this file compiles, but trying to instantiate
// a PythonWrapperPIMPL will cause a runtime error to occur.
namespace pybind11 {

struct object {
    bool is(const object&) const noexcept { return false; }
    explicit operator bool() const noexcept { return false; }
};

} // namespace pybind11
#endif

namespace pluginplay::python::detail_ {

class PythonWrapperPIMPL {
public:
    /// Type Pybind11 uses for passing Python objects into C++
    using object_type = pybind11::object;

    /** @brief Creates a new PythonWrapperPIMPL from the type-erased Python
     *         object.
     */
    explicit PythonWrapperPIMPL(std::any py_value);

    bool operator==(const PythonWrapperPIMPL& rhs) const noexcept {
        return unwrap_any_(py_object).is(unwrap_any_(rhs.py_object));
    }

    bool has_value() const noexcept {
        if(!py_object.has_value()) return false;
        return static_cast<bool>(unwrap_any_(py_object));
    }

    std::any py_object;

private:
    object_type unwrap_any_(std::any py_any) const;
};

inline PythonWrapperPIMPL::PythonWrapperPIMPL(std::any py_value) :
  py_object(std::move(py_value)) {
    if(!with_pybind11())
        throw std::runtime_error("Pybind11 support was not enabled");
}

inline PythonWrapperPIMPL::object_type PythonWrapperPIMPL::unwrap_any_(
  std::any py_any) const {
    if(py_any.has_value()) {
        auto obj = std::any_cast<object_type>(std::move(py_any));
        return object_type(std::move(obj));
    } else
        return object_type();
}

} // namespace pluginplay::python::detail_
