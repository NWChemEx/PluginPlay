#include <SDE/detail_/IOMap.hpp>
#include <catch2/catch.hpp>

using namespace SDE::detail_;

//Make a class satisfying the element API
struct DummyElement {
    template<typename>
    const int& value() const { return value_; }
    int value_;
};

//Make a dummy class to bring ctors into scope
struct DummyMap : public IOMap<DummyElement> {
    using IOMap<DummyElement>::IOMap;
};

void check_state(DummyMap& map, std::map<std::string, int>& corr) {
    SECTION("Overall State") {
        REQUIRE(map.size() == corr.size());
        REQUIRE(map.count("Not a key") == 0);
    }
    SECTION("Individual Elements") {
        for (auto&[k, v] : map) {
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
                REQUIRE(corr.at(k) == v.value_);
            }
            SECTION("Access via at"){
                SECTION("Right value") {
                    REQUIRE(v.value_ == map.at(k).value_);
                }
                SECTION("Is by reference") {
                    REQUIRE(&(v.value_) == &(map.at(k).value_));
                }
            }
            SECTION("Access via value"){
                SECTION("Right value") {
                    REQUIRE(v.value_ == map.value<int>(k));
                }
                SECTION("Is by reference") {
                    REQUIRE(&(v.value_) == &(map.value<int>(k)));
                }
            }
        }
    }
}


TEST_CASE("IOMap Class") {
    DummyMap map;
    std::map<std::string, int> corr;

    SECTION("Default Ctor") {
        check_state(map, corr);
    }

    corr["test1"] = 2;

    SECTION("Adding a value...") {
        SECTION("...with emplace") {
            auto[itr, did_not_have] = map.emplace("test1", DummyElement{2});
            REQUIRE(did_not_have);
            check_state(map, corr);
        }
        SECTION("...with []"){
            map["test1"] = DummyElement{2};
            check_state(map, corr);
        }
    }

    map.emplace("test1", DummyElement{2});

    SECTION("Copy constructor"){
        DummyMap map2(map);
        check_state(map2, corr);
    }
    SECTION("Copy Assignment"){
        DummyMap map2;
        auto& pmap2 = (map2 = map);
        check_state(map2, corr);
        SECTION("Allows chaining"){
            REQUIRE(&pmap2 == &map2);
        }
    }
    SECTION("Move constructor"){
        DummyMap map2(std::move(map));
        check_state(map2, corr);
    }
    SECTION("Move Assignment"){
        DummyMap map2;
        auto& pmap2 = (map2 = std::move(map));
        check_state(map2, corr);
        SECTION("Allows chaining"){
            REQUIRE(&pmap2 == &map2);
        }
    }
}
