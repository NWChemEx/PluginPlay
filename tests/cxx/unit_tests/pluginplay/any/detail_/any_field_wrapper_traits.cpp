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

#include "../test_any.hpp"
#include "pluginplay/any/detail_/any_field_wrapper_traits.hpp"

using namespace pluginplay::any::detail_;

TEMPLATE_TEST_CASE("is_any_field_wrapper", "", testing::types2test) {
    using T            = TestType;
    using wrapper_type = AnyFieldWrapper<T>;
    STATIC_REQUIRE_FALSE(is_any_field_wrapper<T>::value);
    STATIC_REQUIRE(is_any_field_wrapper<wrapper_type>::value);
}

TEMPLATE_TEST_CASE("disable_if_any_field_wrapper_t", "", testing::types2test) {
    using T            = TestType;
    using wrapper_type = AnyFieldWrapper<T>;
    STATIC_REQUIRE(std::is_same_v<int, disable_if_any_field_wrapper_t<T, int>>);
    // Should fail to compile, uncomment to check
    // STATIC_REQUIRE(
    //  std::is_same_v<int, disable_if_any_field_wrapper_t<wrapper_type>>);
}
