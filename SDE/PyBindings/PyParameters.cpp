#include "SDE/PyBindings/PySmartEnums.hpp"
#include "SDE/attic/Parameters.hpp"
#include "SDE/detail_/Memoization.hpp"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace SDE {
namespace detail_ {
struct PyOption {
    pyobject py_get(Option& me) { return me.value_.pythonize(); }

    bool py_valid(Option& me, pyobject value) {
        detail_::SDEAny da_any(me.value_);
        da_any.change_python(value);
        for(auto ci : me.checks_)
            if(!ci(da_any)) return false;
        return true;
    }

    void py_change(Option& me, pyobject value) {
        if(!py_valid(me, value))
            throw std::invalid_argument("Not a valid option value");
        me.value_.change_python(value);
    }
};

struct PyParameters {
    void py_change(Parameters& me, std::string key, pyobject new_val) {
        // Grab option by copy (error checks)
        auto opt = me.at<Option>(key);
        PyOption().py_change(opt, new_val);
        me.insert(key, opt);
        if(me.tracking_changes_)
            me.options_[key].traits.insert(OptionTraits::non_default);
    }
};
} // namespace detail_

void pythonize_Parameters(pybind11::module& m) {
    DECLARE_PySmartEnum(OptionTraits, transparent, optional, non_default);

    pybind11::class_<Option>(m, "Option")
      .def("get", [](Option& me) { return detail_::PyOption().py_get(me); })
      .def("is_valid",
           [](Option& me, pyobject value) {
               return detail_::PyOption().py_valid(me, value);
           })
      .def("change",
           [](Option& me, pyobject value) {
               detail_::PyOption().py_change(me, value);
           })
      .def("__eq__", &Option::operator==)
      .def("__ne__", &Option::operator!=)
      .def_readwrite("description", &Option::description)
      .def_readwrite("traits", &Option::traits);

    pybind11::class_<Parameters>(m, "Parameters")
      .def("change",
           [](Parameters& me, std::string key, pyobject new_val) {
               detail_::PyParameters().py_change(me, key, new_val);
           })
      .def("get_option",
           [](Parameters& me, std::string& key) {
               return pycast(me.at<Option>(key));
           })
      .def("get_value",
           [](Parameters& me, std::string& key) {
               return pycast(me).attr("get_option")(key).attr("get")();
           })
      .def("__contains__", &Parameters::count)
      .def("__len__", &Parameters::size)
      .def("__eq__", &Parameters::operator==)
      .def("__ne__", &Parameters::operator!=)
      .def("get_description", &Parameters::get_description)
      .def("get_traits", &Parameters::get_traits)
      .def("track_changes", &Parameters::track_changes);
}

} // namespace SDE
