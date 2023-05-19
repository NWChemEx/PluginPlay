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

#pragma once
#include <pluginplay/property_type/property_type.hpp>
#include <string>

namespace testing {

struct BaseClass {
    int x = 0;
    bool operator==(const BaseClass& rhs) const noexcept { return x == rhs.x; }
    bool operator<(const BaseClass& rhs) const noexcept { return x < rhs.x; }
};

struct DerivedClass : BaseClass {};

// Simplest property type possible
DECLARE_PROPERTY_TYPE(NullPT);
PROPERTY_TYPE_INPUTS(NullPT) { return pluginplay::declare_input(); }
PROPERTY_TYPE_RESULTS(NullPT) { return pluginplay::declare_result(); }

// Property type for module with one option
DECLARE_PROPERTY_TYPE(OneIn);
PROPERTY_TYPE_INPUTS(OneIn) {
    return pluginplay::declare_input().add_field<int>("Option 1");
}
PROPERTY_TYPE_RESULTS(OneIn) { return pluginplay::declare_result(); }

// Derived property type which adds another input to OneIn
DECLARE_DERIVED_PROPERTY_TYPE(TwoIn, OneIn);
PROPERTY_TYPE_INPUTS(TwoIn) {
    return pluginplay::declare_input().add_field<double>("Option 2");
}
PROPERTY_TYPE_RESULTS(TwoIn) { return pluginplay::declare_result(); }

// Derived property type which adds another input to TwoIn
DECLARE_DERIVED_PROPERTY_TYPE(ThreeIn, TwoIn);
PROPERTY_TYPE_INPUTS(ThreeIn) {
    return pluginplay::declare_input().add_field<std::string>("Option 3");
}
PROPERTY_TYPE_RESULTS(ThreeIn) { return pluginplay::declare_result(); }

// Property type for module with a defaulted option
struct OptionalInput : pluginplay::PropertyType<OptionalInput> {
    auto inputs_() {
        return pluginplay::declare_input().add_field<int>("Option 1", 1);
    }
    auto results_() {
        return pluginplay::declare_result().add_field<int>("Result 1");
    }
};

// Property type that takes a polymorphic option
DECLARE_PROPERTY_TYPE(PolymorphicOptions);
PROPERTY_TYPE_INPUTS(PolymorphicOptions) {
    return pluginplay::declare_input().add_field<const BaseClass&>("base");
}
PROPERTY_TYPE_RESULTS(PolymorphicOptions) {
    return pluginplay::declare_result();
}

// Property type for module with one result
DECLARE_PROPERTY_TYPE(OneOut);
PROPERTY_TYPE_INPUTS(OneOut) { return pluginplay::declare_input(); }
PROPERTY_TYPE_RESULTS(OneOut) {
    return pluginplay::declare_result().add_field<int>("Result 1");
}

// Property type for module with two results
DECLARE_PROPERTY_TYPE(TwoOut);
PROPERTY_TYPE_INPUTS(TwoOut) { return pluginplay::declare_input(); }
PROPERTY_TYPE_RESULTS(TwoOut) {
    return pluginplay::declare_result()
      .add_field<int>("Result 1")
      .add_field<char>("Result 2");
}
} // namespace testing
