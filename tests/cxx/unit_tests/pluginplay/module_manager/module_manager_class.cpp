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

#include "../catch.hpp"
#include "../examples/writing_a_module.hpp"
#include "test_common.hpp"
#include <pluginplay/module_manager/module_manager.hpp>
#include <pluginplay/plugin/plugin.hpp>

using pluginplay::ModuleManager;
using pluginplay::QualifiedKey;

namespace {

// Computes an area and counts how many times it actually ran. Unlike a lambda
// module it is memoizable, so the count shows whether a result came from the
// cache.
struct CountingArea : pluginplay::ModuleBase {
    static inline int n_runs = 0;

    CountingArea() : pluginplay::ModuleBase(this) {
        satisfies_property_type<Area>();
    }

    pluginplay::type::result_map run_(
      pluginplay::type::input_map inputs,
      pluginplay::type::submodule_map) const override {
        ++n_runs;
        const auto [base, height] = Area::unwrap_inputs(inputs);
        auto rv                   = results();
        return Area::wrap_results(rv, base * height);
    }
};

DECLARE_PLUGIN(ProviderA);
DECLARE_PLUGIN(ProviderB);
DECLARE_PLUGIN(Consumer);
DECLARE_PLUGIN(Unnamed);

PLUGIN_CTOR(ProviderA) {
    set_name("provider_a");
    add_module<Rectangle>("A area");
    add_module<CountingArea>("Shared");
}

PLUGIN_CTOR(ProviderB) {
    set_name("provider_b");
    add_module<Rectangle>("B area");
    add_module<CountingArea>("Shared");
}

// Prism prefers provider_a's area. Prism 2 prefers provider_b's area, and
// asks provider_a for a module it does not have.
PLUGIN_CTOR(Consumer) {
    set_name("consumer");
    set_preferred_providers("provider_a", "provider_b");
    add_module<Prism>("Prism");
    add_module<Prism>("Prism 2");
    default_submod_from("provider_a", "Prism", "area", "A area");
    default_submod_from("provider_b", "Prism", "area", "B area");
    default_submod_from("provider_a", "Prism 2", "area", "Not a module");
    default_submod_from("provider_b", "Prism 2", "area", "B area");
}

PLUGIN_CTOR(Unnamed) {}

// Uses a module as a submodule, then erases that module's key
DECLARE_PLUGIN(ErasesSubmod);
PLUGIN_CTOR(ErasesSubmod) {
    set_name("erases_submod");
    add_module<Rectangle>("Rectangle");
    add_module<Prism>("Prism");
    change_submod("Prism", "area", "Rectangle");
    modules().erase("Rectangle");
}

// Same name as ProviderA, in a different case
DECLARE_PLUGIN(ShoutingProviderA);
PLUGIN_CTOR(ShoutingProviderA) { set_name("PROVIDER_A"); }

// Like Consumer, but spells the provider's name in a different case
DECLARE_PLUGIN(CaseConsumer);
PLUGIN_CTOR(CaseConsumer) {
    set_name("case_consumer");
    set_preferred_providers("Provider_A");
    add_module<Prism>("Case Prism");
    default_submod_from("PROVIDER_A", "Case Prism", "area", "A area");
}

// The module a submodule of a module is set to
const pluginplay::Module& submod(ModuleManager& mm, const QualifiedKey& key,
                                 const std::string& callback) {
    return mm.at(key).submods().at(callback).value();
}

} // namespace

TEST_CASE("ModuleManager") {
    ModuleManager mm;

    SECTION("add_module(string)") {
        using mod_t = testing::NoPTModule; // Type of the Module we're adding

        mm.add_module<mod_t>("a mod");
        auto corr = testing::make_module<mod_t>("a mod");
        REQUIRE(mm.at("a mod") == *corr);
        REQUIRE(mm.get_modules("").count("a mod"));
    }

    SECTION("add_module(string, shared_ptr") {
        using mod_t = testing::NoPTModule; // Type of the Module we're adding

        mm.add_module("a mod", std::make_shared<mod_t>());
        auto corr = testing::make_module<mod_t>("a mod");
        REQUIRE(mm.at("a mod") == *corr);
    }

    SECTION("set_runtime") {
        auto prv = std::make_shared<parallelzone::runtime::RuntimeView>();
        mm.set_runtime(prv);
        REQUIRE(&mm.get_runtime() == prv.get());
    }

    SECTION("get_runtime") {
        REQUIRE(mm.get_runtime() == parallelzone::runtime::RuntimeView());
    }

    SECTION("keys") {
        using mod_t = testing::NoPTModule; // Type of the Module we're adding

        mm.add_module("a key", std::make_shared<mod_t>());
        mm.add_module("b key", std::make_shared<testing::ReadyModule>());
        mm.load_plugin<ProviderA>();
        auto keys = mm.keys();
        REQUIRE(keys.size() == mm.size());
        REQUIRE(keys[0] == QualifiedKey("", "a key"));
        REQUIRE(keys[1] == QualifiedKey("", "b key"));
        REQUIRE(keys[2] == QualifiedKey("provider_a", "A area"));
        REQUIRE(keys[3] == QualifiedKey("provider_a", "Shared"));
    }

    SECTION("has_cache") {
        REQUIRE(mm.has_cache());
        ModuleManager no_cache(nullptr, nullptr);
        REQUIRE_FALSE(no_cache.has_cache());
    }
}

