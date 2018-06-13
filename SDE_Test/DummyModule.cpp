#include <SDE/Module.hpp>
#include <SDE/PyBindings/PyModule.hpp>
using namespace SDE;

// Declare some Module types
// DEFINE_PROPERTY(TestProperty1, void, );
DEFINE_PROPERTY_TYPE(TestProperty2, int, int);

// Implement those types
// struct MyProp1 : TestProperty1API {void run(){}};
struct MyProp2 : TestProperty2 {
    int run(int x) { return x + 1; }
};

PYBIND11_MODULE(DummyModule, m) {
    SDE::register_property_type<TestProperty2>(m, "TestProperty2");
    SDE::register_module<MyProp2, TestProperty2>(m, "MyProp2");
    m.def("get_cpp_module", []() {
        auto ptr = std::make_shared<MyProp2>();
        return std::static_pointer_cast<SDE::ModuleBase>(ptr);
    });
}
