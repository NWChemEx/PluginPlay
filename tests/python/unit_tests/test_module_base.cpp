#include "test_pluginplay.hpp"

namespace test_pluginplay {

void test_module_base(pybind11::module_& m) {
    auto m_test = m.def_submodule("test_module_base");
}

} // namespace test_pluginplay
