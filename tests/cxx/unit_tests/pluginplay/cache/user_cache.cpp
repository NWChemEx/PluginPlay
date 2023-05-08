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
#include <pluginplay/cache/module_manager_cache.hpp>
#include <pluginplay/cache/user_cache.hpp>
#include <vector>

using namespace pluginplay::cache;

TEST_CASE("UserCache") {
    ModuleManagerCache cache;
    UserCache default_cache;
    auto pcache = cache.get_or_make_user_cache("my module's cache");

    std::string v0("hello world");
    int v1{42};
    double v2{3.14};
    std::vector<double> v3{1.23, 3.45};

    pcache->cache(v0, v1);

    SECTION("Does not interfere with the corresponding ModuleCache") {
        auto pmod_cache  = cache.get_or_make_module_cache("my module's cache");
        using sub_cache  = UserCache::sub_cache_type;
        using input_map  = sub_cache::key_type;
        using input      = input_map::mapped_type;
        using result_map = sub_cache::mapped_type;
        using result     = result_map::mapped_type;

        input i0;
        i0.set_type<std::string>();
        i0.change(v0);

        result r0;
        r0.set_type<double>();
        r0.change(v2);

        input_map i{{"", i0}};
        result_map r{{"", r0}};

        REQUIRE_FALSE(pmod_cache->count(i));
        pmod_cache->cache(i, r);
        REQUIRE(pmod_cache->count(i));
        REQUIRE(pmod_cache->uncache(i) == r);
        REQUIRE(pcache->uncache<int>(v0) == v1);
    }

    SECTION("count") {
        REQUIRE_FALSE(default_cache.count(v0));
        REQUIRE_FALSE(default_cache.count(v1));
        REQUIRE_FALSE(default_cache.count(v2));
        REQUIRE_FALSE(default_cache.count(v3));

        REQUIRE(pcache->count(v0));
        REQUIRE_FALSE(pcache->count(v1));
        REQUIRE_FALSE(pcache->count(v2));
        REQUIRE_FALSE(pcache->count(v3));
    }

    SECTION("cache/uncache") {
        REQUIRE_THROWS_AS(default_cache.cache(v0, v1), std::runtime_error);
        REQUIRE_THROWS_AS(default_cache.uncache<int>(v0), std::out_of_range);

        REQUIRE(pcache->uncache<int>(v0) == v1);

        // Key exists, doesn't pick optional default
        REQUIRE(pcache->uncache<int>(v0, int{99}) == v1);

        // Wrong type
        REQUIRE_THROWS_AS(pcache->uncache<float>(v0), std::runtime_error);

        // Key DNE
        REQUIRE_THROWS_AS(pcache->uncache<double>(v1), std::out_of_range);

        // Key DNE, so picks optional default
        REQUIRE(pcache->uncache<double>(v1, v2) == v2);

        pcache->cache(v2, v3);
        REQUIRE(pcache->count(v2));
        REQUIRE(pcache->uncache<std::vector<double>>(v2) == v3);
    }

    SECTION("reset_cache") {
        default_cache.reset_cache();
        REQUIRE_FALSE(default_cache.count(v0));
        REQUIRE_FALSE(default_cache.count(v1));
        REQUIRE_FALSE(default_cache.count(v2));
        REQUIRE_FALSE(default_cache.count(v3));

        pcache->reset_cache();
        REQUIRE_FALSE(pcache->count(v0));
        REQUIRE_FALSE(pcache->count(v1));
        REQUIRE_FALSE(pcache->count(v2));
        REQUIRE_FALSE(pcache->count(v3));
    }
}
