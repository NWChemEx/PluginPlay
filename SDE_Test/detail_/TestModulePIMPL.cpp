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
using cache_type = typename ModulePIMPL::cache_type;

inline std::shared_ptr<Module> area_module() {
    auto area  = std::make_shared<Rectangle>();
    auto parea = std::make_unique<ModulePIMPL>(area);
    return std::make_shared<Module>(std::move(parea));
}

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

TEST_CASE("ModulePIMPL class : primary ctor") {
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
    }
}

TEST_CASE("ModulePIMPL class : equality") {
    SECTION("Default ctor") {
        ModulePIMPL pimpl1, pimpl2;
        REQUIRE(pimpl1 == pimpl2);
        REQUIRE(!(pimpl1 != pimpl2));
        SECTION("inputs") {
            pimpl1.inputs()["key"] = ModuleInput{};
            REQUIRE(pimpl1 != pimpl2);
            REQUIRE(!(pimpl1 == pimpl2));
        }
        SECTION("Submods") {
            pimpl1.submods()["key"] = SubmoduleRequest{};
            REQUIRE(pimpl1 != pimpl2);
            REQUIRE(!(pimpl1 == pimpl2));
        }
    }
    SECTION("Primary construction") {
        auto ptr = std::make_shared<Rectangle>();
        ModulePIMPL pimpl1(ptr);
        SECTION("Same module") {
            ModulePIMPL pimpl2(ptr);
            REQUIRE(pimpl1 == pimpl2);
            REQUIRE(!(pimpl1 != pimpl2));
            SECTION("locked-ness") {
                pimpl1.lock();
                REQUIRE(pimpl1 != pimpl2);
                REQUIRE(!(pimpl1 == pimpl2));
            }
        }
        SECTION("Different module") {
            auto ptr2 = std::make_shared<Prism>();
            ModulePIMPL pimpl2(ptr2);
            REQUIRE(pimpl2 != pimpl1);
            REQUIRE(!(pimpl2 == pimpl1));
        }
    }
}

TEST_CASE("ModulePIMPL class : hash") {
    type::hasher h(bphash::HashType::Hash128);
    SECTION("Actual state") {
        auto ptr = std::make_shared<Rectangle>();
        ModulePIMPL pimpl(ptr);
        pimpl.inputs().at("Dimension 1").change(double{1.23});
        pimpl.inputs().at("Dimension 2").change(double{4.56});
        pimpl.hash(h);
        auto hv = bphash::hash_to_string(h.finalize());
        // std::cout << hv <<std::endl;
        REQUIRE(hv == "88a81704d8a35c1e58e5d2eba110a94f");
    }
}

TEST_CASE("ModulePIMPL class : memoization") {
    type::hasher h(bphash::HashType::Hash128);
    ModulePIMPL pimpl(std::make_shared<Prism>());
    pimpl.submods().at("area").change(area_module());
    auto inputs = pimpl.inputs();
    inputs.at("Dimensions").change(std::vector<double>{1.23, 4.56, 7.89});
    SECTION("Default memoization") {
        pimpl.memoize(h, inputs);
        auto hv = bphash::hash_to_string(h.finalize());
        REQUIRE(hv == "f735bc5fea4e675d858f81fe9bc218cd");
    }
    SECTION("Different input") {
        inputs.at("Dimensions").change(std::vector<double>{1.11, 2.22, 3.33});
        pimpl.memoize(h, inputs);
        auto hv = bphash::hash_to_string(h.finalize());
        REQUIRE(hv == "14a280315ed0883345903b5ef34ee4e0");
    }
    SECTION("Different submods") {
        pimpl.submods()
          .at("area")
          .value()
          .change_input("Dimension 1")
          .change(1.23);
        pimpl.memoize(h, inputs);
        auto hv = bphash::hash_to_string(h.finalize());
        REQUIRE(hv == "d0571da6f22c6a9491816e8dd7b9f95f");
    }
}

TEST_CASE("ModulePIMPL class : not_set") {
    SECTION("No submodules") {
        auto ptr = std::make_shared<Rectangle>();
        ModulePIMPL p(ptr);
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
            p.submods().at("area").change(area_module());
            auto ps = p.not_set(p.inputs());
            REQUIRE(ps.empty());
            REQUIRE(p.ready());
        }
    }
}

TEST_CASE("ModulePIMPL class : run") {
    SECTION("No submodules") {
        auto ptr = std::make_shared<Rectangle>();
        auto c   = std::make_shared<cache_type>();
        ModulePIMPL p(ptr, c);
        p.inputs().at("Dimension 1").change(1.23);
        p.inputs().at("Dimension 2").change(4.56);

        SECTION("Can run it") {
            REQUIRE(!p.locked());
            REQUIRE(!p.is_cached(input_map{}));
            auto rv = p.run(input_map{});
            REQUIRE(rv.at("Area").value<double>() == 1.23 * 4.56);
            REQUIRE(p.locked());
            REQUIRE(p.is_cached(input_map{}));
        }
    }

    SECTION("Submodules") {
        auto ptr = std::make_shared<Prism>();
        ModulePIMPL p(ptr);
        std::vector<double> dims{1.23, 4.56, 7.89};
        p.inputs().at("Dimensions").change(dims);
        p.submods().at("area").change(area_module());
        auto rv = p.run(input_map{});
        REQUIRE(rv.at("volume").value<double>() == 1.23 * 4.56 * 7.89);
    }
}
