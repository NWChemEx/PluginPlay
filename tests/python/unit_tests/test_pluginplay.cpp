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

#include "any/test_any.hpp"
#include "fields/test_fields.hpp"
#include "modules.hpp"
#include "property_type/test_property_type.hpp"
#include "property_types.hpp"
#include "python/test_python.hpp"
#include "test_pluginplay.hpp"
#include <pluginplay/module_manager.hpp>
#include <pybind11/operators.h>
#include <pybind11/pybind11.h>

namespace test_pluginplay {

PYBIND11_MODULE(py_test_pluginplay, m) {
    // -------------------------------------------------------------------------
    // -- Export property types (and related classes) used throughout the tests
    // -------------------------------------------------------------------------
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
    EXPORT_PROPERTY_TYPE(OneInOneOut, m);

    m.def("get_mm", []() {
        pluginplay::ModuleManager mm(nullptr);
        mm.add_module<NoPTModule>("C++ no PT");
        mm.add_module<NullModule>("C++ Null PT");
        mm.add_module<DescModule>("C++ with description");
        mm.add_module<CiteModule>("C++ with a citation");
        mm.add_module<PolymorphicModule>("C++ with a polymorphic option");
        mm.add_module<NotReadyModule>("C++ module which isn't ready");
        mm.add_module<ReadyModule>("C++ module which is ready to run");
        mm.add_module<NotReadyModule2>("C++ module which also isn't ready");
        mm.add_module<SubModModule>("C++ module needing a submodule");
        mm.add_module<RealDeal>("C++ module using every feature");
        return mm;
    });

    test_any(m);
    test_fields(m);
    test_module(m);
    test_module_base(m);
    test_property_type(m);
    test_python(m);
    test_submodule_request(m);
}

} // namespace test_pluginplay