TEST_CASE("ModuleManager : plugins") {
    ModuleManager mm;

    SECTION("No plugins") {
        REQUIRE_FALSE(mm.has_plugin("provider_a"));
        REQUIRE(mm.plugin_names() == ModuleManager::name_container_type{""});
        REQUIRE(mm.get_modules("").empty());
        REQUIRE_THROWS_AS(mm.get_plugin("provider_a"), std::out_of_range);
        REQUIRE_THROWS_AS(mm.get_modules("provider_a"), std::out_of_range);
    }

    SECTION("load_plugin<T>()") {
        mm.load_plugin<ProviderA>();
        REQUIRE(mm.has_plugin("provider_a"));
        REQUIRE(mm.plugin_names() ==
                ModuleManager::name_container_type{"", "provider_a"});
        REQUIRE(mm.get_plugin("provider_a") == ProviderA{});
        REQUIRE(&mm.get_modules("provider_a") ==
                &mm.get_plugin("provider_a").get_modules());
        REQUIRE(mm.size() == 2);
        REQUIRE(mm.count("A area") == 1);

        // The modules can be run and are memoizable
        REQUIRE(mm.at("A area").is_memoizable());
        REQUIRE(mm.run_as<Area>("A area", 2.0, 3.0) == 6.0);
    }

    SECTION("load_plugin(unique_ptr)") {
        auto plugin         = std::make_unique<ProviderA>();
        const auto* pplugin = plugin.get();
        mm.load_plugin(std::move(plugin));

        // *this owns the plugin; nothing was copied
        REQUIRE(&mm.get_plugin("provider_a") == pplugin);
    }

    SECTION("Can't load two plugins with the same name") {
        mm.load_plugin<ProviderA>();
        REQUIRE_THROWS_AS(mm.load_plugin<ProviderA>(), std::invalid_argument);
        REQUIRE(mm.plugin_names().size() == 2);
    }

    SECTION("Plugin names are case-insensitive") {
        mm.load_plugin<ProviderA>();
        REQUIRE(mm.has_plugin("Provider_A"));
        REQUIRE(&mm.get_plugin("PROVIDER_A") == &mm.get_plugin("provider_a"));
        REQUIRE(mm.get_modules("Provider_A").count("A area") == 1);
        REQUIRE(mm.count(QualifiedKey("PROVIDER_A", "A area")) == 1);
        REQUIRE(&mm.at(QualifiedKey("Provider_A", "A area")) ==
                &mm.at("A area"));

        // The name keeps the plugin's spelling
        REQUIRE(mm.plugin_names() ==
                ModuleManager::name_container_type{"", "provider_a"});

        // Same name in a different case is still a duplicate
        REQUIRE_THROWS_AS(mm.load_plugin<ShoutingProviderA>(),
                          std::invalid_argument);
    }

    SECTION("Erased submodules are given the runtime and caches") {
        mm.load_plugin<ErasesSubmod>();
        REQUIRE(mm.count("Rectangle") == 0);

        // Prism is only memoizable if its submodule has a UUID, which it only
        // gets by being attached
        REQUIRE(mm.at("Prism").is_memoizable());
        REQUIRE(mm.at("Prism").submods().at("area").value().is_memoizable());
    }

    SECTION("Can't load unnamed or null plugins") {
        REQUIRE_THROWS_AS(mm.load_plugin<Unnamed>(), std::invalid_argument);
        REQUIRE_THROWS_AS(mm.load_plugin(nullptr), std::invalid_argument);
        REQUIRE(mm.plugin_names().size() == 1);
    }

    SECTION("Modules with the same key get different caches") {
        mm.load_plugin<ProviderA>();
        mm.load_plugin<ProviderB>();
        CountingArea::n_runs = 0;

        REQUIRE(mm.run_as<Area>(QualifiedKey("provider_a", "Shared"), 2.0,
                                3.0) == 6.0);
        REQUIRE(CountingArea::n_runs == 1);

        // Same inputs, but a different module, so it must actually run
        REQUIRE(mm.run_as<Area>(QualifiedKey("provider_b", "Shared"), 2.0,
                                3.0) == 6.0);
        REQUIRE(CountingArea::n_runs == 2);

        // Same module and inputs, so the result comes from the cache
        REQUIRE(mm.run_as<Area>(QualifiedKey("provider_a", "Shared"), 2.0,
                                3.0) == 6.0);
        REQUIRE(CountingArea::n_runs == 2);
    }
}

