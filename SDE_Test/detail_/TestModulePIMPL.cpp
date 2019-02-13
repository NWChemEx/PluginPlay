#include "examples/TestModuleBase.hpp"
#include <SDE/Module.hpp>
#include <SDE/detail_/ModulePIMPL.hpp>
#include <catch2/catch.hpp>
using namespace SDE;
using namespace SDE::detail_;

/* For simplicity we use the modules defined in the examples, but this test is
 * not considered part of the examples since the ModulePIMPL class is an
 * implementation detail.
 */

using input_map  = type::input_map;
using result_map = type::result_map;

TEST_CASE("ModulePIMPL class : Default ctor") {
    ModulePIMPL p;
    SECTION("State") {
        REQUIRE(!p.ready());
        REQUIRE(!p.locked());
        REQUIRE(p.inputs() == input_map{});
        SECTION("Problems") {
            auto ps = p.not_set();
            REQUIRE(ps.count("Algorithm") == 1);
        }
    }
    SECTION("Error checking") {
        REQUIRE_THROWS_AS(p.results(), std::runtime_error);
    }
}

TEST_CASE("ModulePIMPL class: primary ctor") {
    SECTION("No submodules") {
        auto ptr = std::make_shared<Rectangle>();
        ModulePIMPL p(ptr);
        SECTION("State") {
            REQUIRE(p.ready());
            REQUIRE(!p.locked());
            SECTION("Problems") {
                auto ps = p.not_set();
                REQUIRE(ps.count("Inputs") == 1);
                std::set<std::string> corr{"Dimension 1", "Dimension 2"};
                REQUIRE(ps.at("Inputs") == corr);
            }
        }

        SECTION("Fix one problem") {
            p.inputs().at("Dimension 1").change(1.23);
            auto ps = p.not_set(p.inputs());
            REQUIRE(ps.count("Inputs") == 1);
            std::set<std::string> corr{"Dimension 2"};
            REQUIRE(ps.at("Inputs") == corr);
        }

        SECTION("Fix all problems") {
            p.inputs().at("Dimension 1").change(1.23);
            p.inputs().at("Dimension 2").change(4.56);
            auto ps = p.not_set(p.inputs());
            REQUIRE(ps.empty());
            REQUIRE(p.ready());
        }
    }
    SECTION("Submodules") {
        auto ptr = std::make_shared<Prism>();
        ModulePIMPL p(ptr);
        SECTION("State") {
            REQUIRE(!p.ready());
            REQUIRE(!p.locked());
            SECTION("Problems") {
                auto ps = p.not_set();
                SECTION("Inputs") {
                    REQUIRE(ps.count("Inputs") == 1);
                    std::set<std::string> corr{"Dimensions"};
                    REQUIRE(ps.at("Inputs") == corr);
                }
                SECTION("Submodules") {
                    REQUIRE(ps.count("Submodules") == 1);
                    std::set<std::string> corr{"area"};
                    REQUIRE(ps.at("Submodules") == corr);
                }
            }
        }

        SECTION("Fix inputs") {
            std::vector<double> dims{1.23, 4.56, 7.89};
            p.inputs().at("Dimensions").change(dims);
            auto ps = p.not_set(p.inputs());
            REQUIRE(ps.count("Inputs") == 0);
            REQUIRE(ps.count("Submodules") == 1);
            std::set<std::string> corr{"area"};
            REQUIRE(ps.at("Submodules") == corr);
        }

        SECTION("Fix all problems") {
            std::vector<double> dims{1.23, 4.56, 7.89};
            p.inputs().at("Dimensions").change(dims);
            auto area  = std::make_shared<Rectangle>();
            auto parea = std::make_unique<ModulePIMPL>(area);
            auto mod   = std::make_shared<Module>(std::move(parea));
            p.submods().at("area").change(mod);
            auto ps = p.not_set(p.inputs());
            REQUIRE(ps.empty());
            REQUIRE(p.ready());
        }
    }
}

TEST_CASE("ModulePIMPL class : run") {
    SECTION("No submodules") {
        auto ptr = std::make_shared<Rectangle>();
        ModulePIMPL p(ptr);
        p.inputs().at("Dimension 1").change(1.23);
        p.inputs().at("Dimension 2").change(4.56);

        SECTION("Can run it") {
            auto rv = p.run(input_map{});
            REQUIRE(rv.at("Area").value<double>() == 1.23 * 4.56);
        }

        SECTION("Does not change inputs of ModuleBase") {
            REQUIRE(ptr->inputs() != p.inputs());
        }
    }

    SECTION("Submodules") {
        auto ptr = std::make_shared<Prism>();
        ModulePIMPL p(ptr);
        std::vector<double> dims{1.23, 4.56, 7.89};
        p.inputs().at("Dimensions").change(dims);
        auto area  = std::make_shared<Rectangle>();
        auto parea = std::make_unique<ModulePIMPL>(area);
        auto mod   = std::make_shared<Module>(std::move(parea));
        p.submods().at("area").change(mod);
        auto rv = p.run(input_map{});
        REQUIRE(rv.at("volume").value<double>() == 1.23 * 4.56 * 7.89);
    }
}
