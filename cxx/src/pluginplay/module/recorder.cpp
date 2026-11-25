/*
 * Copyright 2026 NWChemEx-Project
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

#include <pluginplay/module/recorder.hpp>
#include <utility>

namespace pluginplay {
namespace {

const auto desc = R"(
Recorder
--------

Runs the "Wrapped" submodule, records the results it computes, and returns
them. The inputs are forwarded to "Wrapped" as they are.
)";

} // namespace

Recorder::Recorder(rtti_type rtti, input_map inputs, result_map results) :
  ModuleBase(this) {
    // The property type is chosen at runtime, so Recorders are not
    // interchangeable
    not_shareable();
    description(desc);
    satisfies_property_type(rtti, inputs, std::move(results));
    submods()["Wrapped"]
      .set_type(std::move(rtti), std::move(inputs))
      .set_description("The module whose results are recorded");
}

Recorder::result_map Recorder::run_(input_map inputs,
                                    submodule_map submods) const {
    auto rv = submods.at("Wrapped").value().run(std::move(inputs));
    m_records_.push_back(rv);
    return rv;
}

} // namespace pluginplay
