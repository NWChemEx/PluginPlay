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

#include "../lexical_cast.hpp"
#include <catch2/catch.hpp>
#include <map>
#include <pluginplay/cache/database/key_injector.hpp>
#include <pluginplay/cache/database/native.hpp>

using namespace pluginplay::cache::database;

using test_types = std::tuple<int, double, std::string>;

TEMPLATE_LIST_TEST_CASE("KeyInjector", "", test_types) {
    using key_key_type   = std::string;
    using key_value_type = TestType;
    using key_type       = std::map<key_key_type, key_value_type>;
    using value_type     = key_type;

    using db_type      = KeyInjector<key_type, value_type>;
    using sub_db_type  = Native<key_type, value_type>;
    using key_set_type = typename db_type::key_set_type;

    auto sub_sub_db = std::make_unique<sub_db_type>();
    auto psub       = sub_sub_db.get();
    auto sub_db     = std::make_shared<sub_db_type>(std::move(sub_sub_db));

    auto value = testing::lexical_cast<key_value_type>("42");
    key_key_type defaulted_key{};
    key_value_type defaulted_value{};

    key_type key0{};
    key_type key1{{"hello", value}};

    value_type value0{};

    db_type db(defaulted_key, defaulted_value, sub_db);
    db.insert(key0, value0);

    SECTION("CTor") {
        REQUIRE_THROWS_AS(db_type(defaulted_key, defaulted_value, nullptr),
                          std::runtime_error);
    }

    SECTION("keys") { REQUIRE(db.keys() == key_set_type{key0}); }

    SECTION("count") {
        REQUIRE(db.count(key0));
        REQUIRE_FALSE(db.count(key1));
    }

    SECTION("insert/at") {
        // Can get the value back through the KeyInjector API
        REQUIRE(db.at(key0).get() == value0);

        // Make sure the key/value pair was injected
        key0.emplace(defaulted_key, defaulted_value);
        REQUIRE(sub_db->count(key0));
        REQUIRE(sub_db->at(key0).get() == value0);

        // Can insert/at the value through the KeyInjector API
        db.insert(key1, value0);
        REQUIRE(db.at(key1).get() == value0);

        // Make sure it actually injected the key/value pair
        key1.emplace(defaulted_key, defaulted_value);
        REQUIRE(sub_db->count(key1));
        REQUIRE(sub_db->at(key1).get() == value0);
    }

    SECTION("free") {
        db.free(key0);
        REQUIRE_FALSE(db.count(key0));

        // Can call it twice
        db.free(key0);
        REQUIRE_FALSE(db.count(key0));

        // Make sure it actually freed it up
        key0.emplace(defaulted_key, defaulted_value);
        REQUIRE_FALSE(sub_db->count(key0));
    }

    SECTION("backup") {
        // Still in this DB
        db.backup();
        REQUIRE(db.count(key0));

        // Still in wrapped DB
        key0.emplace(defaulted_key, defaulted_value);
        REQUIRE(sub_db->count(key0));

        // Now it's in the sub db too
        REQUIRE(psub->count(key0));
    }

    SECTION("dump") {
        // Not in this DB
        db.dump();
        REQUIRE_FALSE(db.count(key0));

        // Not in wrapped DB
        key0.emplace(defaulted_key, defaulted_value);
        REQUIRE_FALSE(sub_db->count(key0));

        // Still in the sub db
        REQUIRE(psub->count(key0));
    }
}
