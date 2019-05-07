#include "examples/module_base.hpp"
#include <catch2/catch.hpp>
#include <sde/detail_/module_pimpl.hpp>
#include <sde/module.hpp>

using namespace sde;

inline Module make_module() {
    auto ptr = std::make_shared<Rectangle>();
    return Module(std::make_unique<detail_::ModulePIMPL>(ptr));
}

inline Module make_prism() {
    auto ptr = std::make_shared<Prism>();
    return Module(std::make_unique<detail_::ModulePIMPL>(ptr));
}

TEST_CASE("Module : default ctor") {
    Module m;
    REQUIRE(!m.ready());
    REQUIRE(!m.locked());
    REQUIRE(m.inputs() == type::input_map{});
    REQUIRE(m.submods() == type::submodule_map{});
}

TEST_CASE("Module : Equality") {
    SECTION("Default state") {
        Module m1, m2;
        REQUIRE(m1 == m2);
        REQUIRE(!(m1 != m2));
    }
    SECTION("Non-default state") {
        Module m1 = make_module(), m2 = make_module();
        REQUIRE(m1 == m2);
        REQUIRE(!(m1 != m2));

        SECTION("Locked-ness") {
            m1.lock();
            REQUIRE(m1 != m2);
            REQUIRE(!(m1 == m2));
        }

        SECTION("Input values") {
            m1.change_input("Dimension 1", double{1.23});
            REQUIRE(m1 != m2);
            REQUIRE(!(m1 == m2));
        }
    }
}

TEST_CASE("Module : run_as") {
    Module m(make_module());
    auto[area] = m.run_as<Area>(1.23, 4.56);
    REQUIRE(area == 1.23 * 4.56);
}

TEST_CASE("Module : run") {
    Module m(make_module());
    auto inputs = m.inputs();
    inputs.at("Dimension 1").change(double{1.23});
    inputs.at("Dimension 2").change(double{4.56});
    auto rv = m.run(inputs);
    REQUIRE(rv.at("area").value<double>() == 1.23 * 4.56);
}

TEST_CASE("Module : ready") {
    Module m(make_module());
    REQUIRE(!m.ready());
    REQUIRE(m.ready<Area>());
}

TEST_CASE("Module : lock") {
    Module m(make_module());
    REQUIRE(!m.locked());
    m.lock();
    REQUIRE(m.locked());
}

TEST_CASE("Module : locked") {
    Module m(make_module());
    m.lock();
    REQUIRE(m.locked());
}

TEST_CASE("Module : change_input") {
    Module m(make_module());
    m.change_input("Dimension 1", 1.23);
    REQUIRE(m.inputs().at("Dimension 1").value<double>() == 1.23);
}

TEST_CASE("Module : change_submod") {
    Module m(make_prism());
    m.change_submod("area", std::make_shared<Module>(make_module()));
    REQUIRE(m.submods().at("area").value() == make_module());
}

TEST_CASE("Module : hash") {
    Module m(make_module());
    type::hasher h(bphash::HashType::Hash128);
    m.hash(h);
    auto hv = bphash::hash_to_string(h.finalize());
    REQUIRE(hv == "1b740793eacb5cdf38dc426f626ab5ab");
}

TEST_CASE("Module : copy ctor") {
    Module m1(make_module());
    Module m2(m1);
    REQUIRE(m1 == m2);
}

TEST_CASE("Module : copy assignment") {
    Module m1(make_module());
    Module m2;
    auto* ptr = &(m2 = m1);
    REQUIRE(m1 == m2);
    REQUIRE(ptr == &m2);
}

TEST_CASE("Module : move ctor") {
    Module m1(make_module());
    Module m2(m1);
    Module m3(std::move(m1));
    REQUIRE(m3 == m2);
}

TEST_CASE("Module : move assignment") {
    Module m1(make_module());
    Module m2(m1), m3;
    auto* ptr = &(m3 = std::move(m1));
    REQUIRE(m3 == m2);
    REQUIRE(ptr == &m3);
}
