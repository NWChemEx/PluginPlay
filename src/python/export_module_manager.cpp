#include "export_pluginplay.hpp"
#include <pluginplay/module_manager.hpp>

namespace pluginplay {

void export_module_manager(py_module_reference m) {
    using at_fxn = Module& (ModuleManager::*)(const type::key&);

    py_class_type<ModuleManager>(m, "ModuleManager")
      .def(pybind11::init<>())
      .def("size", &ModuleManager::size)
      //.def("add_module")
      .def("at", static_cast<at_fxn>(&ModuleManager::at))
      .def("copy_module", &ModuleManager::copy_module)
      .def("erase", &ModuleManager::erase)
      .def("rename_module", &ModuleManager::rename_module);
    //.def("change_input", )
    //.def("change_submod",)
    //.def("run_as")
    //.def("set_runtime", &ModuleManager::set_runtime)
    //.def("get_runtime", &ModuleManager::get_runtime)
}

} // namespace pluginplay
