#pragma once
#include <Utilities/SmartEnum.hpp>
#include <pybind11/pybind11.h>

#define _ADD_PYSMARTENUM(name, x) \
    .def_property_readonly_static(#x, [](pybind11::object) { return name::x; })

#define DECLARE_PySmartEnum(name, ...)                                   \
    pybind11::class_<name>(m, #name)                                     \
      .def("__eq__",                                                     \
           [](const name& lhs, const name& rhs) {                        \
               return std::string(lhs) == std::string(rhs);              \
           })                                                            \
      .def("__ne__",                                                     \
           [](const name& lhs, const name& rhs) {                        \
               return std::string(lhs) != std::string(rhs);              \
           })                                                            \
      .def("__repr__", [](const name& lhs) { return std::string(lhs); }) \
      .def("__hash__", [](const name& lhs) {                             \
          return pybind11::hash(pybind11::str(std::string(lhs)));        \
      }) CALL_MACRO_X_FOR_EACH1(_ADD_PYSMARTENUM, name, __VA_ARGS__)
