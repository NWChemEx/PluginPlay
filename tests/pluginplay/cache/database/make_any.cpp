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

#include "../test_cache.hpp"
#include <catch2/catch.hpp>
#include <pluginplay/cache/database/make_any.hpp>
#include <vector>

using namespace pluginplay;
using namespace testing;

/* Testing Strategy:
 *
 * We first unit test the two specializations of MakeAny. Then we perform an
 * two intergration tests to make sure the `MakeAny` specializations are being
 * picked up.
 *
 */

TEST_CASE("MakeAny<ModuleInput>") {
    using input_type    = ModuleInput;
    using make_any_type = cache::database::MakeAny<input_type>;
    using v_type        = std::vector<int>;

    int i{42};
    v_type v{9, 8, 7};

    SECTION("int") {
        using type = int;
        using ref  = int&;
        input_type input;
        input.set_type<type>();
        input.change(i);
        auto corr = input.value<any::AnyField>();
        auto rv   = make_any_type::convert(input);

        // Value is correct
        REQUIRE(corr == rv);

        // Is a copy
        REQUIRE(&any::any_cast<ref>(corr) != &any::any_cast<ref>(rv));
    }

    SECTION("const int&") {
        using type = const int&;
        using ref  = const int&;
        input_type input;
        input.set_type<type>();
        input.change(i);
        auto corr = input.value<any::AnyField>();
        auto rv   = make_any_type::convert(input);

        // Value is correct
        REQUIRE(corr == rv);

        // Is a copy
        REQUIRE(&any::any_cast<ref>(corr) != &any::any_cast<ref>(rv));
    }

    SECTION("std::vector<int>") {
        using type = std::vector<int>;
        using ref  = std::vector<int>&;
        input_type input;
        input.set_type<type>();
        input.change(v);
        auto corr = input.value<any::AnyField>();
        auto rv   = make_any_type::convert(input);

        // Value is correct
        REQUIRE(corr == rv);

        // Is a copy
        REQUIRE(&any::any_cast<ref>(corr) != &any::any_cast<ref>(rv));
    }

    SECTION("const std::vector<int>&") {
        using type = const std::vector<int>&;
        using ref  = const std::vector<int>&;
        input_type input;
        input.set_type<type>();
        input.change(v);
        auto corr = input.value<any::AnyField>();
        auto rv   = make_any_type::convert(input);

        // Value is correct
        REQUIRE(corr == rv);

        // Is a copy
        REQUIRE(&any::any_cast<ref>(corr) != &any::any_cast<ref>(rv));
    }
}

TEST_CASE("MakeAny<ModuleResult>") {
    using result_type   = ModuleResult;
    using shared_any    = typename ModuleResult::shared_any;
    using make_any_type = cache::database::MakeAny<result_type>;
    using v_type        = std::vector<int>;

    int i{42};
    v_type v{9, 8, 7};

    SECTION("int") {
        using type = int;
        using ref  = const int&;
        result_type result;
        result.set_type<type>();
        result.change(i);
        auto corr = result.value<shared_any>();
        auto rv   = make_any_type::convert(result);

        // Value is correct
        REQUIRE(*corr == rv);

        // Is a copy
        REQUIRE(&any::any_cast<ref>(*corr) != &any::any_cast<ref>(rv));
    }

    SECTION("std::vector<int>") {
        using type = std::vector<int>;
        using ref  = const std::vector<int>&;
        result_type result;
        result.set_type<type>();
        result.change(v);
        auto corr = result.value<shared_any>();
        auto rv   = make_any_type::convert(result);

        // Value is correct
        REQUIRE(*corr == rv);

        // Is a copy
        REQUIRE(&any::any_cast<ref>(*corr) != &any::any_cast<ref>(rv));
    }
}

TEMPLATE_LIST_TEST_CASE("TypeEraser<ModuleInput>", "", testing::test_types) {
    using key_t   = ModuleInput;
    using val_t   = std::string;
    using te_type = cache::database::TypeEraser<key_t, val_t>;
    using any_t   = typename te_type::any_type;

    // Make the sub database which implements TypeEraser
    auto [psub_sub_sub_db, psub_sub_db, sub_db] =
      make_transposer<any_t, val_t>();
    using transposer_t = std::decay_t<decltype(sub_db)>;
    auto psub_db       = std::make_shared<transposer_t>(std::move(sub_db));

    // Make the TypeEraser
    te_type db(psub_db);

    // Make the unwrapped, wrapped, and type-erased value.
    TestType value{};
    auto wrapped_value = any::make_any_field<TestType>(value);
    key_t i;
    i.set_type<TestType>();
    i.change(value);

    // Check TypeEraser's initial state
    REQUIRE_FALSE(db.count(i));
    REQUIRE_FALSE(psub_db->count(wrapped_value));
    REQUIRE_FALSE(psub_sub_db->count("Hello"));
    REQUIRE_FALSE(psub_sub_sub_db->count("Hello"));

    // Add a key-value
    db.insert(i, "Hello");

    // Registers as added
    REQUIRE(db.count(i));
    REQUIRE(psub_db->count(wrapped_value));
    REQUIRE(psub_sub_db->count("Hello"));
    REQUIRE_FALSE(psub_sub_sub_db->count("Hello"));

    // Added value is correct
    REQUIRE(db.at(i).get() == "Hello");
    REQUIRE(psub_db->at(wrapped_value).get() == "Hello");
    REQUIRE(psub_sub_db->at("Hello").get() == wrapped_value);
}

TEMPLATE_LIST_TEST_CASE("TypeEraser<ModuleResult>", "", testing::test_types) {
    using key_t   = ModuleResult;
    using val_t   = std::string;
    using te_type = cache::database::TypeEraser<key_t, val_t>;
    using any_t   = typename te_type::any_type;

    // Make the sub database which implements TypeEraser
    auto [psub_sub_sub_db, psub_sub_db, sub_db] =
      make_transposer<any_t, val_t>();
    using transposer_t = std::decay_t<decltype(sub_db)>;
    auto psub_db       = std::make_shared<transposer_t>(std::move(sub_db));

    // Make the TypeEraser
    te_type db(psub_db);

    // Make the unwrapped, wrapped, and type-erased value.
    TestType value{};
    auto wrapped_value = any::make_any_field<TestType>(value);
    key_t i;
    i.set_type<TestType>();
    i.change(value);

    // Check TypeEraser's initial state
    REQUIRE_FALSE(db.count(i));
    REQUIRE_FALSE(psub_db->count(wrapped_value));
    REQUIRE_FALSE(psub_sub_db->count("Hello"));
    REQUIRE_FALSE(psub_sub_sub_db->count("Hello"));

    // Add a key-value
    db.insert(i, "Hello");

    // Registers as added
    REQUIRE(db.count(i));
    REQUIRE(psub_db->count(wrapped_value));
    REQUIRE(psub_sub_db->count("Hello"));
    REQUIRE_FALSE(psub_sub_sub_db->count("Hello"));

    // Added value is correct
    REQUIRE(db.at(i).get() == "Hello");
    REQUIRE(psub_db->at(wrapped_value).get() == "Hello");
    REQUIRE(psub_sub_db->at("Hello").get() == wrapped_value);
}
