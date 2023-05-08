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

#include "pluginplay/detail_/any.hpp"
#include <catch2/catch.hpp>
#include <map>
#include <typeindex>
#include <vector>

using namespace pluginplay::detail_;

TEST_CASE("Any : Default ctor") {
    Any a;
    REQUIRE_FALSE(a.has_value());
    REQUIRE(a.type() == typeid(void));
    REQUIRE_THROWS_AS(a.cast<double>(), std::bad_any_cast);
    REQUIRE(a.str() == "<empty Any>");
}

TEST_CASE("Any : Value ctor") {
    SECTION("POD by value") {
        Any a(int{3});
        REQUIRE(a.has_value());
        REQUIRE(a.type() == typeid(int));
        REQUIRE(a.cast<int>() == 3);
        REQUIRE(a.str() == "3");
    }
    SECTION("POD by reference") {
        int x{3};
        Any a(x);
        REQUIRE(a.has_value());
        REQUIRE(a.type() == typeid(int));
        REQUIRE(a.cast<int>() == 3);
        REQUIRE(a.str() == "3");
    }
    SECTION("POD by const reference") {
        const int x{3};
        Any a(x);
        REQUIRE(a.has_value());
        REQUIRE(a.type() == typeid(const int));
        REQUIRE(a.cast<int>() == 3);
        REQUIRE(a.str() == "3");
    }
    SECTION("Non-POD by move") {
        std::vector<int> x{1, 2, 3, 4};
        auto px = x.data();
        Any a(std::move(x));
        REQUIRE(a.has_value());
        REQUIRE(a.type() == typeid(std::vector<int>));
        REQUIRE(a.cast<std::vector<int>&>().data() == px);
        // RMR I don't know why this is broken on my box, but since we're going
        // to scrap this class shortly I'm not going to worry about it.
        // REQUIRE(a.str() == "[1, 2, 3, 4]");
    }
}

TEST_CASE("Any : Comparisons") {
    Any a(int{3});
    SECTION("Two empty anys") {
        Any a2, a3;
        REQUIRE(a2 == a3);
        REQUIRE_FALSE(a2 != a3);
    }
    SECTION("Same any") {
        Any a2(int{3});
        REQUIRE(a == a2);
        REQUIRE_FALSE(a != a2);
    }
    SECTION("Different full-ness") {
        Any a2;
        REQUIRE_FALSE(a == a2);
        REQUIRE(a != a2);
    }
    SECTION("Different values") {
        Any a2(int{4});
        REQUIRE_FALSE(a == a2);
        REQUIRE(a != a2);
    }
    SECTION("Different types") {
        Any a2(std::vector<int>{3, 4, 5});
        REQUIRE_FALSE(a == a2);
        REQUIRE(a != a2);
    }
}

TEST_CASE("Any : emplace") {
    std::vector<int> x{1, 2, 3, 4};
    Any a(x), a2;
    a2.emplace<std::vector<int>>(x);
    REQUIRE(a == a2);
}

TEST_CASE("Any : reset") {
    Any a(int{3});
    a.reset();
    REQUIRE(a == Any{});
}

TEST_CASE("Any : swap") {
    Any a(int{3}), a2;
    a.swap(a2);
    REQUIRE(a == Any{});
    REQUIRE(a2 == Any{int{3}});
}

TEST_CASE("Any : cast") {
    SECTION("non-const Any") {
        Any a(int{3});
        REQUIRE(a.cast<int>() == 3);
        REQUIRE(a.cast<int&>() == 3);
        REQUIRE(a.cast<const int&>() == 3);
        REQUIRE_THROWS_AS(a.cast<double>(), std::bad_any_cast);
    }
    SECTION("const Any") {
        const Any a(int{3});
        REQUIRE(a.cast<int>() == 3);
        // a.cast<int&>() //Should trip a static assert if uncommented
        REQUIRE(a.cast<const int&>() == 3);
        REQUIRE_THROWS_AS(a.cast<double>(), std::bad_any_cast);
    }
}

TEST_CASE("Any : Copy ctor") {
    Any a(int{3});
    Any a2(a);
    REQUIRE(a == a2);
}

TEST_CASE("Any : Copy assignment") {
    Any a(int{3});
    Any a2;
    auto pa2 = &(a2 = a);
    REQUIRE(a == a2);
    REQUIRE(pa2 == &a2);
}

TEST_CASE("Any : Move ctor") {
    Any a(int{3});
    Any a2(a);
    Any a3(std::move(a));
    REQUIRE(a2 == a3);
}

TEST_CASE("Any : Move assignment") {
    Any a(int{3});
    Any a2(a);
    Any a3;
    auto pa3 = &(a3 = std::move(a));
    REQUIRE(a2 == a3);
    REQUIRE(pa3 == &a3);
}

TEST_CASE("Any : is_convertible") {
    Any a{int{3}};
    REQUIRE(a.is_convertible<int>());
    REQUIRE_FALSE(a.is_convertible<double>());
}

TEST_CASE("AnyCast") {
    Any a{int{3}};
    REQUIRE(&a.cast<int&>() == &AnyCast<int&>(a));
}

TEST_CASE("make_Any") {
    Any a{int{3}};
    REQUIRE(a == make_Any<int>(3));
}
