#include "test_python.hpp"
#include <pluginplay/python/py_type_info.hpp>

namespace test_pluginplay {

using PyTypeInfo = pluginplay::python::PyTypeInfo;

void test_py_type_info(pybind11::module_& m) {
    auto m_test = m.def_submodule("test_py_test_info");

    m_test.def("wraps_int", []() { return PyTypeInfo(typeid(int)); });
    m_test.def("wraps_double",
               []() { return PyTypeInfo(std::type_index(typeid(double))); });

    m_test.def("check_int", [](PyTypeInfo& pinfo) {
        PyTypeInfo corr(std::type_index(typeid(int)));
        return corr == pinfo;
    });

    m_test.def("check_double", [](PyTypeInfo& pinfo) {
        PyTypeInfo corr(std::type_index(typeid(double)));
        return corr == pinfo;
    });
}

} // namespace test_pluginplay
