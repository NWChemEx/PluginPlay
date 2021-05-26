#include "test_common.hpp"
#include <catch2/catch.hpp>
#include <sde/module_base.hpp>

using namespace sde;

static const auto null_pt_rtti = std::type_index(typeid(testing::NullPT));

namespace {
DECLARE_MODULE(NotCached);
inline MODULE_CTOR(NotCached) { satisfies_property_type<testing::NullPT>(); }
inline MODULE_RUN(NotCached) {
    auto x = uncache_data<int>(std::make_tuple(int{1}, int{2}));
    return results();
}

DECLARE_MODULE(IsCached);
inline MODULE_CTOR(IsCached) { satisfies_property_type<testing::OneOut>(); }
inline MODULE_RUN(IsCached) {
    auto t = std::make_tuple(int{1}, int{2});
    cache_data(t, int{3});
    auto x  = uncache_data<int>(t);
    auto rv = results();
    return testing::OneOut::wrap_results(rv, x);
}

DECLARE_MODULE(CachedWithDefault);
inline MODULE_CTOR(CachedWithDefault) {
    satisfies_property_type<testing::OneOut>();
    satisfies_property_type<testing::OneIn>();
}
inline MODULE_RUN(CachedWithDefault) {
    // Returns 4 unless the input is 2, in which case it returns 3
    auto [i] = testing::OneIn::unwrap_inputs(inputs);
    auto t   = std::make_tuple(int{1}, i);
    cache_data(std::make_tuple(int{1}, int{2}), int{3});
    auto x  = uncache_data<int>(t, int{4});
    auto rv = results();
    return testing::OneOut::wrap_results(rv, x);
}

} // namespace

TEST_CASE("ModuleBase : run") {
    testing::ResultModule mod;
    type::result_map corr;
    corr["Result 1"].set_type<int>().change(4);
    REQUIRE(mod.run(type::input_map{}, type::submodule_map{}) == corr);
}

TEST_CASE("ModuleBase : has_description") {
    SECTION("No description") {
        testing::NullModule mod;
        REQUIRE_FALSE(mod.has_description());
    }

    SECTION("Has description") {
        testing::DescModule mod;
        REQUIRE(mod.has_description());
    }
}

TEST_CASE("ModuleBase : results()") {
    SECTION("No results") {
        testing::NullModule mod;
        REQUIRE(mod.results().empty());
    }
    SECTION("Results") {
        testing::ResultModule mod;
        type::result_map corr;
        corr["Result 1"].set_type<int>();
        REQUIRE(mod.results() == corr);
    }
}

TEST_CASE("ModuleBase : inputs()") {
    SECTION("No inputs") {
        testing::NullModule mod;
        REQUIRE(mod.inputs().empty());
    }
    SECTION("Inputs") {
        testing::NotReadyModule mod;
        type::input_map corr;
        corr["Option 1"].set_type<int>();
        REQUIRE(mod.inputs() == corr);
    }
}

TEST_CASE("ModuleBase : submods()") {
    SECTION("No submods") {
        testing::NullModule mod;
        REQUIRE(mod.submods().empty());
    }
    SECTION("Submods") {
        testing::SubModModule mod;
        type::submodule_map corr;
        corr["Submodule 1"].set_type<testing::NullPT>();
        REQUIRE(mod.submods() == corr);
    }
}

TEST_CASE("ModuleBase : property_types()") {
    SECTION("No property type") {
        testing::NoPTModule mod;
        REQUIRE(mod.property_types().empty());
    }
    SECTION("Has Property type") {
        testing::NotReadyModule mod;
        type::rtti corr(typeid(testing::OneIn));
        REQUIRE(mod.property_types() == std::set{corr});
    }
}

TEST_CASE("ModuleBase : type") {
    testing::NotReadyModule mod;
    REQUIRE(mod.type() == type::rtti(typeid(testing::NotReadyModule)));
}

TEST_CASE("ModuleBase : get_desc") {
    SECTION("Throws if no description") {
        testing::NullModule mod;
        REQUIRE_THROWS_AS(mod.get_desc(), std::bad_optional_access);
    }
    SECTION("Get description") {
        testing::DescModule mod;
        REQUIRE(mod.get_desc() == "A description");
    }
}

TEST_CASE("ModuleBase : citations") {
    SECTION("No citations") {
        testing::NullModule mod;
        REQUIRE(mod.citations().empty());
    }
    SECTION("Citations") {
        testing::CiteModule mod;
        REQUIRE(mod.citations() == std::vector{std::string("A citation")});
    }
}

TEST_CASE("ModuleBase : internal cacheing") {
    using cache_type = typename ModuleBase::cache_type;
    auto ptr         = std::make_shared<cache_type>();

    type::input_map inputs;
    type::submodule_map submods;

    SECTION("Throws if no Cache") {
        SECTION("By calling uncache_data") {
            NotCached m;
            REQUIRE_THROWS_AS(m.run(inputs, submods), std::runtime_error);
        }

        SECTION("By calling cache_data") {
            IsCached m;
            REQUIRE_THROWS_AS(m.run(inputs, submods), std::runtime_error);
        }
    }

    SECTION("Throws if not in cache") {
        NotCached m;
        m.set_cache(ptr);
        REQUIRE_THROWS_AS(m.run(inputs, submods), std::out_of_range);
    }

    SECTION("Uncache no default") {
        IsCached m;
        m.set_cache(ptr);
        auto rv = m.run(inputs, submods);
        REQUIRE(rv.at("Result 1").value<int>() == int{3});
    }

    SECTION("Uncache with default") {
        CachedWithDefault m;
        m.set_cache(ptr);

        SECTION("Returns default value") {
            ModuleInput inp;
            inp.set_type<int>().set_default(4);
            inputs["Option 1"] = inp;
            auto rv            = m.run(inputs, submods);
            REQUIRE(rv.at("Result 1").value<int>() == int{4});
        }

        SECTION("Returns cached value") {
            ModuleInput inp;
            inp.set_type<int>().set_default(2);
            inputs["Option 1"] = inp;
            auto rv            = m.run(inputs, submods);
            REQUIRE(rv.at("Result 1").value<int>() == int{3});
        }
    }
}

TEST_CASE("ModuleBase : Comparisons") {
    testing::NullModule mod;
    SECTION("Same module") {
        testing::NullModule mod2;
        REQUIRE(mod == mod2);
        REQUIRE_FALSE(mod != mod2);
    }
    SECTION("Different modules") {
        testing::NotReadyModule mod2;
        REQUIRE_FALSE(mod == mod2);
        REQUIRE(mod != mod2);
    }
}
