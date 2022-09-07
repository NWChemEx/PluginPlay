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
#include <pluginplay/fields/module_result.hpp>
using namespace pluginplay;

TEST_CASE("ModuleResult : default ctor") {
    ModuleResult p;
    REQUIRE_FALSE(p.has_type());
    REQUIRE_FALSE(p.has_value());
    REQUIRE_FALSE(p.has_description());
}

TEST_CASE("ModuleResult : has_type") {
    ModuleResult p;
    SECTION("No type") { REQUIRE_FALSE(p.has_type()); }
    SECTION("Has a type") {
        p.set_type<double>();
        REQUIRE(p.has_type());
    }
}

TEST_CASE("ModuleResult : has_value") {
    ModuleResult p;
    SECTION("No value") { REQUIRE_FALSE(p.has_value()); }
    SECTION("Has a value") {
        p.set_type<double>();
        p.change(double{3.14});
        REQUIRE(p.has_value());
    }
}

TEST_CASE("ModuleResult : has_description") {
    ModuleResult p;
    SECTION("No description") { REQUIRE_FALSE(p.has_description()); }
    SECTION("Description") {
        p.set_description("Hello world");
        REQUIRE(p.has_description());
    }
}

TEST_CASE("ModuleResult : set_type") {
    ModuleResult p;
    auto pp = &(p.set_type<double>());
    REQUIRE(pp == &p);
    SECTION("No value") {
        REQUIRE(p.type() == std::type_index(typeid(double)));
    }
    SECTION("Has value with same type") {
        p.change(double{3.14});
        p.set_type<double>();
    }
    SECTION("Throws if value is different type") {
        p.change(double{3.14});
        REQUIRE_THROWS_AS(p.set_type<int>(), std::runtime_error);
    }
}

TEST_CASE("ModuleResult : change") {
    ModuleResult p;
    const double v = 3.14;
    SECTION("Throws if type is not set") {
        REQUIRE_THROWS_AS(p.change(v), std::bad_optional_access);
    }
    SECTION("Throws if the value's type is wrong") {
        p.set_type<int>();
        REQUIRE_THROWS_AS(p.change(v), std::invalid_argument);
    }
    SECTION("Can set it with value") {
        p.set_type<double>();
        p.change(v);
        REQUIRE(p.value<double>() == v);
    }
    SECTION("Throws if given share_any with wrong type") {
        p.set_type<int>();
        auto any = std::make_shared<type::any>(
          pluginplay::any::make_any_field<double>(v));
        REQUIRE_THROWS_AS(p.change(any), std::invalid_argument);
    }
    SECTION("Can set with shared_any to read/write value") {
        p.set_type<double>();
        auto any = std::make_shared<type::any>(
          pluginplay::any::make_any_field<double>(v));
        p.change(any);
        REQUIRE(p.value<double>() == v);
    }
    SECTION("Can set with shared_any to read-only value") {
        p.set_type<double>();
        auto any = std::make_shared<const type::any>(
          pluginplay::any::make_any_field<double>(v));
        p.change(any);
        REQUIRE(p.value<double>() == v);
    }
}

TEST_CASE("ModuleResult : set_description") {
    ModuleResult p;
    p.set_description("Hello world");
    REQUIRE(p.description() == "Hello world");
}

TEST_CASE("ModuleResult : type") {
    ModuleResult p;
    SECTION("Throws if type is not set") {
        REQUIRE_THROWS_AS(p.type(), std::bad_optional_access);
    }
    SECTION("Works if the type is set") {
        p.set_type<double>();
        REQUIRE(p.type() == std::type_index(typeid(double)));
    }
}

TEST_CASE("ModuleResult : value") {
    ModuleResult p;
    p.set_type<double>();
    SECTION("Throws if value is not set") {
        REQUIRE_THROWS_AS(p.value<double>(), std::runtime_error);
    }
    SECTION("Throws if requested type is wrong") {
        p.change(double{3.14});
        REQUIRE_THROWS_AS(p.value<int>(), std::runtime_error);
    }
    SECTION("Can get value") {
        p.change(double{3.14});
        REQUIRE(p.value<double>() == 3.14);
    }
    SECTION("Can get value as shared_ptr") {
        p.change(double{3.14});
        auto ptr = p.value<std::shared_ptr<const type::any>>();
        REQUIRE(any::any_cast<double>(*ptr) == 3.14);
    }
}

TEST_CASE("ModuleResult : description") {
    ModuleResult p;
    SECTION("Throws if description is not set") {
        REQUIRE_THROWS_AS(p.description(), std::bad_optional_access);
    }
    SECTION("has a description") {
        p.set_description("Hello world");
        REQUIRE(p.description() == "Hello world");
    }
}

TEST_CASE("ModuleResult : comparisons") {
    ModuleResult p, p2;

    SECTION("Both empty") {
        REQUIRE(p == p2);
        REQUIRE_FALSE(p != p2);
    }
    SECTION("Different typed-ness") {
        p.set_type<double>();
        REQUIRE(p != p2);
        REQUIRE_FALSE(p == p2);
    }
    SECTION("Different types") {
        p.set_type<double>();
        p2.set_type<int>();
        REQUIRE(p != p2);
        REQUIRE_FALSE(p == p2);
    }
    SECTION("Different valued-ness") {
        p.set_type<double>();
        p2.set_type<double>();
        p.change(double{3.14});
        REQUIRE(p != p2);
        REQUIRE_FALSE(p == p2);
    }
    SECTION("Different values") {
        p.set_type<double>();
        p2.set_type<double>();
        p.change(double{3.14});
        p2.change(double{1.23});
        REQUIRE(p != p2);
        REQUIRE_FALSE(p == p2);
    }
    SECTION("Different description-ness") {
        p.set_description("Hello world");
        REQUIRE(p != p2);
        REQUIRE_FALSE(p == p2);
    }
    SECTION("Different descriptions") {
        p.set_description("Hello world");
        p2.set_description("Bye world");
        REQUIRE(p != p2);
        REQUIRE_FALSE(p == p2);
    }
}

TEST_CASE("ModuleResult : copy ctor") {
    ModuleResult p;
    p.set_type<double>();
    ModuleResult p2(p);
    REQUIRE(p == p2);
}

TEST_CASE("ModuleResult : copy assignment") {
    ModuleResult p, p2;
    p2.set_type<double>();
    auto pp = &(p = p2);
    REQUIRE(pp == &p);
    REQUIRE(p == p2);
}

TEST_CASE("ModuleResult : move ctor") {
    ModuleResult p;
    p.set_type<double>();
    ModuleResult p2(p);
    ModuleResult p3(std::move(p2));
    REQUIRE(p == p3);
}

TEST_CASE("ModuleResult : move assignment") {
    ModuleResult p, p2;
    p2.set_type<double>();
    ModuleResult p3(p2);
    auto pp = &(p = std::move(p2));
    REQUIRE(pp == &p);
    REQUIRE(p == p3);
}
