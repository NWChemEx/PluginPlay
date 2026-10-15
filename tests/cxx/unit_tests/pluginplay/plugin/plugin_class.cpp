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
#include <pluginplay/plugin/plugin.hpp>

using namespace pluginplay;

namespace {

// Adds the plugin's modules, as a helper function would in a real plugin
void load_modules(ModuleMap& modules) {
    modules.add_module<Rectangle>("Rectangle");
    modules.add_module<Prism>("Prism");
}

// A plugin written the way the design documentation describes
DECLARE_PLUGIN(MyPlugin);

PLUGIN_CTOR(MyPlugin) {
    set_name("my_plugin");
    set_preferred_providers("provider_a", "provider_b");
    load_modules(modules());
    change_submod("Prism", "area", "Rectangle");
    default_submod_from("provider_a", "Prism", "area", "A area");
    default_submod_from("provider_b", "Prism", "area", "B area");
    default_submod_from("provider_a", "Prism", "area", "A area 2");
}

// Exposes the protected members so their error checking can be tested
class TestPlugin : public Plugin {
public:
    using Plugin::add_module;
    using Plugin::change_submod;
    using Plugin::default_submod_from;
    using Plugin::modules;
    using Plugin::set_name;
    using Plugin::set_preferred_providers;
};

} // namespace

