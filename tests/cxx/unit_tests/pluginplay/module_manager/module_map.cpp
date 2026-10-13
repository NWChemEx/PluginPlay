/*
 * Copyright 2026 NWChemEx-Project
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

#include "../catch.hpp"
#include "../examples/writing_a_module.hpp"
#include <pluginplay/module_manager/module_map.hpp>

using namespace pluginplay;

TEST_CASE("ModuleMap") {
    ModuleMap defaulted;

    ModuleMap has_mods;
    has_mods.add_module<Rectangle>("Rectangle");
    has_mods.add_module<Prism>("Prism");

    SECTION("Default ctor") {
        REQUIRE(defaulted.size() == 0);
        REQUIRE(defaulted.empty());
        REQUIRE(defaulted.keys().empty());
    }

    SECTION("add_module<T>") {
        defaulted.add_module<Rectangle>("Rectangle");
        REQUIRE(defaulted.size() == 1);
        REQUIRE(defaulted.count("Rectangle") == 1);
        auto mod = defaulted.at("Rectangle");
        REQUIRE(mod->has_name());
        REQUIRE(mod->get_name() == "Rectangle");
        REQUIRE_FALSE(mod->locked());
    }

    SECTION("add_module(key, base)") {
        defaulted.add_module("Rectangle", std::make_shared<Rectangle>());
        REQUIRE(defaulted.count("Rectangle") == 1);
        // Same as adding by type
        ModuleMap corr;
        corr.add_module<Rectangle>("Rectangle");
        REQUIRE(defaulted == corr);
    }

    SECTION("add_module throws for duplicate key") {
        REQUIRE_THROWS_AS(has_mods.add_module<Rectangle>("Rectangle"),
                          std::invalid_argument);
        // Keys are case-insensitive
        REQUIRE_THROWS_AS(has_mods.add_module<Rectangle>("rectangle"),
                          std::invalid_argument);
        REQUIRE(has_mods.size() == 2);
    }

    SECTION("add_module throws for null base") {
        REQUIRE_THROWS_AS(defaulted.add_module("key", nullptr),
                          std::invalid_argument);
        REQUIRE(defaulted.empty());
    }

    SECTION("count") {
        REQUIRE(has_mods.count("Rectangle") == 1);
        REQUIRE(has_mods.count("RECTANGLE") == 1);
        REQUIRE(has_mods.count("not a key") == 0);
        REQUIRE(defaulted.count("Rectangle") == 0);
    }

    SECTION("at") {
        REQUIRE(has_mods.at("Prism")->get_name() == "Prism");
        REQUIRE_THROWS_AS(has_mods.at("not a key"), std::out_of_range);
    }

    SECTION("keys") {
        auto keys = has_mods.keys();
        REQUIRE(keys.size() == 2);
        REQUIRE(std::count(keys.begin(), keys.end(), "Rectangle") == 1);
        REQUIRE(std::count(keys.begin(), keys.end(), "Prism") == 1);
    }

    SECTION("iteration") {
        std::size_t n = 0;
        for(const auto& [key, mod] : std::as_const(has_mods)) {
            REQUIRE(has_mods.at(key) == mod);
            ++n;
        }
        REQUIRE(n == has_mods.size());
    }

    SECTION("change_submod(key, key, key)") {
        REQUIRE_FALSE(has_mods.has_submod("Prism", "area"));
        has_mods.change_submod("Prism", "area", "Rectangle");
        REQUIRE(has_mods.has_submod("Prism", "area"));
        auto prism = has_mods.at("Prism");
        REQUIRE(prism->submods().at("area").value() ==
                *has_mods.at("Rectangle"));
    }

    SECTION("change_submod(key, key, pointer)") {
        // The submodule comes from a different ModuleMap
        defaulted.add_module<Rectangle>("Other rectangle");
        auto rect = defaulted.at("Other rectangle");
        has_mods.change_submod("Prism", "area", rect);
        REQUIRE(has_mods.has_submod("Prism", "area"));
        REQUIRE(has_mods.at("Prism")->submods().at("area").value() == *rect);
    }

    SECTION("change_submod throws") {
        // Module is not in *this
        REQUIRE_THROWS_AS(
          has_mods.change_submod("not a key", "area", "Rectangle"),
          std::out_of_range);
        // Submodule is not in *this
        REQUIRE_THROWS_AS(has_mods.change_submod("Prism", "area", "not a key"),
                          std::out_of_range);
        // Callback is not a submodule of the module
        REQUIRE_THROWS_AS(
          has_mods.change_submod("Prism", "not a callback", "Rectangle"),
          std::out_of_range);
        // Prism does not satisfy the Area property type
        REQUIRE_THROWS_AS(has_mods.change_submod("Prism", "area", "Prism"),
                          std::runtime_error);
    }

    SECTION("has_submod throws") {
        REQUIRE_THROWS_AS(has_mods.has_submod("not a key", "area"),
                          std::out_of_range);
        REQUIRE_THROWS_AS(has_mods.has_submod("Prism", "not a callback"),
                          std::out_of_range);
    }

    SECTION("default_submod(key, key, key)") {
        REQUIRE(has_mods.default_submod("Prism", "area", "Rectangle"));
        REQUIRE(has_mods.has_submod("Prism", "area"));

        // Already set, so a second default does nothing
        has_mods.add_module<Rectangle>("Rectangle 2");
        has_mods.at("Rectangle 2")->change_input("dimension 1", 1.23);
        REQUIRE_FALSE(has_mods.default_submod("Prism", "area", "Rectangle 2"));
        REQUIRE(has_mods.at("Prism")->submods().at("area").value() ==
                *has_mods.at("Rectangle"));
    }

    SECTION("default_submod(key, key, pointer)") {
        defaulted.add_module<Rectangle>("Other rectangle");
        auto rect = defaulted.at("Other rectangle");
        REQUIRE(has_mods.default_submod("Prism", "area", rect));

        // Already set, so a second default does nothing
        REQUIRE_FALSE(
          has_mods.default_submod("Prism", "area", has_mods.at("Rectangle")));
        REQUIRE(has_mods.at("Prism")->submods().at("area").value() == *rect);
    }

    SECTION("comparisons") {
        REQUIRE(defaulted == ModuleMap{});
        REQUIRE_FALSE(defaulted != ModuleMap{});
        REQUIRE(defaulted != has_mods);

        ModuleMap same;
        same.add_module<Rectangle>("Rectangle");
        same.add_module<Prism>("Prism");
        REQUIRE(same == has_mods);

        // Different key
        ModuleMap diff_key;
        diff_key.add_module<Rectangle>("Rectangle");
        diff_key.add_module<Prism>("Prism 2");
        REQUIRE(diff_key != has_mods);

        // Same keys, but a module's state differs
        same.change_submod("Prism", "area", "Rectangle");
        REQUIRE(same != has_mods);
    }

    SECTION("add_module(key, Module)") {
        defaulted.add_module("Copy", has_mods.at("Rectangle")->unlocked_copy());
        auto mod = defaulted.at("Copy");
        REQUIRE(mod->get_name() == "Copy");
        REQUIRE(mod != has_mods.at("Rectangle"));
        REQUIRE_THROWS_AS(defaulted.add_module("Copy", Module{}),
                          std::invalid_argument);
    }

    SECTION("copy_module") {
        has_mods.at("Rectangle")->lock();
        has_mods.copy_module("Rectangle", "Copy");
        auto copy = has_mods.at("Copy");
        REQUIRE(copy->get_name() == "Copy");
        REQUIRE_FALSE(copy->locked());

        // The copy has its own inputs
        copy->change_input("Dimension 1", 1.23);
        REQUIRE(*copy != *has_mods.at("Rectangle"));

        REQUIRE_THROWS_AS(has_mods.copy_module("not a key", "Copy 2"),
                          std::out_of_range);
        REQUIRE_THROWS_AS(has_mods.copy_module("Rectangle", "Prism"),
                          std::invalid_argument);
    }

    SECTION("erase") {
        has_mods.erase("Rectangle");
        REQUIRE(has_mods.count("Rectangle") == 0);
        REQUIRE(has_mods.size() == 1);
        has_mods.erase("not a key"); // No-op
        REQUIRE(has_mods.size() == 1);
    }

    SECTION("change_input") {
        has_mods.change_input("Rectangle", "Dimension 1", 1.23);
        const auto& inputs = has_mods.at("Rectangle")->inputs();
        REQUIRE(inputs.at("Dimension 1").value<double>() == 1.23);
        REQUIRE_THROWS_AS(
          has_mods.change_input("not a key", "Dimension 1", 1.0),
          std::out_of_range);
        REQUIRE_THROWS_AS(
          has_mods.change_input("Rectangle", "not an input", 1.0),
          std::out_of_range);
    }

    SECTION("copies share modules") {
        ModuleMap copy(has_mods);
        REQUIRE(copy.at("Prism") == has_mods.at("Prism"));
    }
}
