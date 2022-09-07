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

#include "../test_common.hpp"
#include "pluginplay/detail_/submodule_request_pimpl.hpp"
#include <catch2/catch.hpp>

using namespace pluginplay::detail_;
using module_ptr = typename SubmoduleRequestPIMPL::module_ptr;

static const auto null_pt_t      = std::type_index(typeid(testing::NullPT));
static const auto not_ready_pt_t = std::type_index(typeid(testing::OneIn));

TEST_CASE("SubmoduleRequestPIMPL : Default ctor") {
    SubmoduleRequestPIMPL pimpl;
    REQUIRE_FALSE(pimpl.has_module());
    REQUIRE_FALSE(pimpl.has_type());
    REQUIRE_FALSE(pimpl.has_description());
    REQUIRE_FALSE(pimpl.ready());
}

TEST_CASE("SubmoduleRequestPIMPL : Copy ctor") {
    SubmoduleRequestPIMPL pimpl1;
    pimpl1.set_type(null_pt_t, pluginplay::type::input_map{});
    SubmoduleRequestPIMPL pimpl2(pimpl1);
    REQUIRE(pimpl2 == pimpl1);
}

TEST_CASE("SubmoduleRequestPIMPL : Copy assignment") {
    SubmoduleRequestPIMPL pimpl1;
    pimpl1.set_type(null_pt_t, pluginplay::type::input_map{});
    SubmoduleRequestPIMPL pimpl2;
    auto* ptr = &(pimpl2 = pimpl1);
    REQUIRE(pimpl2 == pimpl1);
    REQUIRE(ptr == &pimpl2);
}

TEST_CASE("SubmoduleRequestPIMPL : Move ctor") {
    SubmoduleRequestPIMPL pimpl1;
    pimpl1.set_type(null_pt_t, pluginplay::type::input_map{});
    SubmoduleRequestPIMPL pimpl3(pimpl1);
    SubmoduleRequestPIMPL pimpl2(std::move(pimpl1));
    REQUIRE(pimpl2 == pimpl3);
}

TEST_CASE("SubmoduleRequestPIMPL : Move assignment") {
    SubmoduleRequestPIMPL pimpl1;
    pimpl1.set_type(null_pt_t, pluginplay::type::input_map{});
    SubmoduleRequestPIMPL pimpl3(pimpl1);
    SubmoduleRequestPIMPL pimpl2;
    auto* ptr = &(pimpl2 = std::move(pimpl1));
    REQUIRE(pimpl2 == pimpl3);
    REQUIRE(ptr == &pimpl2);
}

TEST_CASE("SubmoduleRequestPIMPL : clone") {
    SubmoduleRequestPIMPL pimpl1;
    SECTION("Empty") {
        auto ptr = pimpl1.clone();
        REQUIRE(*ptr == pimpl1);
    }
}

TEST_CASE("SubmoduleRequestPIMPL : has_type") {
    SubmoduleRequestPIMPL pimpl1;
    SECTION("No type") { REQUIRE_FALSE(pimpl1.has_type()); }
    SECTION("Type") {
        pimpl1.set_type(null_pt_t, pluginplay::type::input_map{});
        REQUIRE(pimpl1.has_type());
    }
}

TEST_CASE("SubmoduleRequestPIMPL : has_module") {
    SubmoduleRequestPIMPL pimpl;
    SECTION("No Module") { REQUIRE_FALSE(pimpl.has_module()); }
    SECTION("Module") {
        pimpl.set_type(null_pt_t, pluginplay::type::input_map{});
        pimpl.set_module(testing::make_module<testing::NullModule>());
        REQUIRE(pimpl.has_module());
    }
}

TEST_CASE("SubmoduleRequestPIMPL : has_description") {
    SubmoduleRequestPIMPL pimpl;
    SECTION("No description") { REQUIRE_FALSE(pimpl.has_description()); }
    SECTION("Has description") {
        pimpl.set_description("Hello world");
        REQUIRE(pimpl.has_description());
    }
}

TEST_CASE("SubmoduleRequestPIMPL : ready") {
    SubmoduleRequestPIMPL pimpl1;
    SECTION("No type means not ready") { REQUIRE_FALSE(pimpl1.ready()); }
    SECTION("No submodule") {
        pimpl1.set_type(null_pt_t, pluginplay::type::input_map{});
        REQUIRE_FALSE(pimpl1.ready());
    }
    SECTION("Has ready submodule") {
        pimpl1.set_type(null_pt_t, pluginplay::type::input_map{});
        pimpl1.set_module(testing::make_module<testing::NullModule>());
        REQUIRE(pimpl1.ready());
    }
    SECTION("Has not ready submodule") {
        pimpl1.set_type(not_ready_pt_t, pluginplay::type::input_map{});
        pimpl1.set_module(testing::make_module<testing::NotReadyModule>());
        REQUIRE_FALSE(pimpl1.ready());
    }
}

TEST_CASE("SubmoduleRequestPIMPL : set_type") {
    SubmoduleRequestPIMPL p;
    pluginplay::type::input_map inputs;
    p.set_type(null_pt_t, inputs);

    SECTION("Set the property type") { REQUIRE(p.has_type()); }
    SECTION("Fine if module is set and obeys type") {
        p.set_module(testing::make_module<testing::NullModule>());
        p.set_type(null_pt_t, inputs);
    }
    SECTION("Throws if module is already set and different") {
        p.set_type(null_pt_t, inputs);
        p.set_module(testing::make_module<testing::NullModule>());
        REQUIRE_THROWS_AS(p.set_type(not_ready_pt_t, inputs),
                          std::runtime_error);
    }
}

