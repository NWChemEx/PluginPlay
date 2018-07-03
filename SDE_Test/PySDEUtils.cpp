#include <SDE/Pythonization.hpp>
#include <SDE/SDEAny.hpp>
#include <catch/catch.hpp>

/* This file contains C++ exports for testing various aspects of the SDE from
 * Python.  Generally speaking the contents of this file should not be used to
 * test Python bindings for a class.  Rather it's for creating facades for
 * testing aspects of the C++/Python interoperability (e.g. mock property types,
 * or type-erasure).
 */

// Exposes SDEAny to Python as a default constructable opaque type
struct SDEAnyWrapper {
    SDEAnyWrapper() = default;
    SDE::detail_::SDEAny my_any;
};

// Declares the Python module py_sde_utils (name must match .so)
PYBIND11_MODULE(py_sde_utils, m) {
    // Exposes our SDEAny wrapper
    pybind11::class_<SDEAnyWrapper>(m, "SDEAnyWrapper")
      .def(pybind11::init<>())
      .def("get", [](SDEAnyWrapper& self) { return self.my_any.pythonize(); });

    // Returns an SDEAny filled with a vector [1, 2, 3]
    m.def("make_any", []() {
        std::vector<int> v1{1, 2, 3};
        return SDEAnyWrapper{SDE::detail_::SDEAny(v1)};
    });

    // Returns an SDEAny filled with a Python object
    m.def("make_any", [](pybind11::object a_list) {
        return SDEAnyWrapper{SDE::detail_::SDEAny(a_list)};
    });
} // End PYBIND11_MODULE
