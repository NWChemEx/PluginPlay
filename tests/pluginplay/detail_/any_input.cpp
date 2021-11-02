#include "pluginplay/detail_/any_input.hpp"
#include "pluginplay/hasher.hpp"
#include <catch2/catch.hpp>
#include <map>
#include <typeindex>
#include <vector>

using namespace pluginplay::detail_;

TEST_CASE("AnyInput : Default ctor") {
    AnyInput a;
    REQUIRE_FALSE(a.has_value());
    REQUIRE(a.type() == typeid(void));
    REQUIRE(pluginplay::hash_objects(a) == "cbc357ccb763df2852fee8c4fc7d55f2");
    REQUIRE_THROWS_AS(a.cast<double>(), std::bad_any_cast);
    REQUIRE(a.str() == "<empty AnyInput>");
}

TEST_CASE("AnyInput : Value ctor") {
    SECTION("POD by value") {
        AnyInput a(int{3});
        REQUIRE(a.has_value());
        REQUIRE(a.type() == typeid(int));
        REQUIRE(pluginplay::hash_objects(a) ==
                "9a4294b64e60cc012c5ed48db4cd9c48");
        REQUIRE(a.cast<int>() == 3);
        REQUIRE(a.str() == "3");
    }
    SECTION("POD by reference") {
        int x{3};
        AnyInput a(x);
        REQUIRE(a.has_value());
        REQUIRE(a.type() == typeid(int));
        REQUIRE(pluginplay::hash_objects(a) ==
                "9a4294b64e60cc012c5ed48db4cd9c48");
        REQUIRE(a.cast<int>() == 3);
        REQUIRE(a.str() == "3");
    }
    SECTION("POD by const reference") {
        const int x{3};
        AnyInput a(x);
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
        AnyInput a(std::move(x));
        REQUIRE(a.has_value());
        REQUIRE(a.type() == typeid(std::vector<int>));
        REQUIRE(pluginplay::hash_objects(a) ==
                "ad06a09d17cceb43c8d7f0283f889ef6");
        REQUIRE(a.cast<std::vector<int>&>().data() == px);
        REQUIRE(a.str() == "[1, 2, 3, 4]");
    }
}

TEST_CASE("AnyInput : Comparisons") {
    AnyInput a(int{3});
    SECTION("Two empty anys") {
        AnyInput a2, a3;
        REQUIRE(a2 == a3);
        REQUIRE_FALSE(a2 != a3);
    }
    SECTION("Same any") {
        AnyInput a2(int{3});
        REQUIRE(a == a2);
        REQUIRE_FALSE(a != a2);
    }
    SECTION("Different full-ness") {
        AnyInput a2;
        REQUIRE_FALSE(a == a2);
        REQUIRE(a != a2);
    }
    SECTION("Different values") {
        AnyInput a2(int{4});
        REQUIRE_FALSE(a == a2);
        REQUIRE(a != a2);
    }
    SECTION("Different types") {
        AnyInput a2(std::vector<int>{3, 4, 5});
        REQUIRE_FALSE(a == a2);
        REQUIRE(a != a2);
    }
}

TEST_CASE("AnyInput : emplace") {
    std::vector<int> x{1, 2, 3, 4};
    AnyInput a(x), a2;
    a2.emplace<std::vector<int>>(x);
    REQUIRE(a == a2);
}

TEST_CASE("AnyInput : reset") {
    AnyInput a(int{3});
    a.reset();
    REQUIRE(a == AnyInput{});
}

TEST_CASE("AnyInput : swap") {
    AnyInput a(int{3}), a2;
    a.swap(a2);
    REQUIRE(a == AnyInput{});
    REQUIRE(a2 == AnyInput{int{3}});
}

TEST_CASE("AnyInput : cast") {
    SECTION("non-const AnyInput") {
        AnyInput a(int{3});
        REQUIRE(a.cast<int>() == 3);
        REQUIRE(a.cast<int&>() == 3);
        REQUIRE(a.cast<const int&>() == 3);
        REQUIRE_THROWS_AS(a.cast<double>(), std::bad_any_cast);
    }
    SECTION("const AnyInput") {
        const AnyInput a(int{3});
        REQUIRE(a.cast<int>() == 3);
        // a.cast<int&>() //Should trip a static assert if uncommented
        REQUIRE(a.cast<const int&>() == 3);
        REQUIRE_THROWS_AS(a.cast<double>(), std::bad_any_cast);
    }
}

TEST_CASE("AnyInput : Copy ctor") {
    AnyInput a(int{3});
    AnyInput a2(a);
    REQUIRE(a == a2);
}

TEST_CASE("AnyInput : Copy assignment") {
    AnyInput a(int{3});
    AnyInput a2;
    auto pa2 = &(a2 = a);
    REQUIRE(a == a2);
    REQUIRE(pa2 == &a2);
}

TEST_CASE("AnyInput : Move ctor") {
    AnyInput a(int{3});
    AnyInput a2(a);
    AnyInput a3(std::move(a));
    REQUIRE(a2 == a3);
}

TEST_CASE("AnyInput : Move assignment") {
    AnyInput a(int{3});
    AnyInput a2(a);
    AnyInput a3;
    auto pa3 = &(a3 = std::move(a));
    REQUIRE(a2 == a3);
    REQUIRE(pa3 == &a3);
}

TEST_CASE("AnyInput : is_convertible") {
    AnyInput a{int{3}};
    REQUIRE(a.is_convertible<int>());
    REQUIRE_FALSE(a.is_convertible<double>());
}

TEST_CASE("AnyInputCast") {
    AnyInput a{int{3}};
    REQUIRE(&a.cast<int&>() == &AnyInputCast<int&>(a));
}

TEST_CASE("make_AnyInput") {
    AnyInput a{int{3}};
    REQUIRE(a == make_AnyInput<int>(3));
}
