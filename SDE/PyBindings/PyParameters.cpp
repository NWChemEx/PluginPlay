#include "SDE/Memoization.hpp"
#include "SDE/Parameters.hpp"
#include "SDE/PyBindings/PySmartEnums.hpp"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace SDE {

void pythonize_Parameters(pybind11::module& m) {
    DECLARE_PySmartEnum(OptionTraits, transparent, optional, non_default);

    pybind11::class_<Parameters>(m, "Parameters")
      .def(pybind11::init<>())
      .def("change",
           [](Parameters& me, std::string key, pyobject new_val) {
               auto opt = me.at<Option>(key);
               opt.value.insert_python(new_val);
               if(!opt.is_valid())
                   throw std::invalid_argument("Not a valid option value");

               if(me.tracking_changes)
                   opt.traits.insert(OptionTraits::non_default);
               me.insert(key, opt);
           })
      .def("at",
           [](const Parameters& me, std::string key) {
               return me.at<Option>(key).value.pythonize();
           })
      .def("__contains__", &Parameters::count)
      .def("get_description", &Parameters::get_description)
      .def("get_traits", &Parameters::get_traits)
      .def("track_changes", &Parameters::track_changes);
}

} // namespace SDE