#include "export_pluginplay.hpp"
#include "py_module_base.hpp"

namespace pluginplay {

class Publicist : public ModuleBase {
public:
    using ModuleBase::citation;
    using ModuleBase::description;
    using ModuleBase::run_;
};

void export_module_base(py_module_reference m) {
    using submod_fxn  = type::submodule_map& (ModuleBase::*)();
    using inputs_fxn  = type::input_map& (ModuleBase::*)();
    using results_fxn = type::result_map& (ModuleBase::*)();

    using pointer_type = std::shared_ptr<ModuleBase>;
    py_class_type<ModuleBase, PyModuleBase, pointer_type>(m, "ModuleBase")
      .def(pybind11::init<>())
      .def("results", static_cast<results_fxn>(&ModuleBase::results))
      .def("inputs", static_cast<inputs_fxn>(&ModuleBase::inputs))
      .def("submods", static_cast<submod_fxn>(&ModuleBase::submods))
      .def("property_types", &ModuleBase::property_types)
      .def("get_desc", &ModuleBase::get_desc)
      .def("citations", &ModuleBase::citations)
      //   //.def("set_cache", &ModuleBase::set_cache)
      //   .def("get_cache", &ModuleBase::get_cache)
      //   .def("reset_internal_cache", &ModuleBase::reset_internal_cache)
      .def("description", &Publicist::description)
      .def("citation", &Publicist::citation)
      .def("add_input",
           [](ModuleBase& self, std::string key) {
               auto& new_i = self.inputs()[key];
               new_i.set_type<python::PythonWrapper>();
               return new_i;
           })
      .def("add_result",
           [](ModuleBase& self, std::string key) {
               auto& new_r = self.results()[key];
               new_r.set_type<python::PythonWrapper>();
               return new_r;
           })
      .def("add_submodule",
           [](ModuleBase& self, pybind11::object pt, std::string key) {
               auto py_sr = pybind11::cast(SubmoduleRequest());
               py_sr.attr("set_type")(pt);
               auto sr     = py_sr.cast<SubmoduleRequest>();
               auto& smods = self.submods();
               smods.emplace(key, std::move(sr));
               return smods.at(key);
           })
      .def("satisfies_property_type",
           [](ModuleBase& self, pybind11::object pt) {
               auto info    = pt.attr("type")().cast<python::PyTypeInfo>();
               auto inputs  = pt.attr("inputs")().cast<type::input_map>();
               auto results = pt.attr("results")().cast<type::result_map>();
               self.satisfies_property_type(info.value(), std::move(inputs),
                                            std::move(results));
           })
      .def("run_", &Publicist::run_);
}

} // namespace pluginplay
