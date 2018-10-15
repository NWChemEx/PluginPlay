#include <SDE/SDEAny.hpp>
#include <SDE/Cache.hpp>
#include <SDE/Memoization.hpp>
#include <catch/catch.hpp>


const std::string make_hash(double val)
{
    bphash::Hasher h(bphash::HashType::Hash128);
    h(val);
    return bphash::hash_to_string(h.finalize());
}



using namespace SDE;

TEST_CASE("Cache Class")
{
    double val1 = 3.1415;
    auto key1 = make_hash(val1);

    double val2 = 2.7183;
    auto key2 = make_hash(val2);

    SECTION("Insert")
    {
        Cache cache1;
        cache1.insert(key1, val1);
        REQUIRE(cache1.count(key1) == 1);
        REQUIRE(cache1.count("some key") == 0);

        Cache cache2;
        cache2.insert(val1);
        REQUIRE(cache2.count(key1) == 1);
        REQUIRE(cache1.count("some key") == 0);
    }

    SECTION("At")
    {
        Cache cache1;
        cache1.insert(key1, val1);
        double ref_val1 = 3.1415;
        REQUIRE(*(cache1.at<double>(key1)) == ref_val1);

        REQUIRE_THROWS_AS(cache1.at<int>(key1), std::bad_cast);
    }

    SECTION("Reference Counting")
    {
        Cache cache1;
        REQUIRE(cache1.get_use_count(key1) == 0);
        cache1.insert(key1, val1);
        REQUIRE(cache1.get_use_count(key1) == 1);
        auto valAny1 = cache1.at<double>(key1);
        REQUIRE(valAny1.use_count() == 2);
        {
            auto valAny2 = cache1.at<double>(key1);
            REQUIRE(valAny1.use_count() == 3);
            REQUIRE(valAny1.use_count() == valAny2.use_count());
        }
        REQUIRE(valAny1.use_count() == 2);
        REQUIRE(valAny1.use_count() == cache1.get_use_count(key1));
        valAny1.reset();
        REQUIRE(valAny1.use_count() == 0);
        REQUIRE(cache1.get_use_count(key1) == 1);
        cache1.erase(key1);
        REQUIRE(cache1.get_use_count(key1) == 0);
    }

    SECTION("Iterators")
    {
        Cache cache1;
        REQUIRE(cache1.begin() == cache1.end());
        cache1.insert(key1, val1);
        REQUIRE(cache1.begin() != cache1.end());
        REQUIRE(*(cache1.at<double>(cache1.begin())) == val1);
        REQUIRE_THROWS_AS(cache1.at<int>(cache1.begin()), std::bad_cast);

        auto it = cache1.find(key1);
        cache1.erase(it);
        REQUIRE(cache1.begin() == cache1.end());
    }

    SECTION("Synchronize")
    {
        Cache cache1;
        Cache cache2;
        cache1.insert(key1, val1);
        cache2.insert(key1, val1);

        REQUIRE(cache1 == cache2);

        cache2.insert(val2);
        REQUIRE(cache1 != cache2);

        cache1.synchronize(cache2);

        REQUIRE(cache1 == cache2);

        double val3 = 1.6605;
        cache1.insert(val3);
        cache1.synchronize(cache2);
        REQUIRE(cache1 != cache2);
        cache2.synchronize(cache1);
        REQUIRE(cache1 == cache2);

    }
}