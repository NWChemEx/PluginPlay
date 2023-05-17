#include "export_pluginplay.hpp"
#include <pluginplay/module.hpp>
#include <pluginplay/submodule_request.hpp>
#include <pybind11/operators.h>

namespace pluginplay {

pybind11::object py_module_run_as(pybind11::object self, pybind11::object pt,
                                  pybind11::args args) {
    auto inps                = pt.attr("unwrap_inputs")(args);
    auto rvs                 = self.attr("run")(inps);
    pybind11::tuple rv_tuple = pt.attr("unwrap_results")(rvs);
    return rv_tuple.size() != 1 ? rv_tuple : rv_tuple[0];
}

void export_module(py_module_reference m) {
    using ready_fxn         = bool (Module::*)(const type::input_map&) const;
    using change_submod_fxn = void (Module::*)(type::key, Module);

    py_class_type<Module>(m, "Module")
      .def(pybind11::init<>())
      .def("unlocked_copy", &Module::unlocked_copy)
      .def("has_module", &Module::has_module)
      .def("has_description", &Module::has_description)
      .def("locked", &Module::locked)
      .def("list_not_ready", &Module::list_not_ready,
           pybind11::arg("in_inputs") = type::input_map{})
      .def("ready", static_cast<ready_fxn>(&Module::ready))
      .def("reset_cache", &Module::reset_cache)
      .def("reset_internal_cache", &Module::reset_internal_cache)
      .def("is_memoizable", &Module::is_memoizable)
      .def("turn_off_memoization", &Module::turn_off_memoization)
      .def("turn_on_memoization", &Module::turn_on_memoization)
      .def("lock", &Module::lock)
      .def("results", &Module::results)
      .def("inputs", &Module::inputs)
      .def("submods", &Module::submods)
      .def("property_types", &Module::property_types)
      .def("description", &Module::description)
      .def("citations", &Module::citations)
      .def("change_input",
           [](Module& m, std::string key, pybind11::object o) {
               m.change_input(key,
                              any::make_any_field<python::PythonWrapper>(o));
           })
      .def("change_submod",
           static_cast<change_submod_fxn>(&Module::change_submod))
      .def("run_as", &py_module_run_as)
      .def("run", &Module::run)
      .def("profile_info", &Module::profile_info)
      .def("submod_uuids", &Module::submod_uuids)
      .def("uuid", &Module::uuid)
      .def(pybind11::self == pybind11::self)
      .def(pybind11::self != pybind11::self);

    m.def("print_not_ready", &print_not_ready, pybind11::arg("mod"),
          pybind11::arg("ps")     = type::input_map{},
          pybind11::arg("indent") = "");
}

} // namespace pluginplay
