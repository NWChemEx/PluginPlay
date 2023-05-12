#include "test_fields.hpp"
#include <pluginplay/fields/module_result.hpp>

namespace test_pluginplay {

void test_module_result(pybind11::module_& m) {
    auto m_test = m.def_submodule("test_module_results");
}

} // namespace test_pluginplay
