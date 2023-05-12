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
#include "../test_cache.hpp"
#include <pluginplay/cache/database/transposer.hpp>
#include <pluginplay/cache/database/type_eraser.hpp>

using namespace testing;
using namespace pluginplay::cache::database;

TEMPLATE_LIST_TEST_CASE("TypeEraser", "", test_types) {
    // Work out the type of the database we're testing
    using key_type   = TestType;
    using value_type = std::string;
    using db_type    = TypeEraser<key_type, value_type>;

    using key_set_type = typename db_type::key_set_type;

    // The type the DB uses for type-erased stuff
    using any_type = typename db_type::any_type;

    // N.B. AnyField doesn't define operator<, so it can't actually be used as
    //      keys. We use a Transposer to get around this fact
    using transposed_db = Transposer<any_type, value_type>;

    // Make the DB that goes into the Transposer
    auto [psub_sub_db, sub_db] = make_nested_native<value_type, any_type>();
    auto psub_db = std::make_unique<decltype(sub_db)>(std::move(sub_db));

    // Make the transposed DB
    auto psub_dbt = std::make_shared<transposed_db>(std::move(psub_db));

    // Make the DB we're testing
    db_type db(psub_dbt);

    // Make some keys and values to test with
    key_type key0{};
    auto key1 = lexical_cast<key_type>("42");
    value_type value0{};
    value_type value1{"Hello"};

    // Put key0, value0 into the DB we're testing
    db.insert(key0, value0);

    auto any0 = MakeAny<key_type>::convert(key0);

    SECTION("CTors") {
        REQUIRE_THROWS_AS(db_type(nullptr), std::runtime_error);
    }

    SECTION("keys") { REQUIRE(db.keys() == key_set_type{key0}); }

    SECTION("count") {
        REQUIRE(db.count(key0));
        REQUIRE_FALSE(db.count(key1));
    }

    SECTION("insert/at") {
        REQUIRE(db.at(key0).get() == value0);

        db.insert(key1, value1);
        REQUIRE(db.at(key1).get() == value1);
    }

    SECTION("free") {
        db.free(key0);
        REQUIRE_FALSE(db.count(key0));
        REQUIRE_FALSE(psub_dbt->count(any0));

        // Calling it multiple times is fine
        db.free(key0);
        REQUIRE_FALSE(db.count(key0));
        REQUIRE_FALSE(psub_dbt->count(any0));
    }

    SECTION("backup") {
        db.backup();
        // Still have the keys
        REQUIRE(db.count(key0));
        // Sub db still has it
        REQUIRE(psub_dbt->count(any0));
        // Make sure backup was really called
        REQUIRE(psub_sub_db->count(value0));
        REQUIRE(psub_sub_db->at(value0).get() == any0);
    }

    SECTION("dump") {
        db.dump();
        // Shouldn't have the key
        REQUIRE_FALSE(db.count(key0));
        // Sub db also shouldn't have it
        REQUIRE_FALSE(psub_dbt->count(any0));

        // Make sure dump was really called
        REQUIRE(psub_sub_db->count(value0));
        REQUIRE(psub_sub_db->at(value0).get() == any0);
    }
}
