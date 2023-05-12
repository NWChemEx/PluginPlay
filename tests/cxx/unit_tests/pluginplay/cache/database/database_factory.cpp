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
#include <filesystem>
#include <pluginplay/cache/database/database_factory.hpp>

using namespace pluginplay;
using namespace pluginplay::cache::database;

/* These tests serve as integration tests for the Database backend of the cache.
 * It is assumed that the pieces of the Database have been individually unit
 * tested, and found to work. The tests here focus on using the objects returned
 * by DatabaseFactory instances when:
 *
 * - there is no long-term storage
 * - there is long-term storage, but a fresh initialization
 * - there is long-term storage, and we're reading things in.
 */

TEST_CASE("DatabaseFactory : No long-term storage") {
    using input_map_type     = typename DatabaseFactory::input_map_type;
    using module_input_type  = typename DatabaseFactory::module_input_type;
    using result_map_type    = typename DatabaseFactory::result_map_type;
    using module_result_type = typename DatabaseFactory::module_result_type;

    // Make some inputs and results
    module_input_type i0, i1, i2;
    module_result_type r0, r1, r2;

    i0.set_type<int>();
    i0.change(42);
    i1.set_type<const std::string&>();
    i1.change("Hello World");
    i2.set_type<double>();
    i2.change(3.14);

    r0.set_type<int>();
    r0.change(1);
    r1.set_type<std::string>();
    r1.change("foo bar");
    r2.set_type<double>();
    r2.change(1.23);

    std::string key0("field 0"), key1("field 1"), key2("field 2");

    input_map_type inputs;
    inputs.emplace(key0, i0);
    inputs.emplace(key1, i1);
    inputs.emplace(key2, i2);

    result_map_type results;
    results.emplace(key0, r0);
    results.emplace(key1, r1);
    results.emplace(key2, r2);

    // Make the factory
    DatabaseFactory factory;
    auto pdb = factory.default_module_db("foo");

    REQUIRE_FALSE(pdb->count(inputs));

    pdb->insert(inputs, results);

    REQUIRE(pdb->count(inputs));
    REQUIRE(pdb->at(inputs).get() == results);
}
