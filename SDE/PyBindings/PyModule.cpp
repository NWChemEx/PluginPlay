#include "SDE/Memoization.hpp"
#include "SDE/Module.hpp"
#include "SDE/PyBindings/PySmartEnums.hpp"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

using module_pointer = typename SDE::ModuleBase::module_pointer;

namespace SDE {
namespace detail_ {

struct PyModuleBase {
    void set_submodule(module_pointer me, std::string key, module_pointer ptr) {
        me->submodules_[key] = ptr;
    }

    void set_metadata(module_pointer me, const MetaProperty& key,
                      std::string value) {
        me->metadata_[key] = value;
    }

    void set_option(module_pointer me, const std::string& key,
                    const Option& opt) {
        me->parameters_.insert(key, opt);
    }

    void change_parameter(module_pointer me, std::string key, pyobject obj) {
        if(me->locked()) throw std::runtime_error("Module is locked!!!");
        pyobject params = pycast(me->parameters_);
        params.attr("change")(key, obj);
        me->parameters_ = params.cast<Parameters>();
    }
};

} // namespace detail_

void pythonize_Module(pybind11::module& m) {
    DECLARE_PySmartEnum(Resource, time, memory, disk, processes, threads);
    DECLARE_PySmartEnum(MetaProperty, name, version, description, authors,
                        citations);
    DECLARE_PySmartEnum(ModuleTraits, nondeterministic);
    DECLARE_PySmartEnum(ModuleProperty, metadata, parameters, submodules,
                        traits);

    pybind11::class_<ModuleBase, module_pointer>(m, "ModuleBase")
      .def(pybind11::init<>())
      .def("submodules", &ModuleBase::submodules)
      .def("metadata", &ModuleBase::metadata)
      .def("parameters", &ModuleBase::parameters)
      .def("change_submodule", &ModuleBase::change_submodule)
      .def("change_parameter",
           [](module_pointer me, const std::string& key, pyobject obj) {
               detail_::PyModuleBase().change_parameter(me, key, obj);
           })
      .def("locked", &ModuleBase::locked)
      .def("lock", &ModuleBase::lock)
      .def("not_ready", &ModuleBase::not_ready)
      .def("run_as",
           [](module_pointer me, pybind11::object obj, pybind11::args args) {
               auto fxn = obj.attr("_run_as_impl");
               return fxn(me, *args);
           })
      .def("__eq__",
           [](module_pointer lhs, module_pointer rhs) { return lhs == rhs; })
      .def("_set_submodule",
           [](module_pointer me, std::string key, module_pointer ptr) {
               detail_::PyModuleBase().set_submodule(me, key, ptr);
           })
      .def("_set_submodule",
           [](module_pointer me, std::string key, pybind11::none a_none) {
               detail_::PyModuleBase().set_submodule(me, key, module_pointer{});
           })
      .def("_set_option",
           [](module_pointer me, const std::string& key, const Option& opt) {
               detail_::PyModuleBase().set_option(me, key, opt);
           })
      .def("_set_metadata",
           [](module_pointer me, const MetaProperty& key, std::string value) {
               detail_::PyModuleBase().set_metadata(me, key, value);
           });
}
} // namespace SDE
