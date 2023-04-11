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
#include <parallelzone/runtime/runtime_view.hpp>
#include <pluginplay/module_base.hpp>

using namespace pluginplay;

static const auto null_pt_rtti = std::type_index(typeid(testing::NullPT));

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

TEST_CASE("ModuleBase : get_cache") {
    testing::NullModule mod;
    SECTION("Throws if no cache") {
        REQUIRE_THROWS_AS(mod.get_cache(), std::runtime_error);
    }

    SECTION("Works if there's a cache") {
        auto cache = std::make_shared<pluginplay::cache::UserCache>();
        mod.set_cache(cache);
        auto& internal_cache = mod.get_cache();
        REQUIRE(&internal_cache == cache.get());
    }
}

TEST_CASE("ModuleBase : get_runtime") {
    testing::NullModule mod;
    SECTION("Throws if no runtime") {
        REQUIRE_THROWS_AS(mod.get_runtime(), std::runtime_error);
    }

    SECTION("Works if there's a runtime") {
        auto runtime = std::make_shared<parallelzone::runtime::RuntimeView>();
        mod.set_runtime(runtime);
        auto& internal_runtime = mod.get_runtime();
        REQUIRE(&internal_runtime == runtime.get());
    }
}

TEST_CASE("ModuleBase : reset_internal_cache") {
    testing::NullModule mod;

    pluginplay::cache::ModuleManagerCache mm_cache;
    auto cache = mm_cache.get_or_make_user_cache("foo");
    cache->cache(int{1}, int{2});
    mod.set_cache(cache);
    mod.reset_internal_cache();
    auto& internal_cache = mod.get_cache();
    REQUIRE(internal_cache.count(int{1}) == 0);
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
