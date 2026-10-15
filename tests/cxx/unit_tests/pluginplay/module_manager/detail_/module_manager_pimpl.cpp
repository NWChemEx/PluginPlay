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

#include "../../catch.hpp"
#include "../../examples/writing_a_module.hpp"
#include "pluginplay/module_manager/detail_/module_manager_pimpl.hpp"
#include <pluginplay/plugin/plugin.hpp>

using namespace pluginplay;
using namespace pluginplay::detail_;

using base_map   = typename ModuleManagerPIMPL::base_map;
using module_map = typename ModuleManagerPIMPL::module_map;

namespace {

DECLARE_PLUGIN(PimplTestPlugin);

PLUGIN_CTOR(PimplTestPlugin) {
    set_name("test_plugin");
    add_module<Rectangle>("Rectangle");
}

} // namespace

TEST_CASE("ModuleManagerPIMPL : Default ctor") {
    ModuleManagerPIMPL pimpl;
    SECTION("State") {
        REQUIRE(pimpl.size() == 0);
        REQUIRE(pimpl.m_bases == base_map{});
        REQUIRE(pimpl.m_modules == module_map{});
        REQUIRE(pimpl.m_plugins.empty());
    }
}

TEST_CASE("ModuleManagerPIMPL : Runtime") {
    SECTION("ctor") {
        auto runtime = std::make_shared<parallelzone::runtime::RuntimeView>();
        ModuleManagerPIMPL pimple(runtime, nullptr);
        auto& internal_runtime = pimple.get_runtime();
        REQUIRE(&internal_runtime == runtime.get());
    }
    SECTION("set_runtime") {
        ModuleManagerPIMPL pimple;
        auto runtime = std::make_shared<parallelzone::runtime::RuntimeView>();

        auto& internal_runtime1 = pimple.get_runtime();
        REQUIRE(&internal_runtime1 != runtime.get());

        pimple.set_runtime(runtime);
        auto& internal_runtime2 = pimple.get_runtime();
        REQUIRE(&internal_runtime2 == runtime.get());
    }
}

TEST_CASE("ModuleManagerPIMPL : comparison") {
    ModuleManagerPIMPL pimpl1, pimpl2;
    SECTION("Default state") {
        REQUIRE(pimpl1 == pimpl2);
        REQUIRE(!(pimpl1 != pimpl2));
    }
    SECTION("Add a base") {
        auto ptr                    = std::make_shared<Rectangle>();
        pimpl1.m_bases[ptr->type()] = ptr;
        REQUIRE(pimpl1 != pimpl2);
        REQUIRE(!(pimpl1 == pimpl2));
    }
    SECTION("Different bases") {
        SECTION("Different instances") {
            auto ptr1                    = std::make_shared<Rectangle>();
            auto ptr2                    = std::make_shared<Rectangle>();
            pimpl1.m_bases[ptr1->type()] = ptr1;
            pimpl2.m_bases[ptr2->type()] = ptr2;
            REQUIRE(pimpl1 == pimpl2);
            REQUIRE(!(pimpl1 != pimpl2));
        }
        SECTION("Different types") {
            auto ptr1                    = std::make_shared<Rectangle>();
            auto ptr2                    = std::make_shared<Prism>();
            pimpl1.m_bases[ptr1->type()] = ptr1;
            pimpl2.m_bases[ptr2->type()] = ptr2;
            REQUIRE(pimpl1 != pimpl2);
            REQUIRE(!(pimpl1 == pimpl2));
        }
    }
    SECTION("Different modules") {
        SECTION("Different instances") {
            SECTION("Same key") {
                pimpl1.m_modules.add_module("key", Module{});
                pimpl2.m_modules.add_module("key", Module{});
                REQUIRE(pimpl1 == pimpl2);
                REQUIRE(!(pimpl1 != pimpl2));
            }
            SECTION("Different key") {
                pimpl1.m_modules.add_module("key1", Module{});
                pimpl2.m_modules.add_module("key2", Module{});
                REQUIRE(pimpl1 != pimpl2);
                REQUIRE(!(pimpl1 == pimpl2));
            }
        }
    }
}

TEST_CASE("ModuleManagerPIMPL : add_module") {
    auto ptr1 = std::make_shared<Rectangle>();
    ModuleManagerPIMPL pimpl1, pimpl2;
    pimpl2.m_bases[ptr1->type()] = ptr1;
    pimpl2.m_modules.add_module("key1", ptr1);
    pimpl1.add_module("key1", ptr1);
    REQUIRE(pimpl1 == pimpl2);
    SECTION("Module is given the runtime and caches") {
        REQUIRE(&ptr1->get_runtime() == &pimpl1.get_runtime());
        REQUIRE(ptr1->has_uuid());
        REQUIRE(pimpl1.m_modules.at("key1")->is_memoizable());
    }
    SECTION("Can't add a null module") {
        REQUIRE_THROWS_AS(pimpl1.add_module("key2", nullptr),
                          std::invalid_argument);
    }
    SECTION("Can't use the key") {
        REQUIRE_THROWS_AS(pimpl1.add_module("key1", ptr1),
                          std::invalid_argument);
    }
}

