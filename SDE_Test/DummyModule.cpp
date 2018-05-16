#include <SDE/Module.hpp>
#include <SDE/PyBindings/PyModule.hpp>
using namespace SDE;

struct DummyModuleCPP;

// Defines the DummyModule module type
using DummyModuleAPI = ModuleBaseImpl<DummyModuleCPP, bool, int>;

// The C++ instantiation of a DummyModule
struct DummyModuleCPP : DummyModuleAPI {
protected:
    bool run_(int i) override { return i == 1; }
};

PYBIND11_MODULE(DummyModule, m) {
    // Makes DummyModuleAPI visible to Python for implementation
    pythonize_module_type<DummyModuleAPI, bool, int>(m, "DummyModuleAPI");
    pythonize_property<DummyModuleAPI, int>(m, "DummyModuleProp");

    m.def("get_module", []() { return std::make_unique<DummyModuleCPP>(); });
}
