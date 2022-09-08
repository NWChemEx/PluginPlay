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

#include "lexical_cast.hpp"
#include "test_cache.hpp"
#include <map>
#include <pluginplay/cache/proxy_map_maker.hpp>

using namespace pluginplay::cache;
using namespace pluginplay::cache::database;
using namespace testing;

TEMPLATE_LIST_TEST_CASE("ProxyMapMaker", "", testing::test_types) {
    using key_type = std::map<std::string, TestType>;

    auto [psub_sub, psub, puuid_db, db] = make_proxy_map_maker<key_type>();
    using db_type                       = decltype(db);
    using value_type                    = typename db_type::mapped_type;
    using key_set_type                  = typename db_type::key_set_type;

    TestType default_value{};
    key_type key0{{"world", default_value}};

    auto other_value = testing::lexical_cast<TestType>("42");
    key_type key1{{"hello", other_value}};

    db.insert(key0);
    auto uuid = psub->at(default_value).get();
    value_type value0{{"world", uuid}};

    SECTION("CTor") { REQUIRE_THROWS_AS(db_type(nullptr), std::runtime_error); }

    SECTION("keys") { REQUIRE(db.keys() == key_set_type{key0}); }

    SECTION("Count") {
        REQUIRE(db.count(key0));
        REQUIRE_FALSE(db.count(key1));
    }

    SECTION("insert/at") {
        REQUIRE(psub->count(default_value));
        REQUIRE(psub->at(default_value).get() == uuid);
        REQUIRE(db.at(key0) == value0);

        db.insert(key1);
        REQUIRE(db.count(key1));
        value_type value1{{"hello", psub->at(other_value).get()}};
        REQUIRE(db.at(key1) == value1);
    }

    SECTION("un_proxy") { REQUIRE(db.un_proxy(value0) == key0); }

    SECTION("free") {
        db.free(key0);
        REQUIRE_FALSE(db.count(key0));
        REQUIRE_FALSE(psub->count(default_value));
    }

    SECTION("backup") {
        db.backup();

        REQUIRE(db.count(key0));
        REQUIRE(psub->count(default_value));
        REQUIRE(psub_sub->count(default_value));
        REQUIRE(psub_sub->at(default_value).get() == uuid);
    }

    SECTION("dump") {
        db.dump();

        REQUIRE_FALSE(db.count(key0));
        REQUIRE_FALSE(psub->count(default_value));
        REQUIRE(psub_sub->count(default_value));
        REQUIRE(psub_sub->at(default_value).get() == uuid);
    }
}
