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
#include <pluginplay/cache/database/serialized.hpp>
#include <sstream>

/* Testing Strategy:
 *
 * For simplicity we have the Serialized instance we test wrap a testing::Map
 * instance. The latter is unit tested in map.cpp and is known to work. This
 * also provides with a backend which is always present (as opposed to relying
 * on say the RocksDB backend). The unit tests here only sample a couple
 * key/value choices; however, they can broadly be thought of as covering mixes
 * of literals (int) and class types (std::string). Since the tests do not rely
 * on any specific properties of int/std::string the test is not expected to
 * fail for
 */

using namespace pluginplay::cache::database;

using ii_pair    = std::pair<int, int>;
using is_pair    = std::pair<int, std::string>;
using si_pair    = std::pair<std::string, int>;
using ss_pair    = std::pair<std::string, std::string>;
using test_types = std::tuple<ii_pair, is_pair, si_pair, ss_pair>;

TEMPLATE_LIST_TEST_CASE("Serialized", "", test_types) {
    // Unwrap the types for this test
    using value_type  = TestType;
    using key_type    = std::tuple_element_t<0, value_type>;
    using mapped_type = std::tuple_element_t<1, value_type>;

    // Determine the specialization of Serialized being tested
    using serialized_type = Serialized<key_type, mapped_type>;

    // Determine the type returned by keys()
    using key_set_type = typename serialized_type::key_set_type;

    // Work out the type of the wrapped database
    using binary_type = typename serialized_type::binary_type;
    using sub_db_type = Native<binary_type, binary_type>;

    // Make the database wrapped by the Serialized instance
    auto pdb = std::make_unique<sub_db_type>();

    // Make some keys and values to put into the instance
    key_type key0{};
    auto key1 = testing::lexical_cast<key_type>("1");
    mapped_type value0{};
    auto value1 = testing::lexical_cast<mapped_type>("42");

    // Make the Serialized instance
    serialized_type smap(std::move(pdb));

    smap.insert(key1, value1);

    SECTION("keys") { REQUIRE(smap.keys() == key_set_type{key1}); }

    SECTION("count") {
        REQUIRE_FALSE(smap.count(key0));
        REQUIRE(smap.count(key1));
    }

    SECTION("insert/at") {
        smap.insert(key0, value0);
        REQUIRE(smap.count(key0));
        REQUIRE(smap.at(key0).get() == value0);

        // Can overwrite existing key
        smap.insert(key0, value1);
        REQUIRE(smap.count(key0));
        REQUIRE(smap.at(key0).get() == value1);
    }

    SECTION("free") {
        smap.free(key0);
        REQUIRE_FALSE(smap.count(key0));

        // Can free it twice
        smap.free(key0);
        REQUIRE_FALSE(smap.count(key0));
    }

    SECTION("dump") {
        smap.dump();
        REQUIRE_FALSE(smap.count(key0));
    }
}
