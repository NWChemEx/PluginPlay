#include "examples/TestModuleBase.hpp"
#include <SDE/detail_/ModulePIMPL.hpp>
#include <catch2/catch.hpp>
using namespace SDE::detail_;

/* For simplicity we use the modules defined in the examples, but this test is
 * not considered part of the examples since the ModulePIMPL class is an
 * implementation detail.
 */

void check_state(ModulePIMPL& p, bool ready, bool locked) {
    REQUIRE(p.ready() == ready);
    REQUIRE(p.is_locked() == locked);
}

TEST_CASE("ModulePIMPL class") {
    SECTION("Default ctor") {
        ModulePIMPL p;
        check_state(p, false, false);

        auto ps = p.problems();
        REQUIRE(ps.count("Algorithm") == 1);

        SECTION("Can't lock a non-ready PIMPL") {
            REQUIRE_THROWS_AS(p.lock(), std::runtime_error);
        }
    }

    SECTION("Primary ctor") {
        auto ptr = std::make_shared<Rectangle>() ModulePIMPL p(ptr);
        check_state(p, false, false);

        SECTION("Original problems") {
            auto ps = p.problems();
            REQUIRE(ps.count("Inputs") == 1);
            std::set<std::string> corr{"Dimension1", "Dimension2"};
            REQUIRE(ps.at("Inputs") == corr);
        }

        SECTION("Fix one problem") {
            p.inputs().at("Dimension1").change(1.23);
            auto ps = p.problems();
            REQUIRE(ps.count("Inputs") == 1);
            std::set<std::string> corr{"Dimension2"};
            REQUIRE(ps.at("Inputs") == corr);
        }

        SECTION("Fix all problems") {
            p.inputs().at("Dimension1").change(1.23);
            p.inputs().at("Dimension2").change(4.56);
            auto ps = p.problems();
            REQUIRE(ps.size() == 0);
            check_state(p, true, false);
        }

        p.inputs().at("Dimension1").change(1.23);
        p.inputs().at("Dimension2").change(4.56);

        SECTION("Can lock ready module") {
            p.lock();
            check_state(p, true, true);
        }

        SECTION("Running module locks it") {
            auto rv = p.run(input_map{});
            check_state(p, true, true);
            REQUIRE(rv.at("Area").value<double> == 1.23 * 4.56);
        }
    }
}
