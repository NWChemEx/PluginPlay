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
#include <pluginplay/module.hpp>
#include <regex>

using namespace pluginplay;
using namespace testing;
using not_set_t = typename Module::not_ready_type;

TEST_CASE("Module : default ctor") {
    Module p;
    REQUIRE_FALSE(p.has_module());
    REQUIRE_FALSE(p.locked());
}

TEST_CASE("Module : unlocked_copy") {
    SECTION("Copy an unlocked module") {
        Module p;
        auto p2 = p.unlocked_copy();
        REQUIRE(p == p2);
        REQUIRE_FALSE(p.locked());
    }
    SECTION("Copy a locked module") {
        Module p, p3;
        p.lock();
        auto p2 = p.unlocked_copy();
        REQUIRE_FALSE(p2.locked());
        REQUIRE(p != p2);
        REQUIRE(p2 == p3);
    }
}

TEST_CASE("Module : has_module") {
    SECTION("No module") {
        Module p;
        REQUIRE_FALSE(p.has_module());
    }
    SECTION("Has module") {
        auto p = make_module<NullModule>();
        REQUIRE(p->has_module());
    }
}

TEST_CASE("Module : has_description") {
    SECTION("Throws if no impl") {
        Module p;
        REQUIRE_THROWS_AS(p.has_description(), std::runtime_error);
    }
    SECTION("No description") {
        auto mod = make_module<NullModule>();
        REQUIRE_FALSE(mod->has_description());
    }
    SECTION("Description") {
        auto mod = make_module<DescModule>();
        REQUIRE(mod->has_description());
    }
}

TEST_CASE("Module : locked") {
    Module p;
    SECTION("Not locked") { REQUIRE_FALSE(p.locked()); }
    SECTION("Is locked") {
        p.lock();
        REQUIRE(p.locked());
    }
}

TEST_CASE("Module : list_not_ready") {
    SECTION("No PIMPL") {
        Module p;
        REQUIRE_THROWS_AS(p.list_not_ready(), std::runtime_error);
    }
    SECTION("Is ready") {
        auto mod = make_module<NullModule>();
        REQUIRE(mod->list_not_ready().empty());
    }
    SECTION("Not ready because of input") {
        auto mod = make_module<NotReadyModule>();
        not_set_t corr{{"Inputs", std::set<std::string>{"Option 1"}}};
        REQUIRE(mod->list_not_ready() == corr);
        SECTION("Providing input as input fixes it") {
            type::input_map inputs;
            inputs["Option 1"];
            REQUIRE(mod->list_not_ready(inputs).empty());
        }
    }
    SECTION("Not ready because of submodule") {
        auto mod = make_module<SubModModule>();
        not_set_t corr{{"Submodules", std::set<std::string>{"Submodule 1"}}};
        REQUIRE(mod->list_not_ready() == corr);
        SECTION("Setting it fixes it") {
            mod->change_submod("Submodule 1", make_module<NullModule>());
            REQUIRE(mod->list_not_ready().empty());
        }
    }
}

TEST_CASE("Module : ready") {
    SECTION("Throws if no algorithm") {
        Module p;
        REQUIRE_THROWS_AS(p.ready(), std::runtime_error);
    }
    SECTION("Is ready because no arguments") {
        auto mod = make_module<NullModule>();
        REQUIRE(mod->ready());
    }
    SECTION("Is ready because all inputs are defaulted") {
        auto mod = make_module<ReadyModule>();
        REQUIRE(mod->ready());
    }
    SECTION("Not ready because of input") {
        auto mod = make_module<NotReadyModule>();
        REQUIRE_FALSE(mod->ready());
        SECTION("Providing missing input makes it ready") {
            type::input_map inputs;
            inputs["Option 1"];
            REQUIRE(mod->ready(inputs));
        }
    }
    SECTION("Not ready because of missing submodule") {
        auto mod = make_module<SubModModule>();
        REQUIRE_FALSE(mod->ready());
        SECTION("Setting submodule fixes it") {
            mod->change_submod("Submodule 1", make_module<NullModule>());
            REQUIRE(mod->ready());
        }
    }
    SECTION("Checking with property type works") {
        auto mod = make_module<NotReadyModule>();
        SECTION("Still not ready if property type doesn't fix problem") {
            REQUIRE_FALSE(mod->ready<NullPT>());
        }
        SECTION("Ready if property type fixes problem") {
            REQUIRE(mod->ready<OneIn>());
        }
    }
}

