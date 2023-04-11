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
#include <pluginplay/cache/database/native.hpp>
#include <pluginplay/cache/database/transposer.hpp>

using namespace pluginplay::cache::database;

using id_pair    = std::pair<int, double>;
using si_pair    = std::pair<std::string, int>;
using test_types = std::tuple<id_pair, si_pair>;

TEMPLATE_LIST_TEST_CASE("Transposer", "", test_types) {
    using value_type  = TestType;
    using key_type    = std::tuple_element_t<0, value_type>;
    using mapped_type = std::tuple_element_t<1, value_type>;

    using wrapped_db_type = Native<mapped_type, key_type>;
    using backup_db_type  = Native<mapped_type, key_type>;
    using db_type         = Transposer<key_type, mapped_type>;

    using key_set_type = typename db_type::key_set_type;

    key_type key0{};
    auto key1 = testing::lexical_cast<key_type>("42");
    mapped_type val0{};
    auto val1 = testing::lexical_cast<mapped_type>("12");

    auto backup_db  = std::make_unique<backup_db_type>();
    auto pbackup    = backup_db.get();
    auto wrapped_db = std::make_unique<wrapped_db_type>(std::move(backup_db));

    db_type has_val(std::move(wrapped_db));
    has_val.insert(key0, val0);

    SECTION("Ctor") {
        using ptr_type = typename db_type::wrapped_db_pointer;
        REQUIRE_THROWS_AS(db_type(ptr_type{}), std::runtime_error);
    }

    SECTION("keys") { REQUIRE(has_val.keys() == key_set_type{key0}); }

    SECTION("Count") {
        REQUIRE(has_val.count(key0));
        REQUIRE_FALSE(has_val.count(key1));
    }

    SECTION("insert/at") {
        REQUIRE(has_val.at(key0).get() == val0);

        has_val.insert(key1, val1);
        REQUIRE(has_val.count(key1));
        REQUIRE(has_val.at(key1).get() == val1);

        // Can overwrite
        has_val.insert(key1, val0);
        REQUIRE(has_val.at(key1).get() == val0);
    }

    SECTION("free") {
        has_val.free(key0);
        REQUIRE_FALSE(has_val.count(key0));

        // Can free same key multiple times
        has_val.free(key0);
        REQUIRE_FALSE(has_val.count(key0));
    }

    SECTION("backup") {
        has_val.backup();
        REQUIRE(has_val.count(key0));
        REQUIRE(pbackup->count(val0));
        REQUIRE(pbackup->at(val0).get() == key0);
    }

    SECTION("dump") {
        has_val.dump();
        REQUIRE_FALSE(has_val.count(key0));
        REQUIRE(pbackup->count(val0));
        REQUIRE(pbackup->at(val0).get() == key0);
    }
}
