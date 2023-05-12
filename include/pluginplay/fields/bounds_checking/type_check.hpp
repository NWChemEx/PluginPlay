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
#include <pluginplay/types.hpp>
#include <string>
#include <utilities/printing/demangler.hpp>

namespace pluginplay::bounds_checking {

template<typename T>
class TypeCheck {
public:
    template<typename U>
    bool operator()(U&& value) const {
        constexpr bool clean_type = std::decay_t<U>;
        constexpr bool is_any     = std::is_same_v<clean_type, type::any>;
        if constexpr(is_any) {
            return value.template is_convertible<T>() ||
                   value.template is_convertible<const T&>();
        } else {
            return std::is_convertible_v<U, T>();
        }
    }

    std::string str() const {
        const std::string p0("Type == ");
        const auto p1 = utilities::printing::Demangler::demangle(typeid(T));
        return p0 + p1;
    }
};

} // namespace pluginplay::bounds_checking
