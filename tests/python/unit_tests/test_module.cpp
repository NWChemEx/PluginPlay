#include "test_pluginplay.hpp"

namespace test_pluginplay {

void test_module(pybind11::module_& m) {
    auto m_mod = m.def_submodule("test_module");
}

} // namespace test_pluginplay
