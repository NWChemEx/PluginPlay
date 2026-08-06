/*
 * Copyright 2022 NWChemEx-Project
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
#include "type_eraser.hpp"
#include <pluginplay/fields/fields.hpp>

namespace pluginplay::cache::database {

/// Specialize MakeAny for ModuleInputs
template<>
struct MakeAny<ModuleInput> {
    template<typename U>
    static any::AnyField convert(U&& v) {
        return v.template value<any::AnyField>();
    }
}; // namespace pluginplay::template<>structMakeAny<ModuleInput>

/// Specialize MakeAny for ModuleResults
template<>
struct MakeAny<ModuleResult> {
    template<typename U>
    static any::AnyField convert(U&& v) {
        using shared_any = typename ModuleResult::shared_any;
        return *v.template value<shared_any>();
    }
};

} // namespace pluginplay::cache::database