TEST_CASE("Plugin") {
    using default_submod = Plugin::DefaultSubmod;
    using default_list   = Plugin::default_list_type;
    using provider_list  = Plugin::provider_list_type;

    TestPlugin defaulted;
    MyPlugin my_plugin;

    SECTION("Default state") {
        REQUIRE(defaulted.name().empty());
        REQUIRE(defaulted.preferred_providers().empty());
        REQUIRE(defaulted.default_submods_from("provider_a").empty());
        REQUIRE(defaulted.get_modules().empty());
    }

    SECTION("PLUGIN_CTOR") {
        REQUIRE(my_plugin.name() == "my_plugin");
        REQUIRE(my_plugin.preferred_providers() ==
                provider_list{"provider_a", "provider_b"});

        const auto& modules = my_plugin.get_modules();
        REQUIRE(modules.size() == 2);
        REQUIRE(modules.has_submod("Prism", "area"));

        // Records are kept per provider, in the order they were made
        default_list corr_a{{"Prism", "area", "A area"},
                            {"Prism", "area", "A area 2"}};
        default_list corr_b{{"Prism", "area", "B area"}};
        REQUIRE(my_plugin.default_submods_from("provider_a") == corr_a);
        REQUIRE(my_plugin.default_submods_from("provider_b") == corr_b);
        REQUIRE(my_plugin.default_submods_from("provider_c").empty());
    }

    SECTION("set_name") {
        defaulted.set_name("a_name");
        REQUIRE(defaulted.name() == "a_name");
        REQUIRE_THROWS_AS(defaulted.set_name(""), std::invalid_argument);
        // Names must be identifiers
        for(const auto* bad : {"a name", "..", "a::b", "1abc", "a/b", "a-b"})
            REQUIRE_THROWS_AS(defaulted.set_name(bad), std::invalid_argument);
        REQUIRE(defaulted.name() == "a_name");
    }

    SECTION("is_valid_name") {
        for(const auto* good : {"a", "_a", "my_plugin", "Plugin2", "SCF"})
            REQUIRE(Plugin::is_valid_name(good));
        for(const auto* bad : {"", ".", "..", "a name", "a::b", "1abc", "a-b"})
            REQUIRE_FALSE(Plugin::is_valid_name(bad));
    }

    SECTION("add_module") {
        defaulted.add_module<Rectangle>("Rectangle");
        defaulted.add_module("Prism", std::make_shared<Prism>());
        ModuleMap corr;
        load_modules(corr);
        REQUIRE(defaulted.get_modules() == corr);
        REQUIRE_THROWS_AS(defaulted.add_module<Rectangle>("Rectangle"),
                          std::invalid_argument);
    }

    SECTION("change_submod") {
        load_modules(defaulted.modules());
        defaulted.change_submod("Prism", "area", "Rectangle");
        REQUIRE(defaulted.get_modules().has_submod("Prism", "area"));
        REQUIRE_THROWS_AS(defaulted.change_submod("Prism", "area", "not a key"),
                          std::out_of_range);
    }

    SECTION("set_preferred_providers") {
        defaulted.set_preferred_providers("a", std::string("b"));
        REQUIRE(defaulted.preferred_providers() == provider_list{"a", "b"});

        // Replaces the old list
        defaulted.set_preferred_providers(provider_list{"c"});
        REQUIRE(defaulted.preferred_providers() == provider_list{"c"});

        // Can be emptied
        defaulted.set_preferred_providers();
        REQUIRE(defaulted.preferred_providers().empty());
    }

    SECTION("set_preferred_providers throws") {
        defaulted.set_preferred_providers("a");
        REQUIRE_THROWS_AS(defaulted.set_preferred_providers(""),
                          std::invalid_argument);
        REQUIRE_THROWS_AS(defaulted.set_preferred_providers("not valid"),
                          std::invalid_argument);
        REQUIRE_THROWS_AS(defaulted.set_preferred_providers("b", "b"),
                          std::invalid_argument);
        // Names are case-insensitive, so this is a duplicate too
        REQUIRE_THROWS_AS(defaulted.set_preferred_providers("b", "B"),
                          std::invalid_argument);
        REQUIRE(defaulted.preferred_providers() == provider_list{"a"});

        // Can't drop a provider that has recorded defaults
        load_modules(defaulted.modules());
        defaulted.default_submod_from("a", "Prism", "area", "key");
        REQUIRE_THROWS_AS(defaulted.set_preferred_providers("b"),
                          std::invalid_argument);
        REQUIRE(defaulted.preferred_providers() == provider_list{"a"});

        // Keeping it (in any case), and adding others, is fine
        defaulted.set_preferred_providers("b", "A");
        REQUIRE(defaulted.preferred_providers() == provider_list{"b", "A"});
    }

    SECTION("default_submod_from") {
        load_modules(defaulted.modules());
        defaulted.set_preferred_providers("a");
        defaulted.default_submod_from("a", "Prism", "area", "key");
        REQUIRE(defaulted.default_submods_from("a") ==
                default_list{default_submod{"Prism", "area", "key"}});

        // Recording a default doesn't change the module
        REQUIRE_FALSE(defaulted.get_modules().has_submod("Prism", "area"));

        // Provider names are case-insensitive
        defaulted.default_submod_from("A", "Prism", "area", "key 2");
        default_list corr{default_submod{"Prism", "area", "key"},
                          default_submod{"Prism", "area", "key 2"}};
        REQUIRE(defaulted.default_submods_from("a") == corr);
        REQUIRE(defaulted.default_submods_from("A") == corr);
    }

    SECTION("default_submod_from throws") {
        load_modules(defaulted.modules());
        defaulted.set_preferred_providers("a");

        // Not a preferred provider
        REQUIRE_THROWS_AS(
          defaulted.default_submod_from("b", "Prism", "area", "key"),
          std::invalid_argument);
        // Not one of the plugin's modules
        REQUIRE_THROWS_AS(
          defaulted.default_submod_from("a", "not a key", "area", "key"),
          std::out_of_range);
        // Not a submodule of the module
        REQUIRE_THROWS_AS(
          defaulted.default_submod_from("a", "Prism", "not a callback", "key"),
          std::out_of_range);
        REQUIRE(defaulted.default_submods_from("a").empty());
    }

    SECTION("comparisons") {
        REQUIRE(defaulted == TestPlugin{});
        REQUIRE(my_plugin == MyPlugin{});
        REQUIRE_FALSE(my_plugin != MyPlugin{});
        REQUIRE(defaulted != my_plugin);

        TestPlugin other;
        other.set_name("my_plugin");
        REQUIRE(other != defaulted);
    }
}