TEST_CASE("SubmoduleRequestPIMPL : set_module") {
    SubmoduleRequestPIMPL p;
    auto mod = testing::make_module<testing::NullModule>();
    SECTION("Throws if nullptr") {
        std::shared_ptr<pluginplay::Module> ptr;
        REQUIRE_THROWS_AS(p.set_module(ptr), std::runtime_error);
    }
    SECTION("Throws if type is not set") {
        REQUIRE_THROWS_AS(p.set_module(mod), std::bad_optional_access);
    }
    SECTION("Throws if doesn't satisfy property type") {
        p.set_type(null_pt_t, pluginplay::type::input_map{});
        auto mod2 = testing::make_module<testing::NotReadyModule>();
        REQUIRE_THROWS_AS(p.set_module(mod2), std::runtime_error);
    }
    SECTION("Can set the module") {
        p.set_type(null_pt_t, pluginplay::type::input_map{});
        p.set_module(mod);
        REQUIRE(p.value() == *mod);
    }
}

TEST_CASE("SubmoduleRequestPIMPL : set_description") {
    SubmoduleRequestPIMPL p;
    p.set_description("Hello World");
    REQUIRE(p.description() == "Hello World");
}

TEST_CASE("SubmoduleRequestPIMPL : type") {
    SubmoduleRequestPIMPL p;
    SECTION("Not set") {
        REQUIRE_THROWS_AS(p.type(), std::bad_optional_access);
    }
    SECTION("Set") {
        p.set_type(null_pt_t, pluginplay::type::input_map{});
        REQUIRE(p.type() == null_pt_t);
    }
}

TEST_CASE("SubmoduleRequestPIMPL : value") {
    SubmoduleRequestPIMPL p;
    SECTION("No value") { REQUIRE_THROWS_AS(p.value(), std::runtime_error); }
    SECTION("Has a value") {
        p.set_type(null_pt_t, pluginplay::type::input_map{});
        auto mod = testing::make_module<testing::NullModule>();
        p.set_module(mod);
        REQUIRE(p.value() == *mod);
    }
}

TEST_CASE("SubmoduleRequestPIMPL : description") {
    SubmoduleRequestPIMPL p;
    p.set_description("Hello world");
    REQUIRE(p.description() == "Hello world");
}

TEST_CASE("SubmoduleRequestPIMPL : lock") {
    SubmoduleRequestPIMPL p;
    p.set_type(null_pt_t, pluginplay::type::input_map{});
    auto mod = testing::make_module<testing::NullModule>();
    SECTION("Lock a ready module") {
        p.set_module(mod);
        REQUIRE_FALSE(mod->locked());
        p.lock();
        REQUIRE(mod->locked());
    }
    SECTION("Can't lock unready module") {
        REQUIRE_THROWS_AS(p.lock(), std::runtime_error);
    }
}

TEST_CASE("SubmoduleRequestPIMPL : comparisons") {
    SubmoduleRequestPIMPL p, p2;

    SECTION("Default constructed") {
        REQUIRE(p == p2);
        REQUIRE_FALSE(p != p2);
    }
    SECTION("Different having type-ness") {
        p.set_type(null_pt_t, pluginplay::type::input_map{});
        REQUIRE(p != p2);
        REQUIRE_FALSE(p == p2);
    }
    SECTION("Different types") {
        p.set_type(null_pt_t, pluginplay::type::input_map{});
        p2.set_type(not_ready_pt_t, pluginplay::type::input_map{});
        REQUIRE(p != p2);
        REQUIRE_FALSE(p == p2);
    }
    SECTION("Different having description-nes") {
        p.set_description("Hello world");
        REQUIRE(p != p2);
        REQUIRE_FALSE(p == p2);
    }
    SECTION("Different Descriptions") {
        p.set_description("Hello world");
        p2.set_description("Bye world");
        REQUIRE(p != p2);
        REQUIRE_FALSE(p == p2);
    }
    SECTION("Different Module-ness") {
        p.set_type(null_pt_t, pluginplay::type::input_map{});
        p2.set_type(null_pt_t, pluginplay::type::input_map{});
        p.set_module(testing::make_module<testing::NullModule>());
        REQUIRE(p != p2);
        REQUIRE_FALSE(p == p2);
    }
    SECTION("Different Modules") {
        p.set_type(not_ready_pt_t, pluginplay::type::input_map{});
        p2.set_type(not_ready_pt_t, pluginplay::type::input_map{});
        p.set_module(testing::make_module<testing::NotReadyModule>());
        auto mod = testing::make_module<testing::NotReadyModule>();
        mod->change_input("Option 1", 0);
        p2.set_module(mod);
        REQUIRE(p != p2);
        REQUIRE_FALSE(p == p2);
    }
    SECTION("Fully set-up") {
        p.set_type(not_ready_pt_t, pluginplay::type::input_map{});
        p2.set_type(not_ready_pt_t, pluginplay::type::input_map{});
        p.set_module(testing::make_module<testing::NotReadyModule>());
        p2.set_module(testing::make_module<testing::NotReadyModule>());
        REQUIRE(p == p2);
        REQUIRE_FALSE(p != p2);
    }
}
