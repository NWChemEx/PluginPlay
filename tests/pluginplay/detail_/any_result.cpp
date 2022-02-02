#include "pluginplay/detail_/any_result.hpp"
#include "pluginplay/hasher.hpp"
#include <catch2/catch.hpp>
#include <map>
#include <typeindex>
#include <vector>

using namespace pluginplay::detail_;

TEST_CASE("AnyResult : Default ctor") {
    AnyResult a;
    REQUIRE_FALSE(a.has_value());
    REQUIRE(a.type() == typeid(void));
    REQUIRE(pluginplay::hash_objects(a) == "cbc357ccb763df2852fee8c4fc7d55f2");
    REQUIRE_THROWS_AS(a.cast<double>(), std::bad_any_cast);
    REQUIRE(a.str() == "<empty AnyResult>");
}

TEST_CASE("AnyResult : Value ctor") {
    SECTION("POD by value") {
        AnyResult a(int{3});
        REQUIRE(a.has_value());
        REQUIRE(a.type() == typeid(int));
        REQUIRE(pluginplay::hash_objects(a) ==
                "9a4294b64e60cc012c5ed48db4cd9c48");
        REQUIRE(a.cast<int>() == 3);
        REQUIRE(a.str() == "3");
    }
    SECTION("Serialize any_result") {
        std::stringstream ss;

        AnyResult a(int{3}), a2;
        // {
        //     pluginplay::BinaryOutputArchive ar(ss);
        //     ar& a;
        // }

        // {
        //     pluginplay::BinaryInputArchive ar(ss);
        //     ar& a2;
        // }
        //REQUIRE(a == a2);
    }
    SECTION("POD by reference") {
        int x{3};
        AnyResult a(x);
        REQUIRE(a.has_value());
        REQUIRE(a.type() == typeid(int));
        REQUIRE(pluginplay::hash_objects(a) ==
                "9a4294b64e60cc012c5ed48db4cd9c48");
        REQUIRE(a.cast<int>() == 3);
        REQUIRE(a.str() == "3");
    }
    SECTION("POD by const reference") {
        const int x{3};
        AnyResult a(x);
        REQUIRE(a.has_value());
        REQUIRE(a.type() == typeid(const int));
        REQUIRE(pluginplay::hash_objects(a) ==
                "9a4294b64e60cc012c5ed48db4cd9c48");
        REQUIRE(a.cast<int>() == 3);
        REQUIRE(a.str() == "3");
    }
    SECTION("Non-POD by move") {
        std::vector<int> x{1, 2, 3, 4};
        auto px = x.data();
        AnyResult a(std::move(x));
        REQUIRE(a.has_value());
        REQUIRE(a.type() == typeid(std::vector<int>));
        REQUIRE(pluginplay::hash_objects(a) ==
                "ad06a09d17cceb43c8d7f0283f889ef6");
        REQUIRE(a.cast<std::vector<int>&>().data() == px);
        REQUIRE(a.str() == "[1, 2, 3, 4]");
    }
}

TEST_CASE("AnyResult : Comparisons") {
    AnyResult a(int{3});
    SECTION("Two empty any_results") {
        AnyResult a2, a3;
        REQUIRE(a2 == a3);
        REQUIRE_FALSE(a2 != a3);
    }
    SECTION("Same any_result") {
        AnyResult a2(int{3});
        REQUIRE(a == a2);
        REQUIRE_FALSE(a != a2);
    }
    SECTION("Different full-ness") {
        AnyResult a2;
        REQUIRE_FALSE(a == a2);
        REQUIRE(a != a2);
    }
    SECTION("Different values") {
        AnyResult a2(int{4});
        REQUIRE_FALSE(a == a2);
        REQUIRE(a != a2);
    }
    SECTION("Different types") {
        AnyResult a2(std::vector<int>{3, 4, 5});
        REQUIRE_FALSE(a == a2);
        REQUIRE(a != a2);
    }
}

TEST_CASE("AnyResult : emplace") {
    std::vector<int> x{1, 2, 3, 4};
    AnyResult a(x), a2;
    a2.emplace<std::vector<int>>(x);
    REQUIRE(a == a2);
}

TEST_CASE("AnyResult : reset") {
    AnyResult a(int{3});
    a.reset();
    REQUIRE(a == AnyResult{});
}

TEST_CASE("AnyResult : swap") {
    AnyResult a(int{3}), a2;
    a.swap(a2);
    REQUIRE(a == AnyResult{});
    REQUIRE(a2 == AnyResult{int{3}});
}

TEST_CASE("AnyResult : cast") {
    SECTION("non-const AnyResult") {
        AnyResult a(int{3});
        REQUIRE(a.cast<int>() == 3);
        REQUIRE(a.cast<int&>() == 3);
        REQUIRE(a.cast<const int&>() == 3);
        REQUIRE_THROWS_AS(a.cast<double>(), std::bad_any_cast);
    }
    SECTION("const AnyResult") {
        const AnyResult a(int{3});
        REQUIRE(a.cast<int>() == 3);
        // a.cast<int&>() //Should trip a static assert if uncommented
        REQUIRE(a.cast<const int&>() == 3);
        REQUIRE_THROWS_AS(a.cast<double>(), std::bad_any_cast);
    }
}

TEST_CASE("AnyResult : Copy ctor") {
    AnyResult a(int{3});
    AnyResult a2(a);
    REQUIRE(a == a2);
}

TEST_CASE("AnyResult : Copy assignment") {
    AnyResult a(int{3});
    AnyResult a2;
    auto pa2 = &(a2 = a);
    REQUIRE(a == a2);
    REQUIRE(pa2 == &a2);
}

TEST_CASE("AnyResult : Move ctor") {
    AnyResult a(int{3});
    AnyResult a2(a);
    AnyResult a3(std::move(a));
    REQUIRE(a2 == a3);
}

TEST_CASE("AnyResult : Move assignment") {
    AnyResult a(int{3});
    AnyResult a2(a);
    AnyResult a3;
    auto pa3 = &(a3 = std::move(a));
    REQUIRE(a2 == a3);
    REQUIRE(pa3 == &a3);
}

TEST_CASE("AnyResult : is_convertible") {
    AnyResult a{int{3}};
    REQUIRE(a.is_convertible<int>());
    REQUIRE_FALSE(a.is_convertible<double>());
}

TEST_CASE("AnyResultCast") {
    AnyResult a{int{3}};
    REQUIRE(&a.cast<int&>() == &AnyResultCast<int&>(a));
}

TEST_CASE("make_AnyResult") {
    AnyResult a{int{3}};
    REQUIRE(a == make_AnyResult<int>(3));
}
