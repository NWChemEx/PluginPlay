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
 * AnyFieldWrapper implements AnyFieldBase. Functions implemented in
 * AnyFieldBase are tested in AnyFieldBase.cpp and do not need to be retested
 * here. The unit tests here focus on methods implemented in AnyFieldBase.
 *
 * Considerations:
 * - several wrapped types via testing::types2test
 * - wrapping by value, const value, const reference
 * - instances wrap types T and U such that std::decay_t<T> != std::decay_t<U>
 */

TEMPLATE_LIST_TEST_CASE("AnyFieldWrapper", "", testing::types2test) {
    using type              = TestType;
    using wrapper_type      = AnyFieldWrapper<type>;
    using cval_wrapper      = AnyFieldWrapper<const type>;
    using cref_wrapper      = AnyFieldWrapper<const type&>;
    using rtti_type         = typename wrapper_type::rtti_type;
    using different_type    = std::map<int, double>; // A type no in types2test
    using different_wrapper = AnyFieldWrapper<different_type>;

    rtti_type rtti(typeid(type));

    type default_value{};
    auto value = testing::non_default_value<type>();

    wrapper_type defaulted(default_value);
    wrapper_type has_value(value);
    cval_wrapper const_val(value);
    cref_wrapper const_ref(value);
    different_wrapper diff(different_type{});

    SECTION("Value CTor") {
        REQUIRE(defaulted.type() == rtti);
        REQUIRE(has_value.type() == rtti);
        REQUIRE(const_val.type() == rtti);
        REQUIRE(const_ref.type() == rtti);

        REQUIRE(defaulted.template cast<const type&>() == default_value);
        REQUIRE(has_value.template cast<const type&>() == value);
        REQUIRE(const_val.template cast<const type&>() == value);
        REQUIRE(&const_ref.template cast<const type&>() == &value);

        // Ensure we can move complicated types when stored by value
        if constexpr(std::is_same_v<type, std::vector<int>>) {
            const auto corr = value.data();
            wrapper_type moved(std::move(value));
            auto p = moved.template cast<const type&>().data();
            REQURIE(corr == p);
        }
    }

    SECTION("clone") {
        auto defaulted2 = defaulted.clone();
        REQUIRE(defaulted2->are_equal(defaulted));

        auto has_value2 = has_value.clone();
        REQUIRE(has_value2->are_equal(has_value));

        auto const_val2 = const_val.clone();
        REQUIRE(const_val2->value_equal(const_val));

        auto const_ref2 = const_ref.clone();
        REQUIRE(const_ref2->value_equal(const_ref));

        // Is deep copy
        auto corr = &const_ref.template cast<const type&>();
        REQUIRE(corr == &value);
        REQUIRE(&const_ref2->template cast<const type&>() != corr);
        REQUIRE(const_ref2->template cast<const type&>() == value);
    }

    SECTION("are_equal") {
        // Defaults are equal
        REQUIRE(defaulted.are_equal(wrapper_type(default_value)));

        // Values are equal
        REQUIRE(has_value.are_equal(wrapper_type(value)));

        // Default != value
        REQUIRE_FALSE(defaulted.are_equal(has_value));

        // Value != const Value
        REQUIRE_FALSE(has_value.are_equal(const_val));

        // Value != const ref
        REQUIRE_FALSE(has_value.are_equal(const_ref));

        // const value != const ref
        REQUIRE_FALSE(const_val.are_equal(const_ref));

        // Different types
        REQUIRE_FALSE(defaulted.are_equal(diff));
    }

    SECTION("value_equal") {
        REQUIRE(defaulted.value_equal(wrapper_type(default_value)));
        REQUIRE_FALSE(defaulted.value_equal(has_value));
        REQUIRE_FALSE(defaulted.value_equal(const_val));
        REQUIRE_FALSE(defaulted.value_equal(const_ref));
        REQUIRE_FALSE(defaulted.value_equal(diff));

        REQUIRE_FALSE(has_value.value_equal(defaulted));
        REQUIRE(has_value.value_equal(wrapper_type(value)));
        REQUIRE(has_value.value_equal(const_val));
        REQUIRE(has_value.value_equal(const_ref));
        REQUIRE_FALSE(has_value.value_equal(diff));

        REQUIRE_FALSE(const_val.value_equal(defaulted));
        REQUIRE(const_val.value_equal(has_value));
        REQUIRE(const_val.value_equal(cval_wrapper(value)));
        REQUIRE(const_val.value_equal(const_ref));
        REQUIRE_FALSE(const_val.value_equal(diff));

        REQUIRE_FALSE(const_ref.value_equal(defaulted));
        REQUIRE(const_ref.value_equal(has_value));
        REQUIRE(const_ref.value_equal(const_val));
        REQUIRE(const_val.value_equal(cref_wrapper(value)));
        REQUIRE_FALSE(const_val.value_equal(diff));
    }

    SECTION("type") {
        REQUIRE(defaulted.type() == rtti);
        REQUIRE(has_value.type() == rtti);
        REQUIRE(const_val.type() == rtti);
        REQUIRE(const_ref.type() == rtti);
    }

    SECTION("print") {
        std::stringstream ss;

        auto pss = &(has_value.print(ss));
        REQUIRE(pss == &ss);

        if constexpr(std::is_same_v<type, int>) {
            REQUIRE(ss.str() == "42");
        } else if constexpr(std::is_same_v<type, double>) {
            REQUIRE(ss.str() == "3.14");
        } else if constexpr(std::is_same_v<type, std::string>) {
            REQUIRE(ss.str() == "Hello World");
        }
    }

    SECTION("storing_const_reference") {
        REQUIRE_FALSE(defaulted.storing_const_reference());
        REQUIRE_FALSE(has_value.storing_const_reference());
        REQUIRE_FALSE(const_val.storing_const_reference());
        REQUIRE(const_ref.storing_const_reference());
    }

    SECTION("storing_const_value") {
        REQUIRE_FALSE(defaulted.storing_const_value());
        REQUIRE_FALSE(has_value.storing_const_value());
        REQUIRE(const_val.storing_const_value());
        REQUIRE_FALSE(const_ref.storing_const_value());
    }
}
