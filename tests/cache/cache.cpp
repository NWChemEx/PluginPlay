#include "sde/cache/cache.hpp"
#include <catch2/catch.hpp>

using namespace sde;
using sde::hash_objects;

TEST_CASE("Cache Class") {
    Cache c;

    SECTION("Typedefs") {
        using hash_type = typename Cache::hash_type;
        STATIC_REQUIRE(std::is_same_v<hash_type, std::string>);
    }

    SECTION("Count(key)") {
        SECTION("Doesn't have the value") { REQUIRE_FALSE(c.count(int{4})); }

        SECTION("Has value") {
            c.cache(int{4}, 42);
            REQUIRE(c.count(int{4}));
        }
    }

    SECTION("Count(hash)") {
        typename Cache::hash_type da_hash{"Hello World!!!!"};
        SECTION("Doesn't have the value") { REQUIRE_FALSE(c.count(da_hash)); }

        SECTION("Has value : value inserted via hash") {
            c.cache(da_hash, 42);
            REQUIRE(c.count(da_hash));
        }

        SECTION("Has value : value inserted via key") {
            c.cache(int{4}, 42);
            REQUIRE(c.count(hash_objects(int{4})));
        }
    }

    SECTION("cache/uncache") {
        SECTION("POD") {
            c.cache(int{4}, int{42});
            REQUIRE(c.uncache<int>(int{4}) == int{42});
            REQUIRE(std::as_const(c).uncache<int>(int{4}) == int{42});
            auto hv = hash_objects(int{4});
            REQUIRE(c.uncache<int>(hv) == int{42});
            REQUIRE(std::as_const(c).uncache<int>(hv) == int{42});
        }

        SECTION("More complicated by copy") {
            using vector_type = std::vector<int>;
            vector_type data{1, 2, 3, 4};
            c.cache(int{4}, data);
            SECTION("Retrieve by key") {
                auto& cached_data = c.uncache<vector_type&>(int{4});
                REQUIRE(cached_data == data);
                REQUIRE(cached_data.data() != data.data());
            }

            SECTION("Retrieve by hash") {
                auto hv           = hash_objects(int{4});
                auto& cached_data = c.uncache<vector_type&>(hv);
                REQUIRE(cached_data == data);
                REQUIRE(cached_data.data() != data.data());
            }

            SECTION("Retrieve from const object by key") {
                const auto& cached_data =
                  std::as_const(c).uncache<const vector_type&>(int{4});
                REQUIRE(cached_data == data);
                REQUIRE(cached_data.data() != data.data());
            }

            SECTION("Retrieve from const object by hash") {
                auto hv = hash_objects(int{4});
                const auto& cached_data =
                  std::as_const(c).uncache<const vector_type&>(hv);
                REQUIRE(cached_data == data);
                REQUIRE(cached_data.data() != data.data());
            }
        }

        SECTION("More complicated by move") {
            using vector_type = std::vector<int>;
            vector_type data{1, 2, 3, 4};
            auto pdata = data.data();
            c.cache(int{4}, std::move(data));

            SECTION("Retrieve by key") {
                auto& cached_data = c.uncache<vector_type&>(int{4});
                REQUIRE(cached_data == vector_type{1, 2, 3, 4});
                REQUIRE(cached_data.data() == pdata);
            }

            SECTION("Retrieve by hash") {
                auto hv           = hash_objects(int{4});
                auto& cached_data = c.uncache<vector_type&>(hv);
                REQUIRE(cached_data == vector_type{1, 2, 3, 4});
                REQUIRE(cached_data.data() == pdata);
            }

            SECTION("Retrieve from const by key") {
                const auto& cached_data =
                  std::as_const(c).uncache<const vector_type&>(int{4});
                REQUIRE(cached_data == vector_type{1, 2, 3, 4});
                REQUIRE(cached_data.data() == pdata);
            }

            SECTION("Retrieve from const by hash") {
                auto hv = hash_objects(int{4});
                const auto& cached_data =
                  std::as_const(c).uncache<const vector_type&>(hv);
                REQUIRE(cached_data == vector_type{1, 2, 3, 4});
                REQUIRE(cached_data.data() == pdata);
            }
        }

        SECTION("Throws if key DNE") {
            using except = std::out_of_range;
            REQUIRE_THROWS_AS(c.uncache<int>(int{42}), except);
            REQUIRE_THROWS_AS(std::as_const(c).uncache<int>(int{42}), except);
            auto hv = hash_objects(int{42});
            REQUIRE_THROWS_AS(c.uncache<int>(hv), except);
            REQUIRE_THROWS_AS(std::as_const(c).uncache<int>(int{42}), except);
        }

        SECTION("With default values") {
            c.cache(int{4}, int{42});
            SECTION("Normal Behavior") {
                REQUIRE(c.uncache<int>(int{4}, int{43}) == int{42});
                REQUIRE(std::as_const(c).uncache<int>(int{4}, int{43}) ==
                        int{42});
                auto hv = hash_objects(int{4});
                REQUIRE(c.uncache<int>(hv, int{43}) == int{42});
                REQUIRE(std::as_const(c).uncache<int>(hv, int{43}) == int{42});
            }
            SECTION("Defaulting") {
                REQUIRE(c.uncache<int>(int{5}, int{43}) == int{43});
                REQUIRE(std::as_const(c).uncache<int>(int{5}, int{43}) ==
                        int{43});
                auto hv = hash_objects(int{5});
                REQUIRE(c.uncache<int>(hv, int{43}) == int{43});
                REQUIRE(std::as_const(c).uncache<int>(hv, int{43}) == int{43});
            }
        }
    }
}