#include <catch2/catch.hpp>
#include <sde/detail_/submodule_request_pimpl.hpp>

/* Without a real module we can't test:
 *
 * - ready (in an actually ready state)
 * - lock (requires ready passing)
 * - value (requires ready passing)
 * - inequality between instances with different modules.
 *
 * Everything else is testable with just a default constructed Module instance.
 */
using namespace sde::detail_;
using module_ptr = typename SubmoduleRequestPIMPL::module_ptr;

TEST_CASE("SubmoduleRequestPIMPL : Default ctor") {
    SubmoduleRequestPIMPL pimpl;

    SECTION("State") {
        REQUIRE(!pimpl.have_module());
        REQUIRE(!pimpl.type_set());
        REQUIRE(!pimpl.ready());
        REQUIRE(pimpl.m_desc == "");
        REQUIRE(pimpl.m_type == std::type_index(typeid(std::nullptr_t)));
        REQUIRE(pimpl.m_module == module_ptr{});
    }

    SECTION("Error Checking") {
        SECTION("check_type") {
            REQUIRE_THROWS_AS(pimpl.check_type(typeid(int)),
                              std::runtime_error);
        }
        SECTION("value") {
            REQUIRE_THROWS_AS(pimpl.value(), std::runtime_error);
        }
        SECTION("lock") { REQUIRE_THROWS_AS(pimpl.lock(), std::runtime_error); }
    }
}

TEST_CASE("SubmoduleRequestPIMPL : Comparisons") {
    SubmoduleRequestPIMPL pimpl1, pimpl2;
    SECTION("Default state") {
        REQUIRE(pimpl1 == pimpl2);
        REQUIRE(!(pimpl1 != pimpl2));
    }
    SECTION("Different types") {
        pimpl1.set_type(typeid(double), sde::type::input_map{});
        REQUIRE(pimpl1 != pimpl2);
        REQUIRE(!(pimpl1 == pimpl2));
    }
    SECTION("Different descriptions") {
        pimpl1.m_desc = "hello world";
        REQUIRE(pimpl1 != pimpl2);
        REQUIRE(!(pimpl1 == pimpl2));
    }
    SECTION("Different module set-ness") {
        auto ptr        = std::make_shared<sde::Module>();
        pimpl1.m_module = ptr;
        REQUIRE(pimpl1 != pimpl2);
        REQUIRE(!(pimpl1 == pimpl2));
    }
    SECTION("Same module ptr") {
        auto ptr        = std::make_shared<sde::Module>();
        pimpl1.m_module = ptr;
        pimpl2.m_module = ptr;
        REQUIRE(pimpl1 == pimpl2);
        REQUIRE(!(pimpl1 != pimpl2));
    }
    SECTION("Same module different ptrs") {
        auto ptr1       = std::make_shared<sde::Module>();
        auto ptr2       = std::make_shared<sde::Module>();
        pimpl1.m_module = ptr1;
        pimpl2.m_module = ptr2;
        REQUIRE(pimpl1 == pimpl2);
        REQUIRE(!(pimpl1 != pimpl2));
    }
}

TEST_CASE("SubmoduleRequestPIMPL : have_module") {
    SubmoduleRequestPIMPL pimpl;
    REQUIRE(!pimpl.have_module());
    auto ptr       = std::make_shared<sde::Module>();
    pimpl.m_module = ptr;
    REQUIRE(pimpl.have_module());
}

TEST_CASE("SubmoduleRequestPIMPL : type_set") {
    SubmoduleRequestPIMPL pimpl1;
    REQUIRE(!pimpl1.type_set());
    pimpl1.m_type = std::type_index(typeid(double));
    REQUIRE(pimpl1.type_set());
}

TEST_CASE("SubmoduleRequestPIMPL : ready") {
    SubmoduleRequestPIMPL pimpl1;
    SECTION("No type means not ready") { REQUIRE(!pimpl1.ready()); }
    SECTION("No submodule") {
        pimpl1.set_type(typeid(double), sde::type::input_map{});
        REQUIRE(!pimpl1.ready());
    }
}

TEST_CASE("SubmoduleRequestPIMPL : check_type") {
    SubmoduleRequestPIMPL pimpl;
    pimpl.m_type = std::type_index(typeid(double));
    REQUIRE(pimpl.check_type(typeid(double)));
    REQUIRE(!pimpl.check_type(typeid(int)));
}

TEST_CASE("SubmoduleRequestPIMPL : set_type") {
    SubmoduleRequestPIMPL pimpl1, pimpl2;
    pimpl1.set_type(typeid(double), sde::type::input_map{});
    SECTION("Can't set type twice") {
        REQUIRE_THROWS_AS(pimpl1.set_type(typeid(int), sde::type::input_map{}),
                          std::runtime_error);
    }
    pimpl2.m_type = std::type_index(typeid(double));
    REQUIRE(pimpl1 == pimpl2);
}

TEST_CASE("SubmoduleRequestPIMPL : Clone") {
    SubmoduleRequestPIMPL pimpl1;
    pimpl1.set_type(typeid(double), sde::type::input_map{});
    auto ptr = pimpl1.clone();
    REQUIRE(*ptr == pimpl1);
}

TEST_CASE("SubmoduleRequestPIMPL : Copy ctor") {
    SubmoduleRequestPIMPL pimpl1;
    pimpl1.set_type(typeid(double), sde::type::input_map{});
    SubmoduleRequestPIMPL pimpl2(pimpl1);
    REQUIRE(pimpl2 == pimpl1);
}

TEST_CASE("SubmoduleRequestPIMPL : Copy assignment") {
    SubmoduleRequestPIMPL pimpl1;
    pimpl1.set_type(typeid(double), sde::type::input_map{});
    SubmoduleRequestPIMPL pimpl2;
    auto* ptr = &(pimpl2 = pimpl1);
    REQUIRE(pimpl2 == pimpl1);
    REQUIRE(ptr == &pimpl2);
}

TEST_CASE("SubmoduleRequestPIMPL : Move ctor") {
    SubmoduleRequestPIMPL pimpl1;
    pimpl1.set_type(typeid(double), sde::type::input_map{});
    SubmoduleRequestPIMPL pimpl3(pimpl1);
    SubmoduleRequestPIMPL pimpl2(std::move(pimpl1));
    REQUIRE(pimpl2 == pimpl3);
}

TEST_CASE("SubmoduleRequestPIMPL : Move assignment") {
    SubmoduleRequestPIMPL pimpl1;
    pimpl1.set_type(typeid(double), sde::type::input_map{});
    SubmoduleRequestPIMPL pimpl3(pimpl1);
    SubmoduleRequestPIMPL pimpl2;
    auto* ptr = &(pimpl2 = std::move(pimpl1));
    REQUIRE(pimpl2 == pimpl3);
    REQUIRE(ptr == &pimpl2);
}
