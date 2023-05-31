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

// This file meant only for inclusion from module_result.hpp
#include <pluginplay/fields/bounds_checking/type_check.hpp>

namespace pluginplay {

//------------------------------Implementations---------------------------------

inline ModuleResult& ModuleResult::operator=(const ModuleResult& rhs) {
    return *this = std::move(ModuleResult(rhs));
}

template<typename T>
auto& ModuleResult::set_type() {
    constexpr bool is_clean = std::is_same_v<std::decay_t<T>, T>;
    static_assert(is_clean, "Results must be unqualified types.");
    bounds_checking::TypeCheck<T> check;
    auto l = [=](const type::any& value) { return check(value); };
    set_type_check_(std::move(l));
    return set_type_(typeid(T));
}

template<typename T>
void ModuleResult::change(T&& new_value) {
    using clean_T = std::decay_t<T>;
    constexpr bool is_shared_any =
      std::is_same_v<clean_T, shared_any> || // is shared_ptr<const any>
      std::is_same_v<clean_T, std::shared_ptr<type::any>>; // no const
    if constexpr(is_shared_any)
        change_(new_value);
    else if constexpr(detail_::IsCString<T>::value)
        change_(std::move(wrap_value_(std::string(new_value))));
    else
        change_(std::move(wrap_value_(std::forward<T>(new_value))));
}

template<typename T>
T ModuleResult::value() const {
    using clean_T = std::decay_t<T>;
    if constexpr(std::is_same_v<shared_any, clean_T>)
        return at_();
    else if constexpr(detail_::IsSharedPtr<clean_T>::value) {
        using type = typename clean_T::element_type;
        return T(at_(), &value<type&>());
    } else
        return any::any_cast<T>(*at_());
}

template<typename T>
type::any ModuleResult::wrap_value_(T&& new_value) {
    using clean_T = std::decay_t<T>;
    // Is it already an AnyField
    if constexpr(std::is_same_v<clean_T, type::any>) {
        return std::forward<T>(new_value);
    }
    return any::make_any_field<clean_T>(std::forward<T>(new_value));
}

} // namespace pluginplay
