#include <catch2/catch.hpp>
#include <pluginplay/cache/module_manager_cache.hpp>

using namespace pluginplay::cache;

/* Testing Strategy:
 *
 * As far as the user is concerned ModuleManagerCache instances are just maps
 * from keys to ModuleCache and UserCache instances. Here we just test that
 * those mapping features work. Other unit tests are responsible for ensuring
 * that the ModuleCache and UserCache instances work as intended.
 */

TEST_CASE("ModuleManagerCache") {
    ModuleManagerCache memory_only;

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

        // Returns the already existing module cache
        auto pcache2 = memory_only.get_or_make_user_cache("hello");
        REQUIRE(pcache.get() == pcache2.get());
    }
}
