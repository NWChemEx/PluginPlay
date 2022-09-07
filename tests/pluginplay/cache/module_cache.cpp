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

#include "test_cache.hpp"
#include <catch2/catch.hpp>
#include <pluginplay/cache/module_cache.hpp>
#include <pluginplay/cache/module_manager_cache.hpp>

using namespace pluginplay::cache;

/* Testing Strategy:
 *
 * ModuleCache instances are really meant to be created by ModuleManagerCache
 * instances. So we use a ModuleManagerCache instance as a factory. The
 * implementation details specific to the ModuleManagerCache are unit tested
 * elsewhere, here we assume they work and worry about unit testing only the
 * features of the ModuleCache class.
 */

TEMPLATE_LIST_TEST_CASE("ModuleCache", "", testing::test_types) {
    // Get types for input/results
    using key_type    = ModuleCache::key_type;
    using input_type  = key_type::mapped_type;
    using val_type    = ModuleCache::mapped_type;
    using result_type = val_type::mapped_type;

    // Type that doesn't show up in test_types
    using map_t = std::map<int, int>;

    input_type i0, i1;
    i0.set_type<TestType>();
    i0.change(TestType{});
    i1.set_type<map_t>();
    i1.change(map_t{});

    result_type r0, r1;
    r0.set_type<TestType>();
    r0.change(TestType{});
    r1.set_type<map_t>();
    r1.change(map_t{});

    key_type inputs0{{"Hello", i0}, {"World", i1}};
    key_type inputs1{{"green", i1}, {"Blue", i0}};
    val_type results0{{"foo", r0}, {"bar", r1}};
    val_type results1{{"big", r1}, {"bug", r0}};

    ModuleCache default_mod_cache;

    ModuleManagerCache cache;
    auto mod_cache = cache.get_or_make_module_cache("my module's cache");
    mod_cache->cache(inputs0, results0);

    SECTION("Move ctor") {
        ModuleCache moved_to(std::move(*mod_cache));
        REQUIRE(moved_to.count(inputs0));
        REQUIRE(moved_to.uncache(inputs0) == results0);
    }

    SECTION("count") {
        REQUIRE_FALSE(default_mod_cache.count(inputs0));
        REQUIRE_FALSE(default_mod_cache.count(inputs1));

        REQUIRE(mod_cache->count(inputs0));
        REQUIRE_FALSE(mod_cache->count(inputs1));
    }

    SECTION("cache/uncache") {
        using e0 = std::runtime_error;
        using e1 = std::out_of_range;
        REQUIRE_THROWS_AS(default_mod_cache.cache(inputs0, results0), e0);
        REQUIRE_THROWS_AS(default_mod_cache.uncache(inputs0), e1);

        REQUIRE(mod_cache->uncache(inputs0) == results0);

        // Cann add a new pair
        mod_cache->cache(inputs1, results0);
        REQUIRE(mod_cache->uncache(inputs1) == results0);
        REQUIRE(mod_cache->uncache(inputs1) == results0);

        // Can overwrite previous result
        mod_cache->cache(inputs0, results1);
        REQUIRE(mod_cache->uncache(inputs0) == results1);
        REQUIRE(mod_cache->uncache(inputs1) == results0);
    }

    SECTION("clear") {
        default_mod_cache.clear();
        REQUIRE_FALSE(default_mod_cache.count(inputs0));
        REQUIRE_FALSE(default_mod_cache.count(inputs1));

        mod_cache->clear();
        REQUIRE_FALSE(mod_cache->count(inputs0));
        REQUIRE_FALSE(mod_cache->count(inputs1));
    }
}
