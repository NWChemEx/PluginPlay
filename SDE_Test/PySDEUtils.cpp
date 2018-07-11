#include <SDE/Module.hpp>
#include <SDE/Parameters.hpp>
#include <SDE/PyBindings/PyModule.hpp>
#include <SDE/Pythonization.hpp>
#include <SDE/SDEAny.hpp>

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

// Define and implement a dummy property type
DEFINE_PROPERTY_TYPE(TestProperty, int, int);
struct MyProp : TestProperty {
    MyProp() { submodules_["Prop1"] = nullptr; }
    int run(int x) { return x + 1; }
};

// Declares the Python module py_sde_utils (name must match .so)
PYBIND11_MODULE(py_sde_utils, m) {
    // Exposes our SDEAny wrapper
    pybind11::class_<SDEAnyWrapper>(m, "SDEAnyWrapper")
      .def(pybind11::init<>())
      .def("get", [](SDEAnyWrapper& self) { return self.my_any.pythonize(); })
      .def("change_python", [](SDEAnyWrapper& self, pyobject& obj) {
          self.my_any.change_python(obj);
      });

    // Returns an SDEAny filled with a vector [1, 2, 3]
    m.def("make_any", []() {
        std::vector<int> v1{1, 2, 3};
        return SDEAnyWrapper{detail_::SDEAny(v1)};
    });

    // Registers our dummy module with Python
    SDE::register_property_type<TestProperty>(m, "TestProperty");
    SDE::register_module<MyProp, TestProperty>(m, "MyProp");

    // Function for returning the C++ implementation to Python
    m.def("get_cpp_module", []() {
        auto ptr = std::make_shared<MyProp>();
        return std::static_pointer_cast<SDE::ModuleBase>(ptr);
    });

    m.def("run_py_mod", [](std::shared_ptr<SDE::ModuleBase> mod) {
        return *(mod->run_as<TestProperty>(2)) == 3;
    });

    m.def("get_option", []() {
        return Option{3,
                      "Any positive number",
                      {GreaterThan<int>{0}},
                      {OptionTraits::optional, OptionTraits::transparent}};
    });

    m.def("get_params", []() {
        Option opt1{3, "Positive number", {GreaterThan<int>{-1}}};
        Option opt2{std::string("Hello World")};
        std::map<std::string, Option> opts{{"The number 3", opt1},
                                           {"Hello World", opt2}};
        return std::make_tuple(
          Parameters{"The number 3", opt1, "Hello World", opt2}, opts);

    });

} // End PYBIND11_MODULE
