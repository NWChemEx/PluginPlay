#include "export_pluginplay.hpp"
#include <pybind11/operators.h>

namespace pluginplay {

class PyModuleBase : public ModuleBase {
public:
    using ModuleBase::citation;
    using ModuleBase::description;
    type::result_map run_(type::input_map inputs, type::submodule_map submods) {
        PYBIND11_OVERRIDE_PURE(type::result_map, ModuleBase, run_, inputs,
                               submods);
    }
};

void export_module_base(py_module_reference m) {
    py_module_class<ModuleBase, PyModuleBase>(m, "ModuleBase")
      .def(pybind11::init<>())
      //.def("run", &ModuleBase::run)
      .def("has_description", &ModuleBase::has_description)
      .def("results", &ModuleBase::results)
      .def("inputs", &ModuleBase::inputs)
      //.def("submods", &ModuleBase::submods)
      .def("property_types", &ModuleBase::property_types)
      .def("has_uuid", &ModuleBase::has_uuid)
      .def("uuid", &ModuleBase::uuid)
      .def("set_uuid", &ModuleBase::set_uuid)
      .def("get_desc", &ModuleBase::get_desc)
      .def("citations", &ModuleBase::citations)
      //.def("set_cache", &ModuleBase::set_cache)
      .def("get_cache", &ModuleBase::get_cache)
      .def("reset_internal_cache", &ModuleBase::reset_internal_cache)
      .def(pybind11::self == pybind11::self)
      .def(pybind11::self != pybind11::self)
      .def("description", &PyModuleBase::description)
      .def("citation", &PyModuleBase::citation)
    //.def("add_submodule", &)
    //.def("satisfies_property_type")
    //.def("run_");
}

} // namespace pluginplay
