#include <SDE/attic/ComputedProperties.hpp>
#include <catch2/catch.hpp>

using namespace SDE;

template<typename T>
void check_state(ComputedProperties& map,
                 std::map<std::string, Property>&corr) {
    SECTION("Overall State") {
        REQUIRE(map.size() == corr.size());
        REQUIRE(map.count("Not a key") == 0);
    }
    SECTION("Individual Elements") {
        for (auto& [k, v] : map) {
            SECTION("Is supposed to have the element"){
                REQUIRE(corr.count(k));
            }
            SECTION("Has element") {
                REQUIRE(map.count(k));
            }
            SECTION("Exists so emplace does nothing") {
                auto[itr, did_not_have] = map.emplace(k, v);
                REQUIRE(!did_not_have);
            }
            SECTION("Correct value") {
                REQUIRE(corr.at(k) == v);
            }
            SECTION("Access via at"){
                SECTION("Right value") {
                    REQUIRE(v == map.at(k));
                }
                SECTION("Is by reference") {
                    REQUIRE(&v == &(map.at(k)));
                }
            }
            SECTION("Access via value"){
                SECTION("Right value") {
                    REQUIRE(v.template value<T>() == map.value<T>(k));
                }
                SECTION("Is by reference") {
                    REQUIRE(&(v.template value<T>()) == &(map.value<T>(k)));
                }
            }
        }
    }
}


TEST_CASE("ComputedProperties Class") {
    ComputedProperties map;
    std::map<std::string, Property> corr;

    SECTION("Default Ctor") {
        check_state<nullptr_t>(map, corr);
    }

    Property two(int{2});
    corr["test1"] = two;

    SECTION("Adding a value...") {
        SECTION("...with emplace") {
            auto[itr, did_not_have] = map.emplace("test1", two);
            REQUIRE(did_not_have);
            check_state<int>(map, corr);
        }
        SECTION("...with []"){
            map["test1"] = two;
            check_state<int>(map, corr);
        }
    }

    map.emplace("test1", two);

    SECTION("Copy constructor"){
        ComputedProperties map2(map);
        check_state<int>(map2, corr);
    }
    SECTION("Copy Assignment"){
        ComputedProperties map2;
        auto& pmap2 = (map2 = map);
        check_state<int>(map2, corr);
        SECTION("Allows chaining"){
            REQUIRE(&pmap2 == &map2);
        }
    }
    SECTION("Move constructor"){
        ComputedProperties map2(std::move(map));
        check_state<int>(map2, corr);
    }
    SECTION("Move Assignment"){
        ComputedProperties map2;
        auto& pmap2 = (map2 = std::move(map));
        check_state<int>(map2, corr);
        SECTION("Allows chaining"){
            REQUIRE(&pmap2 == &map2);
        }
    }
}