TEST_CASE("ModuleManager : QualifiedKey lookup") {
    ModuleManager mm;
    mm.add_module<Rectangle>("Shared");
    mm.load_plugin<ProviderA>();
    mm.load_plugin<ProviderB>();

    const QualifiedKey a_shared("provider_a", "Shared");
    const QualifiedKey b_shared("provider_b", "Shared");
    const QualifiedKey own_shared("", "Shared");

    SECTION("count") {
        REQUIRE(mm.count("Shared") == 3);
        REQUIRE(mm.count("A area") == 1);
        REQUIRE(mm.count("not a key") == 0);
        REQUIRE(mm.count(a_shared) == 1);
        REQUIRE(mm.count(QualifiedKey("provider_a", "B area")) == 0);
        REQUIRE(mm.count(QualifiedKey("not a plugin", "Shared")) == 0);
    }

    SECTION("size") { REQUIRE(mm.size() == 5); }

    SECTION("at") {
        // Key-only lookups work when the key is unique
        REQUIRE(&mm.at("A area") ==
                &mm.at(QualifiedKey("provider_a", "A area")));

        // Qualified lookups pick the map
        REQUIRE(&mm.at(a_shared) ==
                mm.get_modules("provider_a").at("Shared").get());
        REQUIRE(&mm.at(own_shared) == mm.get_modules("").at("Shared").get());

        REQUIRE_THROWS_AS(mm.at("Shared"), std::invalid_argument);
        REQUIRE_THROWS_AS(mm.at("not a key"), std::out_of_range);
        REQUIRE_THROWS_AS(mm.at(QualifiedKey("provider_a", "B area")),
                          std::out_of_range);
        REQUIRE_THROWS_AS(mm.at(QualifiedKey("not a plugin", "Shared")),
                          std::out_of_range);
    }

    SECTION("Ambiguity error names the plugins") {
        try {
            mm.at("Shared");
            FAIL("Expected an exception");
        } catch(const std::invalid_argument& e) {
            const std::string msg = e.what();
            REQUIRE(msg.find("'provider_a'") != std::string::npos);
            REQUIRE(msg.find("'provider_b'") != std::string::npos);
            REQUIRE(msg.find("own modules") != std::string::npos);
        }
    }

    SECTION("change_input") {
        mm.change_input(a_shared, "Dimension 1", 2.0);
        const auto& inputs = mm.at(a_shared).inputs();
        REQUIRE(inputs.at("Dimension 1").value<double>() == 2.0);
        REQUIRE_FALSE(mm.at(b_shared).inputs().at("Dimension 1").has_value());
        REQUIRE_THROWS_AS(mm.change_input("Shared", "Dimension 1", 2.0),
                          std::invalid_argument);
    }

    SECTION("change_submod across maps") {
        mm.load_plugin<Consumer>();
        mm.change_submod("Prism", "area", b_shared);
        REQUIRE(submod(mm, "Prism", "area") == mm.at(b_shared));
        REQUIRE_THROWS_AS(mm.change_submod("Prism", "area", "Shared"),
                          std::invalid_argument);
    }

    SECTION("copy_module") {
        SECTION("Copies of plugin modules go in the own map") {
            mm.copy_module(a_shared, "Copy");
            REQUIRE(mm.count("Copy") == 1);
            REQUIRE(mm.count(QualifiedKey("", "Copy")) == 1);
            REQUIRE(mm.count(QualifiedKey("provider_a", "Copy")) == 0);
            REQUIRE_FALSE(mm.at("Copy").locked());
        }
        SECTION("The copy may reuse a key a plugin uses") {
            // Only the own map has to be free of the key
            mm.copy_module(a_shared, "A area");
            REQUIRE(mm.count("A area") == 2);
            REQUIRE(mm.count(QualifiedKey("", "A area")) == 1);
        }
        SECTION("Errors") {
            REQUIRE_THROWS_AS(mm.copy_module("Shared", "Copy"),
                              std::invalid_argument);
            REQUIRE_THROWS_AS(mm.copy_module(a_shared, "Shared"),
                              std::invalid_argument);
            REQUIRE_THROWS_AS(mm.copy_module("not a key", "Copy"),
                              std::out_of_range);
        }
    }

    SECTION("erase") {
        mm.erase(a_shared);
        REQUIRE(mm.count("Shared") == 2);
        REQUIRE(mm.count(a_shared) == 0);

        mm.erase("not a key"); // No-op
        REQUIRE(mm.size() == 4);

        REQUIRE_THROWS_AS(mm.erase("Shared"), std::invalid_argument);
    }

    SECTION("rename_module") {
        SECTION("A plugin's module moves to the own map") {
            mm.rename_module(a_shared, "Renamed");
            REQUIRE(mm.count(a_shared) == 0);
            REQUIRE(mm.count(QualifiedKey("", "Renamed")) == 1);
        }
        SECTION("Same key, different map") {
            // Moving A area to the own map under the same key must not trip
            // the ambiguity check when the original is erased
            mm.rename_module("A area", "A area");
            REQUIRE(mm.count(QualifiedKey("", "A area")) == 1);
            REQUIRE(mm.count(QualifiedKey("provider_a", "A area")) == 0);
        }
    }
}