TEST_CASE("ModuleManagerPIMPL : copy_module") {
    auto ptr1 = std::make_shared<Rectangle>();
    ModuleManagerPIMPL pimpl1, pimpl2;
    pimpl1.add_module("key", ptr1);
    pimpl2.add_module("key", ptr1);

    SECTION("Copies non-locked module") {
        pimpl1.copy_module("key", "new_key");
        pimpl2.m_modules.add_module("new_key", ptr1);
        REQUIRE(pimpl1 == pimpl2);
    }
    SECTION("Copy locked module") {
        pimpl1.m_modules.at("key")->lock();
        pimpl1.copy_module("key", "new_key");
        REQUIRE(!pimpl1.m_modules.at("new_key")->locked());
        SECTION("Different instances") {
            auto mod = pimpl1.m_modules.at("new_key");
            mod->change_input("dimension 1", 1.23);
            REQUIRE(*mod != *pimpl1.m_modules.at("key"));
        }
    }
}

TEST_CASE("ModuleManagerPIMPL : count") {
    auto ptr1 = std::make_shared<Rectangle>();
    ModuleManagerPIMPL pimpl1;
    pimpl1.add_module("key", ptr1);
    SECTION("Not a key") { REQUIRE(pimpl1.count("not a key") == 0); }
    SECTION("Is a key") { REQUIRE(pimpl1.count("key") == 1); }
}

TEST_CASE("ModuleManagerPIMPL : size") {
    ModuleManagerPIMPL pimpl1;
    SECTION("Empty") { REQUIRE(pimpl1.size() == 0); }
    SECTION("With module") {
        auto ptr1 = std::make_shared<Rectangle>();
        pimpl1.add_module("key", ptr1);
        REQUIRE(pimpl1.size() == 1);
    }
}

TEST_CASE("ModuleManagerPIMPL : at") {
    ModuleManagerPIMPL pimpl1;
    SECTION("Throws for bad key") {
        REQUIRE_THROWS_AS(pimpl1.at("not a key"), std::out_of_range);
    }
    SECTION("No submodules") {
        auto ptr1 = std::make_shared<Rectangle>();
        pimpl1.add_module("key", ptr1);
        REQUIRE(*pimpl1.m_modules.at("key") == *pimpl1.at("key"));
    }
    SECTION("Submodules") {
        auto ptr1 = std::make_shared<Rectangle>();
        auto ptr2 = std::make_shared<Prism>();
        pimpl1.add_module("key1", ptr1);
        pimpl1.add_module("key2", ptr2);
        // at() does not fill in unset submodules
        auto ptr3 = pimpl1.m_modules.at("key2");
        REQUIRE(ptr3 == pimpl1.at("key2"));
        REQUIRE(!ptr3->ready());
    }
}

TEST_CASE("ModuleManagerPIMPL : keys") {
    ModuleManagerPIMPL pimpl1;
    SECTION("Empty") {
        REQUIRE(pimpl1.keys().size() == 0);
        REQUIRE(pimpl1.keys().empty());
    }
    SECTION("With module") {
        auto ptr1 = std::make_shared<Rectangle>();
        pimpl1.add_module("rectangle", ptr1);
        REQUIRE(pimpl1.keys().size() == 1);
        REQUIRE(pimpl1.keys().at(0) == QualifiedKey("", "rectangle"));
        auto ptr2 = std::make_shared<Prism>();
        pimpl1.add_module("prism", ptr2);
        REQUIRE(pimpl1.keys().size() == 2);
        REQUIRE(pimpl1.keys().at(0) == QualifiedKey("", "prism"));
        REQUIRE(pimpl1.keys().at(1) == QualifiedKey("", "rectangle"));
    }
}

TEST_CASE("ModuleManagerPIMPL : plugins") {
    ModuleManagerPIMPL pimpl1, pimpl2;
    auto plugin         = std::make_unique<PimplTestPlugin>();
    const auto* pplugin = plugin.get();
    pimpl1.load_plugin(std::move(plugin));

    SECTION("State") {
        REQUIRE(pimpl1.m_plugins.size() == 1);
        REQUIRE(pimpl1.m_plugins[0].get() == pplugin);
        REQUIRE(pimpl1.m_modules.empty());
    }

    SECTION("Plugin modules are given the runtime and caches") {
        auto mod = pplugin->get_modules().at("Rectangle");
        REQUIRE(mod->is_memoizable());
        REQUIRE(mod->run_as<Area>(2.0, 3.0) == 6.0);
    }

    SECTION("Comparison") {
        REQUIRE(pimpl1 != pimpl2);
        pimpl2.load_plugin(std::make_unique<PimplTestPlugin>());
        REQUIRE(pimpl1 == pimpl2);
        pimpl2.get_modules("test_plugin")
          .change_input("Rectangle", "Dimension 1", 1.0);
        REQUIRE(pimpl1 != pimpl2);
    }

    SECTION("count, size, and keys span all maps") {
        pimpl1.add_module("Rectangle", std::make_shared<Rectangle>());
        REQUIRE(pimpl1.count("Rectangle") == 2);
        REQUIRE(pimpl1.size() == 2);
        REQUIRE(pimpl1.keys() == ModuleManagerPIMPL::key_container_type{
                                   QualifiedKey("", "Rectangle"),
                                   QualifiedKey("test_plugin", "Rectangle")});
    }
}

TEST_CASE("ModuleManagerPIMPL : has_cache") {
    ModuleManagerPIMPL pimpl0;
    ModuleManagerPIMPL pimpl1(nullptr, nullptr);
    REQUIRE(pimpl0.has_cache());
    REQUIRE_FALSE(pimpl1.has_cache());
}
