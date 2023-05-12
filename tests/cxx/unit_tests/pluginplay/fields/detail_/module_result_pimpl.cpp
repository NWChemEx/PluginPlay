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
#include <pluginplay/fields/detail_/module_result_pimpl.hpp>
using namespace pluginplay;
using namespace pluginplay::detail_;
using shared_any = typename ModuleResultPIMPL::shared_any;

template<typename T, typename U>
static auto wrap_value(U&& value) {
    return std::make_shared<const type::any>(
      any::make_any_field<T>(std::forward<U>(value)));
}

static const std::type_index d(typeid(double));

TEST_CASE("ModuleResultPIMPL : default ctor") {
    ModuleResultPIMPL p;
    REQUIRE_FALSE(p.has_type());
    REQUIRE_FALSE(p.has_value());
    REQUIRE_FALSE(p.has_description());
}

TEST_CASE("ModuleResultPIMPL : has_type") {
    ModuleResultPIMPL p;
    SECTION("No type") { REQUIRE_FALSE(p.has_type()); }
    SECTION("Has a type") {
        p.set_type(d);
        REQUIRE(p.has_type());
    }
}

TEST_CASE("ModuleResultPIMPL : has_value") {
    ModuleResultPIMPL p;
    SECTION("No value") { REQUIRE_FALSE(p.has_value()); }
    SECTION("Has a value") {
        p.set_type(d);
        p.set_value(wrap_value<double>(3.14));
        REQUIRE(p.has_value());
    }
}

TEST_CASE("ModuleResultPIMPL : has_description") {
    ModuleResultPIMPL p;
    SECTION("No description") { REQUIRE_FALSE(p.has_description()); }
    SECTION("Description") {
        p.set_description("Hello world");
        REQUIRE(p.has_description());
    }
}

TEST_CASE("ModuleResultPIMPL : set_type") {
    ModuleResultPIMPL p;
    p.set_type(d);
    SECTION("No value") { REQUIRE(p.type() == d); }
    SECTION("Has value with same type") {
        p.set_value(wrap_value<double>(3.14));
        p.set_type(d);
    }
    SECTION("Throws if value is different type") {
        p.set_value(wrap_value<double>(3.14));
        std::type_index i(typeid(int));
        REQUIRE_THROWS_AS(p.set_type(i), std::runtime_error);
    }
}

TEST_CASE("ModuleResultPIMPL : set_value") {
    ModuleResultPIMPL p;
    auto v = wrap_value<double>(3.14);
    SECTION("Throws if type is not set") {
        REQUIRE_THROWS_AS(p.set_value(v), std::bad_optional_access);
    }
    SECTION("Throws if the value's type is wrong") {
        p.set_type(d);
        auto i = wrap_value<int>(2);
        REQUIRE_THROWS_AS(p.set_value(i), std::invalid_argument);
    }
    SECTION("Can set it") {
        p.set_type(d);
        p.set_value(v);
        REQUIRE(*p.value() == *v);
    }
}

TEST_CASE("ModuleResultPIMPL : set_description") {
    ModuleResultPIMPL p;
    p.set_description("Hello world");
    REQUIRE(p.description() == "Hello world");
}

TEST_CASE("ModuleResultPIMPL : type") {
    ModuleResultPIMPL p;
    SECTION("Throws if type is not set") {
        REQUIRE_THROWS_AS(p.type(), std::bad_optional_access);
    }
    SECTION("Works if the type is set") {
        p.set_type(d);
        REQUIRE(p.type() == d);
    }
}

TEST_CASE("ModuleResultPIMPL : value") {
    ModuleResultPIMPL p;
    SECTION("Throws if value is not set") {
        REQUIRE_THROWS_AS(p.value(), std::runtime_error);
    }
    SECTION("Can get value") {
        p.set_type(d);
        p.set_value(wrap_value<double>(3.14));
        REQUIRE(any::any_cast<double>(*p.value()) == 3.14);
    }
}

TEST_CASE("ModuleResultPIMPL : description") {
    ModuleResultPIMPL p;
    SECTION("Throws if description is not set") {
        REQUIRE_THROWS_AS(p.description(), std::bad_optional_access);
    }
    SECTION("has a description") {
        p.set_description("Hello world");
        REQUIRE(p.description() == "Hello world");
    }
}

TEST_CASE("ModuleResultPIMPL : comparisons") {
    ModuleResultPIMPL p, p2;

    SECTION("Both empty") {
        REQUIRE(p == p2);
        REQUIRE_FALSE(p != p2);
    }
    SECTION("Different typed-ness") {
        p.set_type(d);
        REQUIRE(p != p2);
        REQUIRE_FALSE(p == p2);
    }
    SECTION("Different types") {
        p.set_type(d);
        p2.set_type(std::type_index(typeid(int)));
        REQUIRE(p != p2);
        REQUIRE_FALSE(p == p2);
    }
    SECTION("Different valued-ness") {
        p.set_type(d);
        p2.set_type(d);
        p.set_value(wrap_value<double>(3.14));
        REQUIRE(p != p2);
        REQUIRE_FALSE(p == p2);
    }
    SECTION("Different values") {
        p.set_type(d);
        p2.set_type(d);
        p.set_value(wrap_value<double>(3.14));
        p2.set_value(wrap_value<double>(1.23));
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

TEST_CASE("ModuleResultPIMPL : copy ctor") {
    ModuleResultPIMPL p;
    p.set_type(d);
    ModuleResultPIMPL p2(p);
    REQUIRE(p == p2);
}

TEST_CASE("ModuleResultPIMPL : copy assignment") {
    ModuleResultPIMPL p, p2;
    p2.set_type(d);
    auto pp = &(p = p2);
    REQUIRE(pp == &p);
    REQUIRE(p == p2);
}

TEST_CASE("ModuleResultPIMPL : move ctor") {
    ModuleResultPIMPL p;
    p.set_type(d);
    ModuleResultPIMPL p2(p);
    ModuleResultPIMPL p3(std::move(p2));
    REQUIRE(p == p3);
}

TEST_CASE("ModuleResultPIMPL : move assignment") {
    ModuleResultPIMPL p, p2;
    p2.set_type(d);
    ModuleResultPIMPL p3(p2);
    auto pp = &(p = std::move(p2));
    REQUIRE(pp == &p);
    REQUIRE(p == p3);
}
