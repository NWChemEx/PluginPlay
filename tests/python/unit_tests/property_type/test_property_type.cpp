#include "../../../cxx/unit_tests/pluginplay/unit_testing_pts.hpp"
#include "test_property_type.hpp"
#include <pluginplay/property_type/property_type.hpp>
#include <pybind11/operators.h>

namespace test_pluginplay {

void test_property_type(pybind11::module_& m) {
    using namespace testing;

    pybind11::class_<BaseClass>(m, "BaseClass")
      .def(pybind11::init<>())
      .def("get_x", [](BaseClass& b) { return b.x; })
      .def("set_x", [](BaseClass& b, int new_x) { b.x = new_x; })
      .def(pybind11::self == pybind11::self);

    pybind11::class_<DerivedClass, BaseClass>(m, "DerivedClass");

    EXPORT_PROPERTY_TYPE(NullPT, m);
    EXPORT_PROPERTY_TYPE(OneIn, m);
    EXPORT_PROPERTY_TYPE(TwoIn, m);
    EXPORT_PROPERTY_TYPE(ThreeIn, m);
    EXPORT_PROPERTY_TYPE(OptionalInput, m);
    EXPORT_PROPERTY_TYPE(PolymorphicOptions, m);
    EXPORT_PROPERTY_TYPE(OneOut, m);
    EXPORT_PROPERTY_TYPE(TwoOut, m);

    auto m_pt = m.def_submodule("test_property_type");

    m_pt.def("one_in_inputs", []() {
        pluginplay::type::input_map inputs;
        inputs["Option 1"].set_type<int>();
        inputs["Option 1"].change(1);
        return inputs;
    });
    m_pt.def("two_in_inputs", []() {
        pluginplay::type::input_map inputs;
        inputs["Option 1"].set_type<int>();
        inputs["Option 1"].change(1);
        inputs["Option 2"].set_type<double>();
        inputs["Option 2"].change(3.14);
        return inputs;
    });
    m_pt.def("three_in_inputs", []() {
        pluginplay::type::input_map inputs;
        inputs["Option 1"].set_type<int>();
        inputs["Option 1"].change(1);
        inputs["Option 2"].set_type<double>();
        inputs["Option 2"].change(3.14);
        inputs["Option 3"].set_type<std::string>();
        inputs["Option 3"].change("Hello World!");
        return inputs;
    });
    m_pt.def("polymorph_input", []() {
        pluginplay::type::input_map inputs;
        inputs["base"].set_type<const BaseClass&>();
        inputs["base"].change(DerivedClass());
        return inputs;
    });

    m_pt.def("result_map_one", []() {
        pluginplay::type::result_map results;
        results["Result 1"].set_type<int>();
        return results;
    });

    m_pt.def("result_map_two", []() {
        pluginplay::type::result_map results;
        results["Result 1"].set_type<int>();
        results["Result 2"].set_type<char>();
        return results;
    });
}

} // namespace test_pluginplay
