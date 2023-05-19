/*
 * Copyright 2023 NWChemEx-Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "../property_types.hpp"
#include "test_property_type.hpp"
#include <pluginplay/property_type/property_type.hpp>
#include <pybind11/stl.h>

namespace test_pluginplay {

void test_property_type(pybind11::module_& m) {
    auto m_pt = m.def_submodule("test_property_type");

    m_pt.def("get_types", []() {
        using rtti_type = pluginplay::python::PyTypeInfo;
        std::map<std::string, rtti_type> rv;
        rv.emplace("NullPT", rtti_type(typeid(NullPT)));
        rv.emplace("OneIn", rtti_type(typeid(OneIn)));
        rv.emplace("TwoIn", rtti_type(typeid(TwoIn)));
        rv.emplace("ThreeIn", rtti_type(typeid(ThreeIn)));
        rv.emplace("OptionalInput", rtti_type(typeid(OptionalInput)));
        rv.emplace("PolymorphicOptions", rtti_type(typeid(PolymorphicOptions)));
        rv.emplace("OneOut", rtti_type(typeid(OneOut)));
        rv.emplace("TwoOut", rtti_type(typeid(TwoOut)));
        return rv;
    });

    m_pt.def("one_in_inputs", [](bool set_values) {
        pluginplay::type::input_map inputs;
        inputs["Option 1"].set_type<int>();
        if(set_values) inputs["Option 1"].change(1);
        return inputs;
    });
    m_pt.def("two_in_inputs", [](bool set_values) {
        pluginplay::type::input_map inputs;
        inputs["Option 1"].set_type<int>();
        if(set_values) inputs["Option 1"].change(1);
        inputs["Option 2"].set_type<double>();
        if(set_values) inputs["Option 2"].change(3.14);
        return inputs;
    });
    m_pt.def("three_in_inputs", [](bool set_values) {
        pluginplay::type::input_map inputs;
        inputs["Option 1"].set_type<int>();
        if(set_values) inputs["Option 1"].change(1);
        inputs["Option 2"].set_type<double>();
        if(set_values) inputs["Option 2"].change(3.14);
        inputs["Option 3"].set_type<std::string>();
        if(set_values) inputs["Option 3"].change("Hello World!");
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
