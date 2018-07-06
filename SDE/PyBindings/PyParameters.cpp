#include "SDE/Memoization.hpp"
#include "SDE/PyBindings/PyParameters.hpp"
#include "SDE/PyBindings/PySmartEnums.hpp"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace SDE {

void pythonize_Parameters(pybind11::module& m) {
    DECLARE_PySmartEnum(OptionTraits, transparent, optional, non_default);

    pybind11::class_<PyParameters>(m, "Parameters")
      .def(pybind11::init<>())
      .def("change", &PyParameters::change_python)
      .def("at", &PyParameters::at_python)
      .def("__contains__", &PyParameters::count)
      .def("get_description", &PyParameters::get_description)
      .def("get_traits", &PyParameters::get_traits)
      .def("track_changes", &PyParameters::track_changes);
}

} // namespace SDE