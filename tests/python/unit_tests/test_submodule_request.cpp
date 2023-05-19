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

#include "modules.hpp"
#include "property_types.hpp"
#include "test_pluginplay.hpp"
#include <pluginplay/submodule_request.hpp>

namespace test_pluginplay {

void test_submodule_request(pybind11::module_& m) {
    auto m_test = m.def_submodule("test_submodule_request");

    m_test.def("get_sr_w_not_ready_mod", []() {
        pluginplay::SubmoduleRequest sr;
        sr.set_type<OneIn>();
        sr.change(make_module<NotReadyModule2>());
        return sr;
    });

    m_test.def("get_sr_w_ready_mod", []() {
        pluginplay::SubmoduleRequest sr;
        sr.set_type<OptionalInput>();
        sr.change(make_module<ReadyModule>());
        return sr;
    });
}

} // namespace test_pluginplay
