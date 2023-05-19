#include "export_pluginplay.hpp"
#include "py_module_base.hpp"
#include <pluginplay/any/any.hpp>
#include <pluginplay/module_manager.hpp>
#include <pluginplay/python/python_wrapper.hpp>

namespace pluginplay {

void export_module_manager(py_module_reference m) {
    using module_base_ptr = ModuleManager::module_base_ptr;
    using at_fxn          = Module& (ModuleManager::*)(const type::key&);
    using add_mod_fxn     = void (ModuleManager::*)(type::key, module_base_ptr);

    using py_obj = pybind11::object;
    using python::PythonWrapper;

    py_class_type<ModuleManager>(m, "ModuleManager")
      .def(pybind11::init<>())
      .def("count", &ModuleManager::count)
      .def("size", &ModuleManager::size)
      .def("add_module",
           [](ModuleManager& mm, std::string key,
              std::shared_ptr<PyModuleBase> p) { mm.add_module(key, p); })
      .def("at", static_cast<at_fxn>(&ModuleManager::at))
      .def("copy_module", &ModuleManager::copy_module)
      .def("erase", &ModuleManager::erase)
      .def("rename_module", &ModuleManager::rename_module)
      .def("change_input",
           [](ModuleManager& self, std::string mod_key, std::string opt_key,
              py_obj in) {
               self.change_input(mod_key, opt_key,
                                 any::make_any_field<PythonWrapper>(in));
           })
      .def("change_submod", &ModuleManager::change_submod)
      .def("run_as",
           [](py_obj self, py_obj pt, py_obj key, pybind11::args args) {
               return self.attr("at")(key).attr("run_as")(pt, *args);
           });
    //.def("set_runtime", &ModuleManager::set_runtime)
    //.def("get_runtime", &ModuleManager::get_runtime)

    m.def("defaulted_mm", []() { return ModuleManager(nullptr); });
}

} // namespace pluginplay
