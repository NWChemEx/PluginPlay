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
#include <map>
#include <pluginplay/cache/database/key_proxy_mapper.hpp>

using namespace pluginplay::cache;
using namespace pluginplay::cache::database;
using namespace testing;

TEMPLATE_LIST_TEST_CASE("KeyProxyMapper", "", testing::test_types) {
    // Type of the keys, values, and resulting KeyProxyMapper we're testing
    // N.B. double not in test_types so value_type != key_type
    using key_type     = std::map<std::string, TestType>;
    using value_type   = std::map<std::string, double>;
    using mapper_type  = KeyProxyMapper<key_type, value_type>;
    using key_set_type = typename mapper_type::key_set_type;

    // ProxyMappers need to sub objects a ProxyMapMaker and a sub database
    // First we figure out the ProxyMapMaker type and what's being proxied
    using map_maker_type = typename mapper_type::proxy_map_maker;
    using proxied_type   = typename map_maker_type::key_type;

    // Next we figure out the keys/values for the sub-database
    using sub_db_type  = typename mapper_type::sub_db_type;
    using sub_db_key   = typename sub_db_type::key_type;
    using sub_db_value = typename sub_db_type::mapped_type;

    // Make the ProxyMapMaker instance
    auto [pproxy_sub_sub_db, pproxy_sub_db, pproxy_db, mapper] =
      make_proxy_map_maker<proxied_type>();
    auto wrapped_mapper = std::make_unique<map_maker_type>(std::move(mapper));
    auto pmapper        = wrapped_mapper.get();

    // Make the Database we store the proxied relationship in
    auto [psub_sub_db, sub_db] = make_nested_native<sub_db_key, sub_db_value>();
    auto wrapped_sub_db = std::make_unique<decltype(sub_db)>(std::move(sub_db));
    auto psub_db        = wrapped_sub_db.get();

    // Make the database we're actually testing
    mapper_type db(std::move(wrapped_mapper), std::move(wrapped_sub_db));

    // Make two keys for the database
    key_type key0{{"Hello", TestType{}}};
    key_type key1{{"World", lexical_cast<TestType>("42")}};

    // Make two values for the database
    value_type value0{{"foo", 1.23}};
    value_type value1{{"bar", 3.14}};

    db.insert(key0, value0);

    // This is the value the ProxyMapMaker mapped key0 to, since that class is
    // known to work we don't have to test that it's: {{"Hello", <uuid>}}
    auto mapped_key0 = pmapper->at(key0);

    SECTION("CTor") {
        REQUIRE_THROWS_AS(mapper_type(nullptr, nullptr), std::runtime_error);
    }

    SECTION("keys") { REQUIRE(db.keys() == key_set_type{key0}); }

    SECTION("count") {
        REQUIRE(db.count(key0));
        REQUIRE_FALSE(db.count(key1));
    }

    SECTION("insert/at") {
        // Can get the value back via the real key
        REQUIRE(db.at(key0).get() == value0);
        // Value is stored under the proxied key
        REQUIRE(psub_db->at(mapped_key0).get() == value0);

        // For good measure add another key/value pair
        db.insert(key1, value1);
        // Can get the value back by the real key
        REQUIRE(db.at(key1).get() == value1);
        // Is actually stored under the proxied key
        REQUIRE(psub_db->at(pmapper->at(key1)).get() == value1);
    }

    SECTION("free") {
        db.free(key0);
        // No longer used by outermost database
        REQUIRE_FALSE(db.count(key0));
        // Not removed from ProxyMapMaker
        REQUIRE(pmapper->count(key0));
        // Is actually removed from inner database
        REQUIRE_FALSE(psub_db->count(mapped_key0));

        // Repeated free-ing is okay and doesn't do anything
        db.free(key0);
        REQUIRE_FALSE(db.count(key0));
        REQUIRE(pmapper->count(key0));
        REQUIRE_FALSE(psub_db->count(mapped_key0));
    }

    SECTION("backup") {
        db.backup();
        // Still in outermost database
        REQUIRE(db.count(key0));
        // Still in ProxyMapMaker
        REQUIRE(pmapper->count(key0));
        // Check that we called backup on pmapper
        TestType v{};
        REQUIRE(pproxy_sub_sub_db->count(v));
        REQUIRE(pproxy_sub_sub_db->at(v).get() == mapped_key0["Hello"]);
        // Still in sub_db
        REQUIRE(psub_db->count(mapped_key0));
        // Check that we called backup on psub_db
        REQUIRE(psub_sub_db->count(mapped_key0));
    }

    SECTION("dump") {
        db.dump();
        // No longer in outermost database
        REQUIRE_FALSE(db.count(key0));
        // Still in ProxyMapMaker
        REQUIRE(pmapper->count(key0));
        // Check that we called backup on pmapper
        TestType v{};
        REQUIRE(pproxy_sub_sub_db->count(v));
        REQUIRE(pproxy_sub_sub_db->at(v).get() == mapped_key0["Hello"]);
        // No longer in sub_db
        REQUIRE_FALSE(psub_db->count(mapped_key0));
        // Check that we called dump on psub_db
        REQUIRE(psub_sub_db->count(mapped_key0));
    }
}
