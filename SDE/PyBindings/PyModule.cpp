#include "SDE/Memoization.hpp"
#include "SDE/Module.hpp"
#include "SDE/PyBindings/PyModule.hpp"
namespace py = pybind11;

namespace SDE {
namespace detail_ {

// Trampoline class for ModuleBase
class PyModuleBase : public ModuleBase {
public:
    const std::type_info& type() const noexcept override {
        return typeid(py::object);
    }
};

} // namespace detail_
} // namespace SDE

void pythonize_Module(py::module& m) {
    py::class_<SDE::ModuleBase, SDE::detail_::PyModuleBase>(m, "ModuleBase")
      .def(py::init<>());
    pythonize_module_type<py::object, int>(m, "ModuleBaseImpl");
}
