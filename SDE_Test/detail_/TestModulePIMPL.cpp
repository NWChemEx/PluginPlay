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

using input_map = typename ModulePIMPL::input_map;

TEST_CASE("ModulePIMPL class") {
    SECTION("Default ctor") {
        ModulePIMPL p;
        REQUIRE(p.ready() == false);

        auto ps = p.problems();
        REQUIRE(ps.count("Algorithm") == 1);
    }

    SECTION("Primary ctor: no submodules") {
        auto ptr = std::make_shared<Rectangle>();
        ModulePIMPL p(ptr);
        REQUIRE(p.ready() == false);

        SECTION("Original problems") {
            auto ps = p.problems();
            REQUIRE(ps.count("Inputs") == 1);
            std::set<std::string> corr{"Dimension 1", "Dimension 2"};
            REQUIRE(ps.at("Inputs") == corr);
        }

        SECTION("Fix one problem") {
            p.inputs().at("Dimension 1").change(1.23);
            auto ps = p.problems();
            REQUIRE(ps.count("Inputs") == 1);
            std::set<std::string> corr{"Dimension 2"};
            REQUIRE(ps.at("Inputs") == corr);
        }

        SECTION("Fix all problems") {
            p.inputs().at("Dimension 1").change(1.23);
            p.inputs().at("Dimension 2").change(4.56);
            auto ps = p.problems();
            REQUIRE(ps.size() == 0);
            REQUIRE(p.ready());
        }

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
    SECTION("Primary ctor: submodule") {
        auto ptr = std::make_shared<Prism>();
        ModulePIMPL p(ptr);
        REQUIRE(!p.ready());

        SECTION("Original problems") {
            auto ps = p.problems();
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

        SECTION("Fix inputs") {
            std::vector<double> dims{1.23, 4.56, 7.89};
            p.inputs().at("Dimensions").change(dims);
            auto ps = p.problems();
            REQUIRE(ps.count("Inputs") == 0);
            REQUIRE(ps.count("Submodules") == 1);
            std::set<std::string> corr{"area"};
            REQUIRE(ps.at("Submodules") == corr);
        }

        SECTION("Fix all problems") {
            std::vector<double> dims{1.23, 4.56, 7.89};
            p.inputs().at("Dimensions").change(dims);

            auto area = std::make_shared<Rectangle>();
            auto mod =
              std::make_shared<Module>(std::make_unique<ModulePIMPL>(area));

            p.submods().at("area").change(mod);
            auto ps = p.problems();
            REQUIRE(ps.size() == 0);
            REQUIRE(p.ready());

            SECTION("Can run it") {
                auto rv = p.run(input_map{});
                REQUIRE(rv.at("volume").value<double>() == 1.23 * 4.56 * 7.89);
            }
        }
    }
}
