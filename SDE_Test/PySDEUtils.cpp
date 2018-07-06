#include <SDE/Pythonization.hpp>
#include <SDE/SDEAny.hpp>
#include <SDE/PyBindings/PyParameters.hpp>
#include <catch/catch.hpp>

/* This file contains C++ exports for testing various aspects of the SDE from
 * Python.  Generally speaking the contents of this file should not be used to
 * test Python bindings for a class.  Rather it's for creating facades for
 * testing aspects of the C++/Python interoperability (e.g. mock property types,
 * or type-erasure).
 */

using namespace SDE;

// Exposes SDEAny to Python as a default constructable opaque type
struct SDEAnyWrapper {
    SDEAnyWrapper() = default;
    detail_::SDEAny my_any;
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
        return SDEAnyWrapper{detail_::SDEAny(v1)};
    });

    // Returns an SDEAny filled with a Python object
    m.def("make_any", [](pybind11::object a_list) {
        return SDEAnyWrapper{detail_::SDEAny(a_list)};
    });

    PyParameters params;
    params.insert("The number 3",
                  Option{3, "some description",
                         {GreaterThan<int>{0}},
                         {OptionTraits::optional, OptionTraits::transparent}});
    params.insert("Pi", Option{3.1416});
    params.insert("A vector", Option{std::vector<int>{1, 2, 3}});
    params.insert("Hello", Option{std::string{"Hello world"}});

    m.attr("params") = params;

} // End PYBIND11_MODULE
