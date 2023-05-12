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
#include "pluginplay/any/detail_/any_field_wrapper.hpp"
#include <sstream>

using namespace pluginplay::any::detail_;

/* Testing Strategy:
 *
 * AnyFieldBase is abstract so we need to make a derived instance and test
 * through that. Here we only test functions which are fully implemented in
 * AnyFieldBase.
 */

TEMPLATE_LIST_TEST_CASE("AnyFieldBase", "", testing::types2test) {
    using type         = TestType;
    using wrapper_type = AnyFieldWrapper<type>;
    using cval_wrapper = AnyFieldWrapper<const type>;
    using cref_wrapper = AnyFieldWrapper<const type&>;
    using rtti_type    = typename wrapper_type::rtti_type;
    using map_type     = std::map<int, double>; // Type we know it won't contain

    rtti_type rtti(typeid(type));

    type default_value{};
    auto value = testing::non_default_value<type>();

    wrapper_type defaulted(default_value);
    wrapper_type has_value(value);
    cval_wrapper const_val(value);
    cref_wrapper const_ref(value);

    SECTION("cast") {
        using error_t = std::runtime_error;

        REQUIRE(defaulted.template cast<type>() == default_value);
        REQUIRE(defaulted.template cast<const type>() == default_value);
        REQUIRE(defaulted.template cast<type&>() == default_value);
        REQUIRE(defaulted.template cast<const type&>() == default_value);
        REQUIRE_THROWS_AS(defaulted.template cast<map_type>(), error_t);

        REQUIRE(has_value.template cast<type>() == value);
        REQUIRE(has_value.template cast<const type>() == value);
        REQUIRE(has_value.template cast<type&>() == value);
        REQUIRE(has_value.template cast<const type&>() == value);
        REQUIRE_THROWS_AS(has_value.template cast<map_type>(), error_t);

        REQUIRE(const_val.template cast<type>() == value);
        REQUIRE(const_val.template cast<const type>() == value);
        REQUIRE_THROWS_AS(const_val.template cast<type&>() == value, error_t);
        REQUIRE(const_val.template cast<const type&>() == value);
        REQUIRE_THROWS_AS(const_val.template cast<map_type>(), error_t);

        REQUIRE(const_ref.template cast<type>() == value);
        REQUIRE(const_ref.template cast<const type>() == value);
        REQUIRE_THROWS_AS(const_ref.template cast<type&>() == value, error_t);
        REQUIRE(const_ref.template cast<const type&>() == value);
        REQUIRE_THROWS_AS(const_ref.template cast<map_type>(), error_t);
    }

    SECTION("cast() const") {
        using error_t = std::runtime_error;

        const auto& cdefault = std::as_const(defaulted);
        REQUIRE(cdefault.template cast<type>() == default_value);
        REQUIRE(cdefault.template cast<const type>() == default_value);
        REQUIRE(cdefault.template cast<const type&>() == default_value);
        REQUIRE_THROWS_AS(cdefault.template cast<map_type>(), error_t);

        const auto& cval = std::as_const(has_value);
        REQUIRE(cval.template cast<type>() == value);
        REQUIRE(cval.template cast<const type>() == value);
        REQUIRE(cval.template cast<const type&>() == value);
        REQUIRE_THROWS_AS(cval.template cast<map_type>(), error_t);

        const auto& cval2 = std::as_const(const_val);
        REQUIRE(cval2.template cast<type>() == value);
        REQUIRE(cval2.template cast<const type>() == value);
        REQUIRE(cval2.template cast<const type&>() == value);
        REQUIRE_THROWS_AS(cval2.template cast<map_type>(), error_t);

        const auto& cref = std::as_const(const_ref);
        REQUIRE(cref.template cast<type>() == value);
        REQUIRE(cref.template cast<const type>() == value);
        REQUIRE(cref.template cast<const type&>() == value);
        REQUIRE_THROWS_AS(cref.template cast<map_type>(), error_t);

        // Trying to get a mutable reference back from a read-only AnyFieldBase
        // will trip a static assert (and it should). Uncomment this line to
        // check it
        // REQUIRE(cdefault.template cast<type&>() == default_value);
    }

    SECTION("is_convertible") {
        REQUIRE(defaulted.template is_convertible<type>());
        REQUIRE(defaulted.template is_convertible<const type>());
        REQUIRE(defaulted.template is_convertible<type&>());
        REQUIRE(defaulted.template is_convertible<const type&>());
        REQUIRE_FALSE(defaulted.template is_convertible<map_type>());

        REQUIRE(has_value.template is_convertible<type>());
        REQUIRE(has_value.template is_convertible<const type>());
        REQUIRE(has_value.template is_convertible<type&>());
        REQUIRE(has_value.template is_convertible<const type&>());
        REQUIRE_FALSE(has_value.template is_convertible<map_type>());

        REQUIRE(const_val.template is_convertible<type>());
        REQUIRE(const_val.template is_convertible<const type>());
        REQUIRE_FALSE(const_val.template is_convertible<type&>());
        REQUIRE(const_val.template is_convertible<const type&>());
        REQUIRE_FALSE(const_val.template is_convertible<map_type>());

        REQUIRE(const_ref.template is_convertible<type>());
        REQUIRE(const_ref.template is_convertible<const type>());
        REQUIRE_FALSE(const_ref.template is_convertible<type&>());
        REQUIRE(const_ref.template is_convertible<const type&>());
        REQUIRE_FALSE(const_ref.template is_convertible<map_type>());
    }

    SECTION("is_convertible() const") {
        const auto& cdefault = std::as_const(defaulted);
        REQUIRE(cdefault.template is_convertible<type>());
        REQUIRE(cdefault.template is_convertible<const type>());
        REQUIRE_FALSE(cdefault.template is_convertible<type&>());
        REQUIRE(cdefault.template is_convertible<const type&>());
        REQUIRE_FALSE(cdefault.template is_convertible<map_type>());

        const auto& cval = std::as_const(has_value);
        REQUIRE(cval.template is_convertible<type>());
        REQUIRE(cval.template is_convertible<const type>());
        REQUIRE_FALSE(cval.template is_convertible<type&>());
        REQUIRE(cval.template is_convertible<const type&>());
        REQUIRE_FALSE(cval.template is_convertible<map_type>());

        const auto& cval2 = std::as_const(const_val);
        REQUIRE(cval2.template is_convertible<type>());
        REQUIRE(cval2.template is_convertible<const type>());
        REQUIRE_FALSE(cval2.template is_convertible<type&>());
        REQUIRE(cval2.template is_convertible<const type&>());
        REQUIRE_FALSE(cval2.template is_convertible<map_type>());

        const auto& cref = std::as_const(const_ref);
        REQUIRE(cref.template is_convertible<type>());
        REQUIRE(cref.template is_convertible<const type>());
        REQUIRE_FALSE(cref.template is_convertible<type&>());
        REQUIRE(cref.template is_convertible<const type&>());
        REQUIRE_FALSE(cref.template is_convertible<map_type>());
    }
}
