#include <SDE/Module.hpp>
#include <SDE/PyBindings/PyModule.hpp>
using namespace SDE;

// Declare some Module types
// DEFINE_PROPERTY(TestProperty1, void, );
DEFINE_MODULE_TYPE(TestProperty2, int, int);

// Implement those types
// struct MyProp1 : TestProperty1API {void run(){}};
struct MyProp2 : TestProperty2 {
    int run(int x) { return x + 1; }
};

PYBIND11_MODULE(DummyModule, m) { DEFINE_PYTHON_PROPERTY(m, TestProperty2); }