TEST_CASE("Module : reset_cache") {
    auto mod_pimpl = make_module_pimpl_with_cache<RealDeal>();
    auto mod       = pluginplay::Module(
      std::make_unique<pluginplay::detail_::ModulePIMPL>(mod_pimpl));

    auto in = mod.inputs();
    in.at("Option 1").change(1);

    auto result1 = mod.run(in).at("Result 1").value<int>();
    REQUIRE(mod_pimpl.is_cached(in));
    mod.reset_cache();
    REQUIRE_FALSE(mod_pimpl.is_cached(in));
    auto result2 = mod.run(in).at("Result 1").value<int>();
    REQUIRE(mod_pimpl.is_cached(in));
}

TEST_CASE("Module : reset_internal_cache") {
    auto mod_base_ptr = std::make_shared<testing::NullModule>();
    auto mod          = pluginplay::Module(
      std::make_unique<pluginplay::detail_::ModulePIMPL>(mod_base_ptr));

    pluginplay::cache::ModuleManagerCache mm_cache;
    auto cache = mm_cache.get_or_make_user_cache("foo");
    cache->cache(int{1}, int{2});
    mod_base_ptr->set_cache(cache);

    mod.reset_internal_cache();

    REQUIRE(cache->count(int{1}) == 0);
}

TEST_CASE("Module : lock") {
    type::input_map inputs;
    SECTION("Can lock if no submodules") {
        Module p;
        p.lock();
        REQUIRE(p.locked());
    }
    SECTION("Throws if submodules are not ready") {
        auto mod  = make_module<SubModModule>();
        auto mod2 = make_module<SubModModule>();
        mod->change_submod("Submodule 1", mod2);
        REQUIRE_THROWS_AS(mod->lock(), std::runtime_error);
    }
    SECTION("Submodules are ready") {
        auto mod  = make_module<SubModModule>();
        auto mod2 = make_module<NullModule>();
        mod->change_submod("Submodule 1", mod2);
        mod->lock();
        REQUIRE(mod->locked());
        SECTION("Also locks submodule") { REQUIRE(mod2->locked()); }
    }
}

TEST_CASE("Module : results()") {
    SECTION("Throws if no impl") {
        Module p;
        REQUIRE_THROWS_AS(p.results(), std::runtime_error);
    }
    SECTION("Module has no results") {
        auto mod = make_module<NullModule>();
        REQUIRE(mod->results().empty());
    }
    SECTION("Module has results") {
        auto mod = make_module<ResultModule>();
        type::result_map corr;
        corr["Result 1"].set_type<int>();
        REQUIRE(mod->results() == corr);
    }
}

TEST_CASE("Module : inputs()") {
    SECTION("Implementation is not set") {
        Module p;
        REQUIRE_THROWS_AS(p.inputs(), std::runtime_error);
    }
    SECTION("Module accepts no inputs") {
        auto mod = make_module<NullModule>();
        REQUIRE(mod->inputs().empty());
    }
    SECTION("Module has inputs") {
        auto mod = make_module<NotReadyModule>();
        type::input_map corr;
        corr["Option 1"].set_type<int>();
        REQUIRE(mod->inputs() == corr);
    }
}

TEST_CASE("Module : submods()") {
    SECTION("No implementation") {
        Module p;
        REQUIRE_THROWS_AS(p.submods(), std::runtime_error);
    }
    SECTION("No submods") {
        auto mod = make_module<testing::NullModule>();
        REQUIRE(mod->submods().empty());
    }
    SECTION("Has submods") {
        auto mod = make_module<testing::SubModModule>();
        type::submodule_map corr;
        corr["Submodule 1"].set_type<testing::NullPT>();
        REQUIRE(mod->submods() == corr);
    }
}

TEST_CASE("Module : property_types") {
    SECTION("Throws if no implementation") {
        Module p;
        REQUIRE_THROWS_AS(p.property_types(), std::runtime_error);
    }
    SECTION("No property types") {
        auto mod = make_module<NoPTModule>();
        REQUIRE(mod->property_types().empty());
    }
    SECTION("Has property types") {
        auto mod = make_module<NullModule>();
        REQUIRE(mod->property_types() == std::set{type::rtti{typeid(NullPT)}});
    }
}

