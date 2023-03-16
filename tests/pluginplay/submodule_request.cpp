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

#include "test_common.hpp"
#include <catch2/catch.hpp>
#include <pluginplay/submodule_request.hpp>

using namespace pluginplay;

static const auto null_pt_t      = std::type_index(typeid(testing::NullPT));
static const auto not_ready_pt_t = std::type_index(typeid(testing::OneIn));
static const auto inputs         = type::input_map{};

TEST_CASE("SubmoduleRequest : default ctor") {
    SubmoduleRequest r;
    REQUIRE_FALSE(r.has_module());
    REQUIRE_FALSE(r.has_type());
    REQUIRE_FALSE(r.has_description());
    REQUIRE_FALSE(r.ready());
}

TEST_CASE("SubmoduleRequest : has_type") {
    SubmoduleRequest r;
    SECTION("No type") { REQUIRE_FALSE(r.has_type()); }
    SECTION("Type") {
        r.set_type<testing::NullPT>();
        REQUIRE(r.has_type());
    }
}

TEST_CASE("SubmoduleRequest : has_module") {
    SubmoduleRequest r;
    SECTION("No Module") { REQUIRE_FALSE(r.has_module()); }
    SECTION("Module") {
        r.set_type<testing::NullPT>();
        r.change(testing::make_module<testing::NullModule>());
        REQUIRE(r.has_module());
    }
}

TEST_CASE("SubmoduleRequest : has_description") {
    SubmoduleRequest r;
    SECTION("No description") { REQUIRE_FALSE(r.has_description()); }
    SECTION("Has description") {
        r.set_description("Hello world");
        REQUIRE(r.has_description());
    }
}

TEST_CASE("SubmoduleRequestL : ready") {
    SubmoduleRequest r;
    SECTION("No type means not ready") { REQUIRE_FALSE(r.ready()); }
    SECTION("No submodule") {
        r.set_type<testing::NullPT>();
        REQUIRE_FALSE(r.ready());
    }
    SECTION("Has ready submodule") {
        r.set_type<testing::NullPT>();
        r.change(testing::make_module<testing::NullModule>());
        REQUIRE(r.ready());
    }
    SECTION("Has not ready submodule") {
        r.set_type<testing::OneIn>();
        r.change(testing::make_module<testing::NotReadyModule2>());
        REQUIRE_FALSE(r.ready());
    }
}

TEST_CASE("SubmoduleRequest : set_type") {
    SubmoduleRequest r;
    r.set_type<testing::NullPT>();

    SECTION("Set the property type") { REQUIRE(r.has_type()); }
    SECTION("Fine if module is set and obeys type") {
        r.change(testing::make_module<testing::NullModule>());
        r.set_type<testing::NullPT>();
    }
    SECTION("Throws if module is already set and different") {
        r.set_type<testing::NullPT>();
        r.change(testing::make_module<testing::NullModule>());
        REQUIRE_THROWS_AS(r.set_type<testing::OneIn>(), std::runtime_error);
    }
}

TEST_CASE("SubmoduleRequest : change") {
    SubmoduleRequest r;
    auto mod = testing::make_module<testing::NullModule>();
    SECTION("Throws if nullptr") {
        std::shared_ptr<pluginplay::Module> ptr;
        REQUIRE_THROWS_AS(r.change(ptr), std::runtime_error);
    }
    SECTION("Throws if type is not set") {
        REQUIRE_THROWS_AS(r.change(mod), std::bad_optional_access);
    }
    SECTION("Throws if doesn't satisfy property type") {
        r.set_type<testing::NullPT>();
        auto mod2 = testing::make_module<testing::NotReadyModule>();
        REQUIRE_THROWS_AS(r.change(mod2), std::runtime_error);
    }
    SECTION("Can set the module") {
        r.set_type<testing::NullPT>();
        r.change(mod);
        REQUIRE(r.value() == *mod);
    }
}

TEST_CASE("SubmoduleRequest : set_description") {
    SubmoduleRequest r;
    r.set_description("Hello World");
    REQUIRE(r.description() == "Hello World");
}

TEST_CASE("SubmoduleRequest : type") {
    SubmoduleRequest r;
    SECTION("Not set") {
        REQUIRE_THROWS_AS(r.type(), std::bad_optional_access);
    }
    SECTION("Set") {
        r.set_type<testing::NullPT>();
        REQUIRE(r.type() == std::type_index(typeid(testing::NullPT)));
    }
}

TEST_CASE("SubmoduleRequest : value") {
    SubmoduleRequest r;
    SECTION("No value") { REQUIRE_THROWS_AS(r.value(), std::runtime_error); }
    SECTION("Has a value") {
        r.set_type<testing::NullPT>();
        auto mod = testing::make_module<testing::NullModule>();
        r.change(mod);
        REQUIRE(r.value() == *mod);
    }
}

