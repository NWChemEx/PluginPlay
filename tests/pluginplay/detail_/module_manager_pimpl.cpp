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

#include "../examples/writing_a_module.hpp"
#include "pluginplay/detail_/module_manager_pimpl.hpp"
#include <catch2/catch.hpp>

using namespace pluginplay;
using namespace pluginplay::detail_;

using base_map    = typename ModuleManagerPIMPL::base_map;
using module_map  = typename ModuleManagerPIMPL::module_map;
using default_map = typename ModuleManagerPIMPL::default_map;

TEST_CASE("ModuleManagerPIMPL : Default ctor") {
    ModuleManagerPIMPL pimpl;
    SECTION("State") {
        REQUIRE(pimpl.size() == 0);
        REQUIRE(pimpl.m_bases == base_map{});
        REQUIRE(pimpl.m_modules == module_map{});
        REQUIRE(pimpl.m_defaults == default_map{});
    }
}

TEST_CASE("ModuleManagerPIMPL : Runtime") {
    SECTION("ctor") {
        auto runtime = std::make_shared<parallelzone::runtime::RuntimeView>();
        ModuleManagerPIMPL pimple(runtime);
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
            auto ptr1 = std::make_shared<Module>();
            auto ptr2 = std::make_shared<Module>();
            SECTION("Same key") {
                pimpl1.m_modules["key"] = ptr1;
                pimpl2.m_modules["key"] = ptr2;
                REQUIRE(pimpl1 == pimpl2);
                REQUIRE(!(pimpl1 != pimpl2));
            }
            SECTION("Different key") {
                pimpl1.m_modules["key1"] = ptr1;
                pimpl2.m_modules["key2"] = ptr2;
                REQUIRE(pimpl1 != pimpl2);
                REQUIRE(!(pimpl1 == pimpl2));
            }
        }
    }
    SECTION("Different defaults") {
        SECTION("Different types") {
            pimpl1.m_defaults[std::type_index(typeid(double))] = "key1";
            pimpl2.m_defaults[std::type_index(typeid(int))]    = "key1";
            REQUIRE(pimpl1 != pimpl2);
            REQUIRE(!(pimpl1 == pimpl2));
        }
        SECTION("Different keys") {
            pimpl1.m_defaults[std::type_index(typeid(int))] = "key1";
            pimpl2.m_defaults[std::type_index(typeid(int))] = "key2";
            REQUIRE(pimpl1 != pimpl2);
            REQUIRE(!(pimpl1 == pimpl2));
        }
    }
}

TEST_CASE("ModuleManagerPIMPL : add_module") {
    auto ptr1 = std::make_shared<Rectangle>();
    ModuleManagerPIMPL pimpl1, pimpl2;
    pimpl2.m_bases[ptr1->type()] = ptr1;
    auto pimpl                   = std::make_unique<ModulePIMPL>(ptr1);
    pimpl2.m_modules["key1"]     = std::make_shared<Module>(std::move(pimpl));
    pimpl1.add_module("key1", ptr1);
    REQUIRE(pimpl1 == pimpl2);
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
        pimpl2.m_modules["new_key"] = pimpl2.m_modules["key"];
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

TEST_CASE("ModuleManagerPIMPL : set_default") {
    auto ptr1 = std::make_shared<Rectangle>();
    ModuleManagerPIMPL pimpl1, pimpl2;
    pimpl1.add_module("key", ptr1);
    pimpl2.add_module("key", ptr1);
    SECTION("Can't set to non-exisitant module") {
        REQUIRE_THROWS_AS(
          pimpl1.set_default(typeid(double), type::input_map{}, "not a key"),
          std::out_of_range);
    }
    SECTION("Good key") {
        pimpl1.set_default(typeid(Area), type::input_map{}, "key");
        pimpl2.m_defaults[typeid(Area)] = "key";
        REQUIRE(pimpl1 == pimpl2);
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
        REQUIRE(!pimpl1.m_modules.at("key2")->ready());
        SECTION("No default") {
            auto ptr3 = pimpl1.m_modules.at("key2");
            REQUIRE(ptr3 == pimpl1.at("key2"));
            REQUIRE(!ptr3->ready());
        }
        SECTION("Default") {
            pimpl1.set_default(typeid(Area), ptr1->inputs(), "key1");
            auto old_mod  = *pimpl1.m_modules.at("key2");
            auto smod     = *pimpl1.m_modules.at("key1");
            auto& new_mod = *pimpl1.at("key2");
            REQUIRE(old_mod != new_mod);
            REQUIRE(new_mod.ready(ptr2->inputs()));
            REQUIRE(new_mod.submods().at("area").value() == smod);
        }
    }
}
