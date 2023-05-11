#include "test_any.hpp"
#include <pluginplay/any/any.hpp>

namespace test_pluginplay {

void test_any_field(pybind11::module_& m) {
    auto m_test_any = m.def_submodule("test_any_field");
    m_test_any.def("get_vector", []() {
        std::vector<int> v{1, 2, 3};
        return pluginplay::any::make_any_field<std::vector<int>>(std::move(v));
    });
}

} // namespace test_pluginplay
