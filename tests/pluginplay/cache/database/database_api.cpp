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

#include <catch2/catch.hpp>
#include <pluginplay/cache/database/database_api.hpp>
#include <pluginplay/cache/database/native.hpp>

using namespace pluginplay::cache::database;

/* Testing Strategy:
 *
 * For the most part DatabasePIMPL only defines an API. Classes which derive
 * from DatabasePIMPL should test their implementations through the API of
 * DatabasePIMPL to ensure they work. The exception to this is the `at` method,
 * which relies on count to determine if a key exists before retrieving the
 * value. If count_ and at_ work, then we only need to test that the logic in
 * at is setup correctly so that it throws when a value isn't found.
 */

TEST_CASE("DatabasePIMPL") {
    Native<std::string, std::string> m;
    m.insert("Hello", "World");

    SECTION("at") {
        REQUIRE(m.at("Hello").get() == "World");
        REQUIRE_THROWS_AS(m.at("Not a key"), std::out_of_range);
    }
}
