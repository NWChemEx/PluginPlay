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
      .def("change", &Parameters::change_python)
      .def("at", &Parameters::at_python)
      .def("__contains__", &Parameters::count)
      .def("get_description", &Parameters::get_description)
      .def("get_traits", &Parameters::get_traits)
      .def("track_changes", &Parameters::track_changes);
}

} // namespace SDE