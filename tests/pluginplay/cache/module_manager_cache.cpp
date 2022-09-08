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
#include <pluginplay/cache/module_manager_cache.hpp>
#include <pluginplay/config/config.hpp>
using namespace pluginplay::cache;

/* Testing Strategy:
 *
 * As far as the user is concerned ModuleManagerCache instances are just maps
 * from keys to ModuleCache and UserCache instances. Here we just test that
 * those mapping features work. Other unit tests are responsible for ensuring
 * that the ModuleCache and UserCache instances work as intended.
 */

TEST_CASE("ModuleManagerCache") {
    auto root_dir   = std::filesystem::temp_directory_path();
    auto cache_dir  = std::filesystem::path("mmcache_test");
    auto cache_path = root_dir / cache_dir;

    ModuleManagerCache memory_only;

    SECTION("Path CTor") {
        if(std::filesystem::exists(cache_path))
            std::filesystem::remove_all(cache_path);

        if(pluginplay::with_rocksdb()) {
            // Cache shouldn't exist yet
            REQUIRE_FALSE(std::filesystem::exists(cache_path));

            ModuleManagerCache disk(cache_path);

            // Make sure it exists now
            REQUIRE(std::filesystem::exists(cache_path));

        } else {
            using e = std::runtime_error;
            REQUIRE_THROWS_AS(ModuleManagerCache(cache_path), e);
        }
    }

    SECTION("change_save_location") {
        if(std::filesystem::exists(cache_path))
            std::filesystem::remove_all(cache_path);

        REQUIRE_FALSE(std::filesystem::exists(cache_path));

        if(pluginplay::with_rocksdb()) {
            memory_only.change_save_location(cache_path.string());
            REQUIRE(std::filesystem::exists(cache_path));

            // The parent dir (root_dir/cache_dir/cache_dir) DNE, this tests we
            // can still make the child dir
            auto multiple_nestings =
              root_dir / cache_dir / cache_dir / cache_dir;
            memory_only.change_save_location(multiple_nestings);
            REQUIRE(std::filesystem::exists(multiple_nestings));

            std::filesystem::remove_all(multiple_nestings);
        } else {
            using e = std::runtime_error;
            REQUIRE_THROWS_AS(memory_only.change_save_location(cache_path), e);
        }
    }

    SECTION("get_or_make_module_cache") {
        auto pcache = memory_only.get_or_make_module_cache("hello");

        // New key, new cache
        auto pcache1 = memory_only.get_or_make_module_cache("world");
        REQUIRE(pcache.get() != pcache1.get());

        // Returns the already existing module cache
        auto pcache2 = memory_only.get_or_make_module_cache("hello");
        REQUIRE(pcache.get() == pcache2.get());
    }

    SECTION("get_or_make_user_cache") {
        auto pcache = memory_only.get_or_make_user_cache("hello");

        // New key, new cache
        auto pcache1 = memory_only.get_or_make_user_cache("world");
        REQUIRE(pcache.get() != pcache1.get());

        // Returns the already existing user cache
        auto pcache2 = memory_only.get_or_make_user_cache("hello");
        REQUIRE(pcache.get() == pcache2.get());
    }
}
