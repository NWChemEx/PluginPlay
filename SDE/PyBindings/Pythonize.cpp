#include "SDE/PyBindings/PyModule.hpp"
#include <pybind11/pybind11.h>
namespace py = pybind11;

// extern void pythonize_Module(py::module&);
namespace Test {
// Declare some Module types
// DEFINE_PROPERTY(TestProperty1, void, );
DEFINE_PROPERTY(TestProperty2, int, int);

// Implement those types
// struct MyProp1 : TestProperty1API {void run(){}};
struct MyProp2 : TestProperty2API {
    int run(int x) { return x + 1; }
};

PYBIND11_MODULE(SDE, m) {
    // DEFINE_PYTHON_PROPERTY(m, Test::TestProperty1API, Test::TestProperty1);
    DEFINE_PYTHON_PROPERTY(m, TestProperty2API, TestProperty2);
}
} // namespace Test
