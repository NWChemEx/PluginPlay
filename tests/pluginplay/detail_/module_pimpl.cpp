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
#include "pluginplay/detail_/module_pimpl.hpp"
#include <catch2/catch.hpp>
#include <regex>

using namespace pluginplay;
using namespace pluginplay::detail_;
using namespace testing;
using not_set_t = typename ModulePIMPL::not_set_type;

/* I don't see how to test the contents of the time stamp returned by time_stamp
 * without duplicating its contents (and even then there's the possibility that
 * the second call happens in an entirely new year changing basically every
 * value). Thus this unit test simply ensures that the resulting string has the
 * correct format via regex.
 */
TEST_CASE("time_stamp") {
    auto rv = time_stamp();
    std::regex corr("^\\d\\d-\\d\\d-\\d{4} \\d\\d:\\d\\d:\\d\\d\\.\\d{3}$");
    REQUIRE(std::regex_search(rv, corr));
}

// Used to test that different implementations compare different
struct NullModule2 : ModuleBase {
    NullModule2() : ModuleBase(this) { satisfies_property_type<NullPT>(); }
    pluginplay::type::result_map run_(
      pluginplay::type::input_map,
      pluginplay::type::submodule_map) const override {
        return results();
    }
};

TEST_CASE("ModulePIMPL") {
    SECTION("CTors") {
        SECTION("default ctor") {
            ModulePIMPL p;
            REQUIRE_FALSE(p.has_module());
            REQUIRE_FALSE(p.locked());
        }
        SECTION("copy ctor") {
            auto mod = make_module_pimpl<NullModule>();
            ModulePIMPL mod2(mod);
            REQUIRE(mod == mod2);
        }

        SECTION("copy assignment") {
            auto mod = make_module_pimpl<NullModule>();
            ModulePIMPL mod2;
            auto pmod2 = &(mod2 = mod);
            REQUIRE(pmod2 == &mod2);
            REQUIRE(mod2 == mod);
        }

        SECTION("move ctor") {
            auto mod = make_module_pimpl<NullModule>();
            ModulePIMPL mod2(mod);
            ModulePIMPL mod3(std::move(mod2));
            REQUIRE(mod == mod3);
        }

        SECTION("move assignment") {
            auto mod = make_module_pimpl<NullModule>();
            ModulePIMPL mod2;
            ModulePIMPL mod3(mod);
            auto pmod2 = &(mod2 = std::move(mod));
            REQUIRE(pmod2 == &mod2);
            REQUIRE(mod2 == mod3);
        }
    }

    SECTION("has_module") {
        SECTION("No module") {
            ModulePIMPL p;
            REQUIRE_FALSE(p.has_module());
        }
        SECTION("Has module") {
            ModulePIMPL p = make_module_pimpl<NullModule>();
            REQUIRE(p.has_module());
        }
    }

    SECTION("has_description") {
        SECTION("Throws if no impl") {
            ModulePIMPL p;
            REQUIRE_THROWS_AS(p.has_description(), std::runtime_error);
        }
        SECTION("No description") {
            auto mod = make_module_pimpl<NullModule>();
            REQUIRE_FALSE(mod.has_description());
        }
        SECTION("Description") {
            auto mod = make_module_pimpl<DescModule>();
            REQUIRE(mod.has_description());
        }
    }

    SECTION("locked") {
        type::input_map inputs;
        SECTION("Not locked") {
            ModulePIMPL p;
            REQUIRE_FALSE(p.locked());
        }
        SECTION("Is locked") {
            ModulePIMPL p;
            p.lock();
            REQUIRE(p.locked());
        }
    }

    SECTION("not_set") {
        SECTION("No PIMPL") {
            ModulePIMPL p;
            REQUIRE_THROWS_AS(p.not_set(), std::runtime_error);
        }
        SECTION("Is ready") {
            auto mod = make_module_pimpl<NullModule>();
            REQUIRE(mod.not_set().empty());
        }
        SECTION("Not ready input") {
            auto mod = make_module_pimpl<NotReadyModule>();
            not_set_t corr{{"Inputs", std::set<std::string>{"Option 1"}}};
            REQUIRE(mod.not_set() == corr);
            SECTION("Providing input as input fixes it") {
                type::input_map inputs;
                inputs["Option 1"];
                REQUIRE(mod.not_set(inputs).empty());
            }
        }
        SECTION("Not ready submodule") {
            auto mod = make_module_pimpl<SubModModule>();
            not_set_t corr{
              {"Submodules", std::set<std::string>{"Submodule 1"}}};
            REQUIRE(mod.not_set() == corr);
            SECTION("Setting it fixes it") {
                mod.submods()
                  .at("Submodule 1")
                  .change(make_module<NullModule>());
                REQUIRE(mod.not_set().empty());
            }
        }
    }

    SECTION("ready") {
        SECTION("Throws if no PIMPL") {
            ModulePIMPL p;
            REQUIRE_THROWS_AS(p.ready(), std::runtime_error);
        }
        SECTION("Is ready") {
            auto mod = make_module_pimpl<NullModule>();
            REQUIRE(mod.ready());
        }
        SECTION("Not ready input") {
            auto mod = make_module_pimpl<NotReadyModule>();
            REQUIRE_FALSE(mod.ready());
            SECTION("Providing input as input fixes it") {
                type::input_map inputs;
                inputs["Option 1"];
                REQUIRE(mod.ready(inputs));
            }
        }
        SECTION("Not ready submodule") {
            auto mod = make_module_pimpl<SubModModule>();
            REQUIRE_FALSE(mod.ready());
            SECTION("Setting it fixes it") {
                mod.submods()
                  .at("Submodule 1")
                  .change(make_module<NullModule>());
                REQUIRE(mod.ready());
            }
        }
    }

    SECTION("lock") {
        type::input_map inputs;
        SECTION("No submodules") {
            ModulePIMPL p;
            p.lock();
            REQUIRE(p.locked());
        }
        SECTION("Throws if submodule is not ready") {
            auto mod  = make_module_pimpl<SubModModule>();
            auto mod2 = make_module<SubModModule>();
            mod.submods().at("Submodule 1").change(mod2);
            REQUIRE_THROWS_AS(mod.lock(), std::runtime_error);
        }
        SECTION("Submodules") {
            auto mod  = make_module_pimpl<SubModModule>();
            auto mod2 = make_module<NullModule>();
            mod.submods().at("Submodule 1").change(mod2);
            mod.lock();
            REQUIRE(mod.locked());
            REQUIRE(mod2->locked());
        }
    }

    SECTION("unlock") {
        SECTION("unlocked") {
            ModulePIMPL p;
            p.unlock();
            REQUIRE_FALSE(p.locked());
        }
        SECTION("locked") {
            ModulePIMPL p;
            p.lock();
            p.unlock();
            REQUIRE_FALSE(p.locked());
        }
    }

    SECTION("results()") {
        SECTION("Throws if no impl") {
            ModulePIMPL p;
            REQUIRE_THROWS_AS(p.results(), std::runtime_error);
        }
        SECTION("No results") {
            auto mod = make_module_pimpl<NullModule>();
            REQUIRE(mod.results().empty());
        }
        SECTION("Results") {
            auto mod = make_module_pimpl<ResultModule>();
            type::result_map corr;
            corr["Result 1"].set_type<int>();
            REQUIRE(mod.results() == corr);
        }
    }

    SECTION("inputs()") {
        SECTION("No PIMPL") {
            ModulePIMPL p;
            REQUIRE_THROWS_AS(p.inputs(), std::runtime_error);
        }
        SECTION("No inputs") {
            auto mod = make_module_pimpl<NullModule>();
            REQUIRE(mod.inputs().empty());
        }
        SECTION("Inputs") {
            auto mod = make_module_pimpl<NotReadyModule>();
            type::input_map corr;
            corr["Option 1"].set_type<int>();
            REQUIRE(mod.inputs() == corr);
        }
        SECTION("Doesn't change module base") {
            auto mod_base = std::make_shared<NullModule>();
            ModulePIMPL p(mod_base);
            REQUIRE(&(mod_base->inputs()) != &(p.inputs()));
        }
    }

    SECTION("submods()") {
        SECTION("No implementation") {
            ModulePIMPL p;
            REQUIRE_THROWS_AS(p.submods(), std::runtime_error);
        }
        SECTION("No submods") {
            auto mod = make_module_pimpl<testing::NullModule>();
            REQUIRE(mod.submods().empty());
        }
        SECTION("Submods") {
            auto mod = make_module_pimpl<testing::SubModModule>();
            type::submodule_map corr;
            corr["Submodule 1"].set_type<testing::NullPT>();
            REQUIRE(mod.submods() == corr);
        }
        SECTION("Doesn't change module base") {
            auto mod_base = std::make_shared<NullModule>();
            ModulePIMPL p(mod_base);
            REQUIRE(&(mod_base->submods()) != &(p.submods()));
        }
    }

    SECTION("property_types") {
        SECTION("Throws if no impl") {
            ModulePIMPL p;
            REQUIRE_THROWS_AS(p.property_types(), std::runtime_error);
        }
        SECTION("No property types") {
            auto mod = make_module_pimpl<NoPTModule>();
            REQUIRE(mod.property_types().empty());
        }
        SECTION("Has prop types") {
            auto mod = make_module_pimpl<NullModule>();
            REQUIRE(mod.property_types() ==
                    std::set{type::rtti{typeid(NullPT)}});
        }
        SECTION("Doesn't change module base") {
            auto mod_base = std::make_shared<NullModule>();
            ModulePIMPL p(mod_base);
            REQUIRE(&(mod_base->property_types()) != &(p.property_types()));
        }
    }

    SECTION("description") {
        SECTION("Throws if no impl") {
            ModulePIMPL p;
            REQUIRE_THROWS_AS(p.description(), std::runtime_error);
        }
        SECTION("Throws if no description") {
            auto mod = make_module_pimpl<NullModule>();
            REQUIRE_THROWS_AS(mod.description(), std::bad_optional_access);
        }
        SECTION("Works") {
            auto mod = make_module_pimpl<DescModule>();
            REQUIRE(mod.description() == "A description");
        }
    }

    SECTION("citations") {
        SECTION("Throws if no impl") {
            ModulePIMPL p;
            REQUIRE_THROWS_AS(p.citations(), std::runtime_error);
        }
        SECTION("Works") {
            auto mod = make_module_pimpl<CiteModule>();
            REQUIRE(mod.citations() == std::vector{std::string{"A citation"}});
        }
    }

    SECTION("profile_info") {
        ModulePIMPL p = make_module_pimpl<SubModModule>();
        p.submods().at("submodule 1").change(make_module<NullModule>());

        SECTION("Run hasn't been called") {
            std::regex corr("^  Submodule 1[\\r\\n]$");
            REQUIRE(std::regex_search(p.profile_info(), corr));
        }

        SECTION("Run has been called") {
            p.run(pluginplay::type::input_map{});
            std::regex corr(
              "^\\d\\d-\\d\\d-\\d{4} \\d\\d:\\d\\d:\\d\\d\\.\\d{3} : \\d h "
              "\\d m \\d s \\d+ ms[\\r\\n]  Submodule 1[\\r\\n]$");
            REQUIRE(std::regex_search(p.profile_info(), corr));
        }
    }

    SECTION("submod_uuids") {
        using submod_map_type        = ModulePIMPL::submod_uuid_map;
        const std::string submod_key = "Submodule 1";

        // No submodules
        ModulePIMPL no_submod = make_module_pimpl<NullModule>();
        REQUIRE(no_submod.submod_uuids() == submod_map_type{});

        // Submods
        ModulePIMPL submods = make_module_pimpl<SubModModule>();
        auto submod_ptr     = make_module_with_cache<NullModule>();
        submods.submods().at(submod_key).change(submod_ptr);

        submod_map_type corr{{submod_key, submod_ptr->uuid()}};
        REQUIRE(submods.submod_uuids() == corr);

        // Submod w/ Submod
        auto submod_ptr2 = make_module_with_cache<SubModModule>();
        submod_ptr2->change_submod(submod_key, submod_ptr);
        submods.submods().at(submod_key).change(submod_ptr2);

        corr[submod_key]                    = submod_ptr2->uuid();
        corr[submod_key + ":" + submod_key] = submod_ptr->uuid();

        REQUIRE(submods.submod_uuids() == corr);
    }

    SECTION("is_cached") {
        SECTION("No cache") {
            auto mod = make_module_pimpl<NullModule>();
            REQUIRE_FALSE(mod.is_cached(type::input_map{}));
        }

        SECTION("With cache") {
            auto mod = make_module_pimpl_with_cache<RealDeal>();
            auto in  = mod.inputs();
            in.at("Option 1").change(1);
            auto result = mod.run(in).at("Result 1").value<int>();
            REQUIRE(mod.is_cached(in));
        }
    }

    SECTION("reset_cache") {
        auto mod = make_module_pimpl_with_cache<RealDeal>();
        auto in  = mod.inputs();
        in.at("Option 1").change(1);
        auto result1 = mod.run(in).at("Result 1").value<int>();
        REQUIRE(mod.is_cached(in));
        mod.reset_cache();
        REQUIRE_FALSE(mod.is_cached(in));
        auto result2 = mod.run(in).at("Result 1").value<int>();
        REQUIRE(mod.is_cached(in));
    }

    SECTION("reset_internal_cache") {
        auto mod_base_ptr = std::make_shared<testing::NullModule>();
        auto mod_pimpl    = pluginplay::detail_::ModulePIMPL(mod_base_ptr);
        pluginplay::cache::ModuleManagerCache mm_cache;

        auto cache = mm_cache.get_or_make_user_cache("foo");
        cache->cache(int{1}, int{2});
        mod_base_ptr->set_cache(cache);

        mod_pimpl.reset_internal_cache();

        REQUIRE(cache->count(int{1}) == 0);
    }

    SECTION("is_memoizable") {
        SECTION("memoizable") {
            // Isn't memoizable if UUID is not set
            auto mod = make_module_pimpl<NullModule>();
            REQUIRE_FALSE(mod.is_memoizable());

            // Memoizable if UUID is set
            auto mod2 = make_module_pimpl_with_cache<NullModule>();
            REQUIRE(mod2.is_memoizable());
        }
        SECTION("turn off/on memoization") {
            auto mod = make_module_pimpl_with_cache<NullModule>();
            mod.turn_off_memoization();
            REQUIRE_FALSE(mod.is_memoizable());
            mod.turn_on_memoization();
            REQUIRE(mod.is_memoizable());
        }
        SECTION("Submodules") {
            auto mod  = make_module_with_cache<SubModModule>();
            auto mod2 = make_module_with_cache<NullModule>();
            auto mod3 = make_module_with_cache<NullModule>();
            auto mod4 = make_module<NullModule>();
            REQUIRE(mod2.get()->is_memoizable());
            REQUIRE(mod3.get()->is_memoizable());

            // No UUID, so not memoizable
            REQUIRE_FALSE(mod4.get()->is_memoizable());

            mod3.get()->turn_off_memoization();
            REQUIRE_FALSE(mod3.get()->is_memoizable());
            mod.get()->change_submod("Submodule 1", mod3);
            REQUIRE_FALSE(mod.get()->is_memoizable());
            mod.get()->change_submod("Submodule 1", mod2);
            REQUIRE(mod.get()->is_memoizable());

            // Changing it to a non-memoizable submod makes it non-memoizable
            mod.get()->change_submod("Submodule 1", mod4);
            REQUIRE_FALSE(mod.get()->is_memoizable());
        }
    }

    SECTION("run") {
        SECTION("Throws if no implementation") {
            ModulePIMPL p;
            REQUIRE_THROWS_AS(p.run(type::input_map{}), std::runtime_error);
        }
        SECTION("Throws if inputs are not ready") {
            auto mod = make_module_pimpl<NotReadyModule>();
            REQUIRE_THROWS_AS(mod.run(mod.inputs()), std::runtime_error);
        }
        SECTION("Throws if the module is not ready") {
            auto mod = make_module_pimpl<NotReadyModule>();
            REQUIRE_THROWS_AS(mod.run(type::input_map{}), std::runtime_error);
        }
        SECTION("Works") {
            auto mod = make_module_pimpl<ResultModule>();
            REQUIRE(mod.run(type::input_map{}).at("Result 1").value<int>() ==
                    4);
            SECTION("Locks module") { REQUIRE(mod.locked()); }
        }
    }

    SECTION("comparisons") {
        ModulePIMPL p;
        SECTION("Empty") {
            ModulePIMPL p2;
            REQUIRE(p == p2);
            REQUIRE_FALSE(p != p2);
        }
        SECTION("Different module-ness") {
            auto mod = make_module_pimpl<NullModule>();
            REQUIRE(p != mod);
            REQUIRE_FALSE(mod == p);
        }
        SECTION("Different lockedness") {
            auto mod  = make_module_pimpl<NullModule>();
            auto mod2 = make_module_pimpl<NullModule>();
            mod.lock();
            REQUIRE(mod != mod2);
            REQUIRE_FALSE(mod == mod2);
        }
        SECTION("Different inputs") {
            auto mod  = make_module_pimpl<NotReadyModule>();
            auto mod2 = make_module_pimpl<NotReadyModule>();
            mod.inputs().at("Option 1").change(int{3});
            REQUIRE(mod != mod2);
            REQUIRE_FALSE(mod == mod2);
        }
        SECTION("Different submodules") {
            auto mod  = make_module_pimpl<SubModModule>();
            auto mod2 = make_module_pimpl<SubModModule>();
            mod.submods().at("Submodule 1").change(make_module<NullModule>());
            REQUIRE(mod != mod2);
            REQUIRE_FALSE(mod == mod2);
        }
        SECTION("Different Implementations") {
            auto mod  = make_module_pimpl<NullModule>();
            auto mod2 = make_module_pimpl<NullModule2>();
            REQUIRE(mod != mod2);
            REQUIRE_FALSE(mod == mod2);
        }
        SECTION("Different property types") {
            auto mod  = make_module_pimpl<NotReadyModule>();
            auto mod2 = make_module_pimpl<NotReadyModule>();
            mod.property_types().insert(type::rtti{typeid(NullPT)});
            REQUIRE(mod != mod2);
            REQUIRE_FALSE(mod == mod2);
        }
    }
}
