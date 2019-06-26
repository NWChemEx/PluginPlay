#include "sde/detail_/module_pimpl.hpp"
#include "test_common.hpp"
#include <catch2/catch.hpp>

using namespace sde;
using namespace sde::detail_;
using namespace testing;
using not_set_t = typename ModulePIMPL::not_set_type;

TEST_CASE("ModulePIMPL : default ctor") {
    ModulePIMPL p;
    REQUIRE_FALSE(p.has_module());
    REQUIRE_FALSE(p.locked());
}

TEST_CASE("ModulePIMPL : has_module") {
    SECTION("No module") {
        ModulePIMPL p;
        REQUIRE_FALSE(p.has_module());
    }
    SECTION("Has module") {
        ModulePIMPL p = make_module_pimpl<NullModule>();
        REQUIRE(p.has_module());
    }
}

TEST_CASE("ModulePIMPL : has_description") {
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

TEST_CASE("ModulePIMPL : locked") {
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

TEST_CASE("ModulePIMPL : not_set") {
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
        not_set_t corr{{"Submodules", std::set<std::string>{"Submodule 1"}}};
        REQUIRE(mod.not_set() == corr);
        SECTION("Setting it fixes it") {
            mod.submods().at("Submodule 1").change(make_module<NullModule>());
            REQUIRE(mod.not_set().empty());
        }
    }
}

TEST_CASE("ModulePIMPL : ready") {
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
            mod.submods().at("Submodule 1").change(make_module<NullModule>());
            REQUIRE(mod.ready());
        }
    }
}

TEST_CASE("ModulePIMPL : lock") {
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

TEST_CASE("ModulePIMPL : unlock") {
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

TEST_CASE("ModulePIMPL : results()") {
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

TEST_CASE("ModulePIMPL : inputs()") {
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

TEST_CASE("ModulePIMPL : submods()") {
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

TEST_CASE("ModulePIMPL : property_types") {
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
        REQUIRE(mod.property_types() == std::set{type::rtti{typeid(NullPT)}});
    }
    SECTION("Doesn't change module base") {
        auto mod_base = std::make_shared<NullModule>();
        ModulePIMPL p(mod_base);
        REQUIRE(&(mod_base->property_types()) != &(p.property_types()));
    }
}

TEST_CASE("ModulePIMPL : description") {
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

TEST_CASE("ModulePIMPL : citations") {
    SECTION("Throws if no impl") {
        ModulePIMPL p;
        REQUIRE_THROWS_AS(p.citations(), std::runtime_error);
    }
    SECTION("Works") {
        auto mod = make_module_pimpl<CiteModule>();
        REQUIRE(mod.citations() == std::vector{std::string{"A citation"}});
    }
}

TEST_CASE("ModulePIMPL : hash") {
    SECTION("Inputs not set") {
        auto mod = make_module_pimpl<NotReadyModule>();
        REQUIRE(hash_objects(mod) == "cbc357ccb763df2852fee8c4fc7d55f2");
    }
    SECTION("Input set") {
        auto mod = make_module_pimpl<NotReadyModule>();
        mod.inputs().at("Option 1").change(3);
        REQUIRE(hash_objects(mod) == "9a4294b64e60cc012c5ed48db4cd9c48");
    }
}

TEST_CASE("ModulePIMPL : is_cached") {
    SECTION("No cache") {
        auto mod = make_module_pimpl<NullModule>();
        REQUIRE_FALSE(mod.is_cached(type::input_map{}));
    }
}

TEST_CASE("ModulePIMPL : run") {
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
        REQUIRE(mod.run(type::input_map{}).at("Result 1").value<int>() == 4);
        SECTION("Locks module") { REQUIRE(mod.locked()); }
    }
}

// Used to test that different implementations compare different
struct NullModule2 : ModuleBase {
    NullModule2() : ModuleBase(this) { satisfies_property_type<NullPT>(); }
    sde::type::result_map run_(sde::type::input_map,
                               sde::type::submodule_map) const override {
        return results();
    }
};

TEST_CASE("ModulePIMPL : comparisons") {
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

TEST_CASE("ModulePIMPL : copy ctor") {
    auto mod = make_module_pimpl<NullModule>();
    ModulePIMPL mod2(mod);
    REQUIRE(mod == mod2);
}

TEST_CASE("ModulePIMPL : copy assignment") {
    auto mod = make_module_pimpl<NullModule>();
    ModulePIMPL mod2;
    auto pmod2 = &(mod2 = mod);
    REQUIRE(pmod2 == &mod2);
    REQUIRE(mod2 == mod);
}

TEST_CASE("ModulePIMPL : move ctor") {
    auto mod = make_module_pimpl<NullModule>();
    ModulePIMPL mod2(mod);
    ModulePIMPL mod3(std::move(mod2));
    REQUIRE(mod == mod3);
}

TEST_CASE("ModulePIMPL : move assignment") {
    auto mod = make_module_pimpl<NullModule>();
    ModulePIMPL mod2;
    ModulePIMPL mod3(mod);
    auto pmod2 = &(mod2 = std::move(mod));
    REQUIRE(pmod2 == &mod2);
    REQUIRE(mod2 == mod3);
}
