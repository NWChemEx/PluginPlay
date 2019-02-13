#include "examples/TestModuleBase.hpp"
#include <SDE/detail_/ModuleManagerPIMPL.hpp>
#include <catch2/catch.hpp>

using namespace SDE;
using namespace SDE::detail_;

using base_map    = typename ModuleManagerPIMPL::base_map;
using module_map  = typename ModuleManagerPIMPL::module_map;
using cache_map   = typename ModuleManagerPIMPL::cache_map;
using default_map = typename ModuleManagerPIMPL::default_map;

TEST_CASE("ModuleManagerPIMPL : Default ctor") {
    ModuleManagerPIMPL pimpl;
    SECTION("State") {
        REQUIRE(pimpl.size() == 0);
        REQUIRE(pimpl.m_bases == base_map{});
        REQUIRE(pimpl.m_modules == module_map{});
        REQUIRE(pimpl.m_caches == cache_map{});
        REQUIRE(pimpl.m_defaults == default_map{});
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
    }
    SECTION("Add a module") {
        auto ptr                = std::make_shared<Module>();
        pimpl1.m_modules["key"] = ptr;
        REQUIRE(pimpl1 != pimpl2);
        REQUIRE(!(pimpl1 == pimpl2));
    }
}
