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

#include "pluginplay/any/any.hpp"
#include "test_any.hpp"

using namespace pluginplay::any;

/* Testing Strategy:
 *
 * The majority of make_any_field is taken care of by the ctors
 * for good measure we exhaustively test cv and reference-qualified types for
 * each to ensure that the types are forwarded correctly. For simplicity we also
 * test unwrapping the values, via pluginplay::any_cast at the same time
 * (otherwise we'd duplicate the construction for the any_cast tests)
 */

TEMPLATE_LIST_TEST_CASE("make_any_field/any_cast", "", testing::types2test) {
    using type    = TestType;
    using error_t = std::runtime_error;

    auto value = testing::non_default_value<type>();

    auto has_value = make_any_field<type>(value);
    auto has_cval  = make_any_field<const type>(value);
    auto has_cref  = make_any_field<const type&>(value);

    //
    // The following 2 tests should raise static asserts (uncomment to test)
    //
    // REQUIRE_THROWS_AS(make_any_field<type&>(value), error_t);
    // REQUIRE_THROWS_AS(any_cast<type>(value), error_t);

    // Can get the value back by copy
    REQUIRE(any_cast<type>(has_value) == value);
    REQUIRE(any_cast<type>(has_cval) == value);
    REQUIRE(any_cast<type>(has_cref) == value);

    // Can get the value back by read-only copy
    REQUIRE(any_cast<const type>(has_value) == value);
    REQUIRE(any_cast<const type>(has_cval) == value);
    REQUIRE(any_cast<const type>(has_cref) == value);

    // Can get the value back by mutable reference (when we're allowed to)
    REQUIRE(any_cast<type&>(has_value) == value);
    REQUIRE_THROWS_AS(any_cast<type&>(has_cval), error_t);
    REQUIRE_THROWS_AS(any_cast<type&>(has_cref), error_t);

    // Can get the value back by read-only reference
    REQUIRE(any_cast<const type&>(has_value) == value);
    REQUIRE(any_cast<const type&>(has_cval) == value);
    REQUIRE(any_cast<const type&>(has_cref) == value);

    // Additional check that no copy occurred when wrapped by cref
    REQUIRE(&any_cast<const type&>(has_cref) == &value);

    // Throws if wrapped value isn't convertible to requested type
    REQUIRE_THROWS_AS(any_cast<error_t>(has_value), error_t);

    // Throws if AnyInput is empty
    REQUIRE_THROWS_AS(any_cast<type>(AnyField{}), error_t);

    // Ensures that we actually move the value into the AnyInput and don't
    // copy it.
    if constexpr(std::is_same_v<type, std::vector<int>>) {
        const auto* p = value.data();
        auto moved    = make_any_field<type>(std::move(value));
        REQUIRE(p == any_cast<const type&>(moved).data());
    }
}