TEST_CASE("SubmoduleRequest : description") {
    SubmoduleRequest r;
    r.set_description("Hello world");
    REQUIRE(r.description() == "Hello world");
}

TEST_CASE("SubmoduleRequest : lock") {
    SubmoduleRequest p;
    p.set_type<testing::NullPT>();
    auto mod = testing::make_module<testing::NullModule>();
    SECTION("Lock a ready module") {
        p.change(mod);
        REQUIRE_FALSE(mod->locked());
        p.lock();
        REQUIRE(mod->locked());
    }
    SECTION("Can't lock unready module") {
        REQUIRE_THROWS_AS(p.lock(), std::runtime_error);
    }
}

TEST_CASE("SubmoduleRequest : run_as") {
    SubmoduleRequest r;
    SECTION("Throws if type is not set") {
        REQUIRE_THROWS_AS(r.run_as<testing::NullPT>(),
                          std::bad_optional_access);
    }
    SECTION("Throws if type is different") {
        r.set_type<testing::NullPT>();
        REQUIRE_THROWS_AS(r.run_as<testing::OneIn>(3), std::invalid_argument);
    }
    SECTION(
      "Trips static assert if arguments not convertible to correct types") {
        // r.run_as<testing::TwoIn>(3,"4"); // uncomment to trip static assert
        // r.run_as<testing::TwoIn>("3",4); // uncomment to trip static assert
    }
    SECTION("Throws if module is not set") {
        r.set_type<testing::NullPT>();
        REQUIRE_THROWS_AS(r.run_as<testing::NullPT>(), std::runtime_error);
    }
    SECTION("Works") {
        r.set_type<testing::NullPT>();
        r.change(testing::make_module<testing::NullModule>());
        r.run_as<testing::NullPT>();
    }
}

TEST_CASE("SubmoduleRequest : comparisons") {
    SubmoduleRequest r, r2;

    SECTION("Default constructed") {
        REQUIRE(r == r2);
        REQUIRE_FALSE(r != r2);
    }
    SECTION("Different having type-ness") {
        r.set_type<testing::NullPT>();
        REQUIRE(r != r2);
        REQUIRE_FALSE(r == r2);
    }
    SECTION("Different types") {
        r.set_type<testing::NullPT>();
        r2.set_type<testing::OneIn>();
        REQUIRE(r != r2);
        REQUIRE_FALSE(r == r2);
    }
    SECTION("Different having description-nes") {
        r.set_description("Hello world");
        REQUIRE(r != r2);
        REQUIRE_FALSE(r == r2);
    }
    SECTION("Different Descriptions") {
        r.set_description("Hello world");
        r2.set_description("Bye world");
        REQUIRE(r != r2);
        REQUIRE_FALSE(r == r2);
    }
    SECTION("Different Module-ness") {
        r.set_type<testing::NullPT>();
        r2.set_type<testing::NullPT>();
        r.change(testing::make_module<testing::NullModule>());
        REQUIRE(r != r2);
        REQUIRE_FALSE(r == r2);
    }
    SECTION("Different Modules") {
        r.set_type<testing::OneIn>();
        r2.set_type<testing::OneIn>();
        r.change(testing::make_module<testing::NotReadyModule>());
        auto mod = testing::make_module<testing::NotReadyModule>();
        mod->change_input("Option 1", 0);
        r2.change(mod);
        REQUIRE(r != r2);
        REQUIRE_FALSE(r == r2);
    }
    SECTION("Fully set-up") {
        r.set_type<testing::OneIn>();
        r2.set_type<testing::OneIn>();
        r.change(testing::make_module<testing::NotReadyModule>());
        r2.change(testing::make_module<testing::NotReadyModule>());
        REQUIRE(r == r2);
        REQUIRE_FALSE(r != r2);
    }
}

TEST_CASE("SubmoduleRequest : Copy ctor") {
    SubmoduleRequest r;
    r.set_type<testing::NullPT>();
    SubmoduleRequest r2(r);
    REQUIRE(r2 == r);
}

TEST_CASE("SubmoduleRequest : Copy assignment") {
    SubmoduleRequest r;
    r.set_type<testing::NullPT>();
    SubmoduleRequest r2;
    auto* ptr = &(r2 = r);
    REQUIRE(r2 == r);
    REQUIRE(ptr == &r2);
}

TEST_CASE("SubmoduleRequest : Move ctor") {
    SubmoduleRequest r;
    r.set_type<testing::NullPT>();
    SubmoduleRequest r3(r);
    SubmoduleRequest r2(std::move(r));
    REQUIRE(r2 == r3);
}

TEST_CASE("SubmoduleRequest : Move assignment") {
    SubmoduleRequest r;
    r.set_type<testing::NullPT>();
    SubmoduleRequest r3(r);
    SubmoduleRequest r2;
    auto* ptr = &(r2 = std::move(r));
    REQUIRE(r2 == r3);
    REQUIRE(ptr == &r2);
}
