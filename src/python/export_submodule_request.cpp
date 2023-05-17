#include "export_pluginplay.hpp"
#include <pluginplay/python/python.hpp>
#include <pluginplay/submodule_request.hpp>
#include <pybind11/operators.h>

namespace pluginplay {

void py_set_type(SubmoduleRequest& sr, pybind11::object pt) {
    auto py_rtti   = pt.attr("type")();
    auto rtti      = py_rtti.cast<python::PyTypeInfo>().value();
    auto py_inputs = pt.attr("inputs")();
    sr.set_type(rtti, py_inputs.cast<type::input_map>());
}

auto py_sr_run_as(pybind11::object sr, pybind11::object pt,
                  pybind11::args args) {
    auto py_mod = sr.attr("value")();
    return py_mod.attr("run_as")(pt, args);
}

void export_submodule_request(py_module_reference m) {
    using value_fxn = Module& (SubmoduleRequest::*)();

    py_class_type<SubmoduleRequest>(m, "SubmoduleRequest")
      .def(pybind11::init<>())
      .def("has_type", &SubmoduleRequest::has_type)
      .def("has_module", &SubmoduleRequest::has_module)
      .def("has_description", &SubmoduleRequest::has_description)
      .def("ready", &SubmoduleRequest::ready)
      .def("set_type", &py_set_type)
      .def("set_description", &SubmoduleRequest::set_description)
      .def("submod_uuids", &SubmoduleRequest::submod_uuids)
      .def("uuid", &SubmoduleRequest::uuid)
      .def("value", static_cast<value_fxn>(&SubmoduleRequest::value))
      .def("description", &SubmoduleRequest::description)
      .def("lock", &SubmoduleRequest::lock)
      .def("run_as", &py_sr_run_as)
      .def(pybind11::self == pybind11::self)
      .def(pybind11::self != pybind11::self);
}

} // namespace pluginplay
