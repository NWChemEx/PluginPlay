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
#include "pluginplay/property_type/detail_/static_assert_convertible_verbose.hpp"

namespace pluginplay {
// ----------------------------- Implementations -------------------------------
#define PROP_TYPE PropertyType<DerivedType, BaseType>

template<typename DerivedType, typename BaseType>
auto PROP_TYPE::inputs() {
    DerivedType parent;
    if constexpr(am_base_pt) {
        return parent.inputs_();
    } else {
        return parent.inputs_() + BaseType::inputs();
    }
}

template<typename DerivedType, typename BaseType>
auto PROP_TYPE::results() {
    DerivedType parent;
    if constexpr(am_base_pt) {
        return parent.results_();
    } else {
        return parent.results_() + BaseType::results();
    }
}

template<typename DerivedType, typename BaseType>
template<typename T, typename... Args>
auto PROP_TYPE::wrap_inputs(T&& rv, Args&&... args) {
    return wrap_(std::forward<T>(rv), inputs(), std::forward<Args>(args)...);
}

template<typename DerivedType, typename BaseType>
template<typename T, typename... Args>
auto& PROP_TYPE::wrap_results(T&& rv, Args&&... args) {
    return wrap_(std::forward<T>(rv), results(), std::forward<Args>(args)...);
}

template<typename DerivedType, typename BaseType>
template<typename T>
auto PROP_TYPE::unwrap_inputs(T&& rv) {
    // If there are no inputs to unwrap we return an empty tuple
    using input_type                 = decltype(DerivedType::inputs());
    using traits_type                = typename input_type::traits_type;
    using input_tuple                = typename traits_type::tuple_of_fields;
    static constexpr bool has_inputs = traits_type::nfields > 0;

    if constexpr(has_inputs) {
        return unwrap_(inputs(), std::forward<T>(rv));
    } else {
        return input_tuple{};
    }
}

template<typename DerivedType, typename BaseType>
template<typename T>
auto PROP_TYPE::unwrap_results(T&& rv) {
    return unwrap_(results(), std::forward<T>(rv));
}

template<typename DerivedType, typename BaseType>
template<typename T, typename U, typename... Args>
auto& PROP_TYPE::wrap_(T&& rv, U&& builder, Args&&... args) {
    if constexpr(sizeof...(Args) > 0)
        wrap_guts_<0>(std::forward<T>(rv), std::forward<U>(builder),
                      std::forward<Args>(args)...);
    return rv;
}

template<typename DerivedType, typename BaseType>
template<std::size_t ArgI, typename T, typename U, typename V, typename... Args>
void PROP_TYPE::wrap_guts_(T&& rv, U&& builder, V&& value, Args&&... args) {
    using traits_type     = typename std::decay_t<U>::traits_type;
    using tuple_of_fields = typename traits_type::tuple_of_fields;
    using type            = std::tuple_element_t<ArgI, tuple_of_fields>;
    using clean_V         = std::decay_t<V>;
    constexpr bool is_any = std::is_same_v<clean_V, pluginplay::type::any>;
    constexpr bool is_ref = std::is_reference_v<type>;
    detail_::STATIC_ASSERT_CONVERTIBLE_VERBOSE<V, type, ArgI>();

    auto key = (builder.begin() + ArgI)->first;
    if constexpr(is_any) {
        rv.at(key).change(std::forward<V>(value));
    } else if constexpr(is_ref && !is_any) {
        rv.at(key).change(static_cast<type>(value));
    } else {
        rv.at(key).change(std::forward<V>(value));
    }
    if constexpr(sizeof...(Args) > 0)
        wrap_guts_<ArgI + 1>(std::forward<T>(rv), std::forward<U>(builder),
                             std::forward<Args>(args)...);
}

template<typename DerivedType, typename BaseType>
template<typename T, typename U>
auto PROP_TYPE::unwrap_(T&& builder, U&& rv) {
    auto results =
      unwrap_guts_<0>(std::forward<T>(builder), std::forward<U>(rv));
    using tuple_type            = decltype(results);
    constexpr std::size_t nargs = std::tuple_size_v<tuple_type>;
    if constexpr(nargs == 0) return;
    //        else if constexpr(nargs == 1){
    //            using type = std::tuple_element_t <0, tuple_type>;
    //            return std::forward<type>(std::get<0>(results));
    //        }
    else
        return results;
}

template<typename DerivedType, typename BaseType>
template<std::size_t ArgI, typename T, typename U>
auto PROP_TYPE::unwrap_guts_(T&& builder, U&& rv) {
    using tuple_of_fields = typename T::traits_type::tuple_of_fields;
    constexpr auto nargs  = std::tuple_size_v<tuple_of_fields>;
    if constexpr(ArgI == nargs)
        return std::make_tuple();
    else {
        using type = std::tuple_element_t<ArgI, tuple_of_fields>;
        auto key   = (builder.begin() + ArgI)->first;
        auto lhs   = std::tuple<type>(rv.at(key).template value<type>());
        auto rhs =
          unwrap_guts_<ArgI + 1>(std::forward<T>(builder), std::forward<U>(rv));
        return std::tuple_cat(std::move(lhs), std::move(rhs));
    }
}

#undef PROP_TYPE
} // namespace pluginplay