TEST_CASE("Module : change_input") {
    const std::string key{"Option 1"};
    const int value = 3;
    SECTION("Throws if locked") {
        Module p;
        p.lock();
        REQUIRE_THROWS_AS(p.change_input(key, value), std::runtime_error);
    }
    SECTION("Throws if no implementation") {
        Module p;
        REQUIRE_THROWS_AS(p.change_input(key, value), std::runtime_error);
    }
    SECTION("Throws if input does not exist") {
        auto mod = make_module<NotReadyModule>();
        REQUIRE_THROWS_AS(mod->change_input("Not a key", 3), std::out_of_range);
    }
    SECTION("Throws if value is wrong type") {
        auto mod = make_module<NotReadyModule>();
        REQUIRE_THROWS_AS(mod->change_input(key, key), std::invalid_argument);
    }
    SECTION("Works") {
        auto mod = make_module<NotReadyModule>();
        mod->change_input(key, value);
        REQUIRE(mod->inputs().at(key).value<int>() == value);
    }
}

TEST_CASE("Module : change_submod") {
    const std::string key{"Submodule 1"};
    auto mod   = make_module<SubModModule>();
    auto value = make_module<NullModule>();
    SECTION("Throws if locked") {
        Module p;
        REQUIRE_THROWS_AS(p.change_submod(key, value), std::runtime_error);
    }
    SECTION("Throws if no implementation") {
        Module p;
        REQUIRE_THROWS_AS(p.change_submod(key, value), std::runtime_error);
    }
    SECTION("Throws if request does not exist") {
        REQUIRE_THROWS_AS(mod->change_submod("Not a key", value),
                          std::out_of_range);
    }
    SECTION("Throws if value is wrong type") {
        auto mod2 = make_module<NotReadyModule>();
        REQUIRE_THROWS_AS(mod->change_submod(key, mod2), std::runtime_error);
    }
    SECTION("Works") {
        mod->change_submod(key, value);
        REQUIRE(mod->submods().at(key).value() == *value);
    }
}

TEST_CASE("Module : add_property_type") {
    using T = OneIn;
    SECTION("Throws if locked") {
        Module p;
        REQUIRE_THROWS_AS(p.add_property_type<T>(), std::runtime_error);
    }
    SECTION("Throws if no implementation") {
        Module p;
        REQUIRE_THROWS_AS(p.add_property_type<T>(), std::runtime_error);
    }
    SECTION("Works") {
        auto mod = make_module<NullModule>();
        mod->add_property_type<T>();
        std::set corr{type::rtti{typeid(NullPT)}, type::rtti{typeid(T)}};
        REQUIRE(mod->property_types() == corr);
    }
}

TEST_CASE("Module : description") {
    SECTION("Throws if no impl") {
        Module p;
        REQUIRE_THROWS_AS(p.description(), std::runtime_error);
    }
    SECTION("Throws if no description") {
        auto mod = make_module<NullModule>();
        REQUIRE_THROWS_AS(mod->description(), std::bad_optional_access);
    }
    SECTION("Works") {
        auto mod = make_module<DescModule>();
        REQUIRE(mod->description() == "A description");
    }
}

TEST_CASE("Module : citations") {
    SECTION("Throws if no impl") {
        Module p;
        REQUIRE_THROWS_AS(p.citations(), std::runtime_error);
    }
    SECTION("Works") {
        auto mod = make_module<CiteModule>();
        REQUIRE(mod->citations() == std::vector{std::string{"A citation"}});
    }
}

TEST_CASE("Module : run_as") {
    SECTION("Throws if it module doesn't satisfy property type") {
        Module p;
        REQUIRE_THROWS_AS(p.run_as<NullPT>(), std::runtime_error);
    }
    SECTION("Throws if inputs are not ready") {
        auto mod = make_module<NotReadyModule>();
        REQUIRE_THROWS_AS(mod->run(mod->inputs()), std::runtime_error);
    }
    SECTION("Throws if the module is not ready") {
        auto mod = make_module<NotReadyModule>();
        mod->add_property_type<NullPT>();
        REQUIRE_THROWS_AS(mod->run_as<NullPT>(), std::runtime_error);
    }
    SECTION("Can call defaulted module without arg") {
        auto mod = make_module<ReadyModule>();
        REQUIRE(mod->run_as<OptionalInput>() == 1);
    }
    SECTION("Can call defaulted module without arg") {
        auto mod = make_module<ReadyModule>();
        REQUIRE(mod->run_as<OptionalInput>(42) == 42);
    }
    SECTION("Can call use derived classes polymorphically") {
        auto mod = make_module<PolymorphicModule>();
        SECTION("Works with base class") {
            testing::BaseClass b;
            REQUIRE_NOTHROW(mod->run_as<PolymorphicOptions>(b));
        }
        SECTION("Works with derived class") {
            testing::DerivedClass c;
            REQUIRE_NOTHROW(mod->run_as<PolymorphicOptions>(c));
        }
    }
    SECTION("Works") {
        auto mod = make_module<ResultModule>();
        REQUIRE(mod->run_as<OneOut>() == 4);
        SECTION("Locks module") { REQUIRE(mod->locked()); }
    }
}

