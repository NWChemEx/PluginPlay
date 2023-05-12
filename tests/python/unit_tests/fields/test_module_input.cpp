#include "test_fields.hpp"
#include <pluginplay/fields/module_input.hpp>

namespace test_pluginplay {
using namespace pluginplay;

void test_module_input(pybind11::module_& m) {
    auto m_test = m.def_submodule("test_module_input");

    // This makes a ModuleInput which expects an integer
    m_test.def("get_i_float", []() {
        ModuleInput i;
        i.set_type<double>();
        return i;
    });

    // This makes a ModuleInput which expects a std::vector<int>
    m_test.def("get_i_list", []() {
        ModuleInput i;
        i.set_type<std::vector<int>>();
        return i;
    });

    // This makes a ModuleInput which expects a std::vector<int> default
    // initialized to {1, 2, 3}
    m_test.def("get_filled_i_list", []() {
        ModuleInput i;
        i.set_type<std::vector<int>>();
        i.change(std::vector<int>{1, 2, 3});
        return i;
    });

    // This will check that i contains a list {1, 2, 3}
    m_test.def("unwrap_python_list", [](ModuleInput& i) {
        const auto& value = i.value<std::vector<int>>();
        return value == std::vector<int>{1, 2, 3};
    });
}

} // namespace test_pluginplay