TEST_CASE("ModuleManager : wire_defaults") {
    ModuleManager mm;

    SECTION("Highest priority loaded provider wins") {
        mm.load_plugin<Consumer>();
        mm.load_plugin<ProviderA>();
        mm.load_plugin<ProviderB>();
        mm.wire_defaults();
        REQUIRE(submod(mm, "Prism", "area") == mm.at("A area"));
        // provider_a lacks "Not a module", so provider_b fills Prism 2
        REQUIRE(submod(mm, "Prism 2", "area") == mm.at("B area"));
    }

    SECTION("Lower priority provider is used if higher is not loaded") {
        mm.load_plugin<Consumer>();
        mm.load_plugin<ProviderB>();
        mm.wire_defaults();
        REQUIRE(submod(mm, "Prism", "area") == mm.at("B area"));
    }

    SECTION("No providers leaves submodules unset") {
        mm.load_plugin<Consumer>();
        mm.wire_defaults();
        REQUIRE_FALSE(mm.at("Prism").submods().at("area").has_module());
    }

    SECTION("Load order does not matter") {
        mm.load_plugin<ProviderB>();
        mm.load_plugin<ProviderA>();
        mm.load_plugin<Consumer>();
        mm.wire_defaults();
        REQUIRE(submod(mm, "Prism", "area") == mm.at("A area"));
    }

    SECTION("User choices are kept") {
        mm.load_plugin<Consumer>();
        mm.load_plugin<ProviderA>();
        mm.load_plugin<ProviderB>();
        mm.change_submod("Prism", "area", "B area");
        mm.wire_defaults();
        REQUIRE(submod(mm, "Prism", "area") == mm.at("B area"));
    }

    SECTION("Can be called again after loading another plugin") {
        mm.load_plugin<Consumer>();
        mm.load_plugin<ProviderB>();
        mm.wire_defaults();
        mm.load_plugin<ProviderA>();
        mm.wire_defaults();
        // Already set from provider_b, so not overwritten by provider_a
        REQUIRE(submod(mm, "Prism", "area") == mm.at("B area"));
    }

    SECTION("Provider names are matched case-insensitively") {
        mm.load_plugin<CaseConsumer>();
        mm.load_plugin<ProviderA>();
        mm.wire_defaults();
        REQUIRE(submod(mm, "Case Prism", "area") == mm.at("A area"));
    }

    SECTION("Erased modules are skipped") {
        mm.load_plugin<Consumer>();
        mm.load_plugin<ProviderA>();
        mm.erase("Prism");
        REQUIRE_NOTHROW(mm.wire_defaults());
    }
}