TEST_CASE("Module : run") {
    SECTION("Throws if no implementation") {
        Module p;
        REQUIRE_THROWS_AS(p.run(), std::runtime_error);
    }
    SECTION("Throws if inputs are not ready") {
        auto mod = make_module<NotReadyModule>();
        REQUIRE_THROWS_AS(mod->run(mod->inputs()), std::runtime_error);
    }
    SECTION("Throws if the module is not ready") {
        auto mod = make_module<NotReadyModule>();
        REQUIRE_THROWS_AS(mod->run(), std::runtime_error);
    }
    SECTION("Works") {
        auto mod = make_module<ResultModule>();
        REQUIRE(mod->run().at("Result 1").value<int>() == 4);
        SECTION("Locks module") { REQUIRE(mod->locked()); }
    }
}

TEST_CASE("Module : profile_info") {
    auto p = make_module<SubModModule>();
    p->change_submod("submodule 1", make_module<NullModule>());

    SECTION("Run hasn't been called") {
        std::regex corr("^  Submodule 1[\\r\\n]$");
        REQUIRE(std::regex_search(p->profile_info(), corr));
    }

    SECTION("Run has been called") {
        p->run(pluginplay::type::input_map{});
        std::regex corr(
          "^\\d\\d-\\d\\d-\\d{4} \\d\\d:\\d\\d:\\d\\d\\.\\d{3} : \\d h "
          "\\d m \\d s \\d+ ms[\\r\\n]  Submodule 1[\\r\\n]$");
        REQUIRE(std::regex_search(p->profile_info(), corr));
    }
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

TEST_CASE("Module : comparisons") {
    Module p;
    SECTION("Empty") {
        Module p2;
        REQUIRE(p == p2);
        REQUIRE_FALSE(p != p2);
    }
    SECTION("Different module-ness") {
        auto mod = make_module<NullModule>();
        REQUIRE(p != *mod);
        REQUIRE_FALSE(*mod == p);
    }
    SECTION("Different lockedness") {
        auto mod  = make_module<NullModule>();
        auto mod2 = make_module<NullModule>();
        mod->lock();
        REQUIRE(*mod != *mod2);
        REQUIRE_FALSE(*mod == *mod2);
    }
    SECTION("Different inputs") {
        auto mod  = make_module<NotReadyModule>();
        auto mod2 = make_module<NotReadyModule>();
        mod->change_input("Option 1", int{3});
        REQUIRE(*mod != *mod2);
        REQUIRE_FALSE(*mod == *mod2);
    }
    SECTION("Different submodules") {
        auto mod  = make_module<SubModModule>();
        auto mod2 = make_module<SubModModule>();
        mod->change_submod("Submodule 1", make_module<NullModule>());
        REQUIRE(*mod != *mod2);
        REQUIRE_FALSE(*mod == *mod2);
    }
    SECTION("Different Implementations") {
        auto mod  = make_module<NullModule>();
        auto mod2 = make_module<NullModule2>();
        REQUIRE(*mod != *mod2);
        REQUIRE_FALSE(*mod == *mod2);
    }
    SECTION("Different property types") {
        auto mod  = make_module<NotReadyModule>();
        auto mod2 = make_module<NotReadyModule>();
        mod->add_property_type<NullPT>();
        REQUIRE(*mod != *mod2);
        REQUIRE_FALSE(*mod == *mod2);
    }
}

TEST_CASE("Module : copy ctor") {
    auto mod = make_module<NullModule>();
    Module mod2(*mod);
    REQUIRE(*mod == mod2);
}

TEST_CASE("Module : copy assignment") {
    auto mod = make_module<NullModule>();
    Module mod2;
    auto pmod2 = &(mod2 = *mod);
    REQUIRE(pmod2 == &mod2);
    REQUIRE(mod2 == *mod);
}

TEST_CASE("Module : move ctor") {
    auto mod = make_module<NullModule>();
    Module mod2(*mod);
    Module mod3(std::move(mod2));
    REQUIRE(*mod == mod3);
}

TEST_CASE("Module : move assignment") {
    auto mod = make_module<NullModule>();
    Module mod2;
    Module mod3(*mod);
    auto pmod2 = &(mod2 = std::move(*mod));
    REQUIRE(pmod2 == &mod2);
    REQUIRE(mod2 == mod3);
}
