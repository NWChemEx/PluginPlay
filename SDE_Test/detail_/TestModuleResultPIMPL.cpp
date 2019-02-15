#include <SDE/detail_/ModuleResultPIMPL.hpp>
#include <catch2/catch.hpp>
using namespace SDE;
using namespace SDE::detail_;
using shared_any = typename ModuleResultPIMPL::shared_any;
/* Our testing strategy is to establish that default state is correct. Then we
 * establish that changing any member (aside from checks) causes equality to
 * fail. With this knowledge we check each function comparing the resulting
 * instance to an instance we have set to the correct state.
 *
 */

TEST_CASE("ModuleResultPIMPL : Default ctor") {
    ModuleResultPIMPL pimpl;
    SECTION("Default state") {
        REQUIRE(!pimpl.is_type_set());
        REQUIRE(pimpl.m_value == shared_any{});
        REQUIRE(pimpl.m_desc == "");
        REQUIRE(pimpl.m_type == std::type_index(typeid(std::nullptr_t)));
    }

    SECTION("Error checking") {
        SECTION("Can't change the value") {
            REQUIRE_THROWS_AS(pimpl.change(shared_any{}), std::runtime_error);
        }
        SECTION("Can't check validity") {
            REQUIRE_THROWS_AS(pimpl.is_valid(type::any{}), std::runtime_error);
        }
    }
}

TEST_CASE("ModuleResultPIMPL : Equality comparisons") {
    ModuleResultPIMPL pimpl1, pimpl2;

    SECTION("Defaults are equivalent") { REQUIRE(pimpl1 == pimpl2); }
    SECTION("Different types") {
        pimpl2.m_type = std::type_index(typeid(double));
        REQUIRE(pimpl1 != pimpl2);
    }
    SECTION("Different descriptions") {
        pimpl2.m_desc = "hello world";
        REQUIRE(pimpl1 != pimpl2);
    }
    SECTION("Different values") {
        pimpl2.m_value = std::make_shared<SDEAny>(make_SDEAny<int>(3));
        REQUIRE(pimpl1 != pimpl2);
    }
}

TEST_CASE("ModuleResultPIMPL : set_type") {
    ModuleResultPIMPL pimpl1, pimpl2;
    pimpl1.set_type(typeid(double));
    pimpl2.m_type = std::type_index(typeid(double));
    REQUIRE(pimpl1 == pimpl2);
    REQUIRE(pimpl1.is_type_set());

    SECTION("Can't change the type after it's set") {
        const auto& type = typeid(int);
        REQUIRE_THROWS_AS(pimpl1.set_type(type), std::runtime_error);
    }
}

TEST_CASE("ModuleResultPIMPL : change") {
    ModuleResultPIMPL pimpl1, pimpl2;
    pimpl1.set_type(typeid(double));
    pimpl2.set_type(typeid(double));
    auto ptr       = std::make_shared<type::any>(make_SDEAny<double>(3.14));
    pimpl2.m_value = ptr;

    SECTION("Change the value") {
        pimpl1.change(ptr);
        REQUIRE(pimpl1 == pimpl2);
    }
    SECTION("Invalid value") {
        auto ptr2 = std::make_shared<type::any>(make_SDEAny<int>(3));
        REQUIRE_THROWS_AS(pimpl1.change(ptr2), std::invalid_argument);
    }
}

TEST_CASE("ModuleResultPIMPL : clone") {
    ModuleResultPIMPL pimpl1;
    pimpl1.set_type(typeid(double));
    auto da_any = std::make_shared<type::any>(make_SDEAny<double>(3.14));
    pimpl1.change(da_any);

    auto da_clone = pimpl1.clone();
    REQUIRE(*da_clone == pimpl1);
    REQUIRE(da_clone.get() != &pimpl1);
}

TEST_CASE("ModuleResultPIMPL : copy ctor") {
    ModuleResultPIMPL pimpl1;
    pimpl1.set_type(typeid(double));
    auto da_any = std::make_shared<type::any>(make_SDEAny<double>(3.14));
    pimpl1.change(da_any);
    ModuleResultPIMPL pimpl2(pimpl1);
    REQUIRE(pimpl1 == pimpl2);
}

TEST_CASE("ModuleResultPIMPL : copy assignment") {
    ModuleResultPIMPL pimpl1;
    pimpl1.set_type(typeid(double));
    auto da_any = std::make_shared<type::any>(make_SDEAny<double>(3.14));
    pimpl1.change(da_any);
    ModuleResultPIMPL pimpl2;
    auto* ptr = &(pimpl2 = pimpl1);
    REQUIRE(pimpl1 == pimpl2);
    REQUIRE(ptr == &pimpl2);
}

TEST_CASE("ModuleResultPIMPL : move ctor") {
    ModuleResultPIMPL pimpl1;
    pimpl1.set_type(typeid(double));
    auto da_any = std::make_shared<type::any>(make_SDEAny<double>(3.14));
    pimpl1.change(da_any);
    ModuleResultPIMPL pimpl2(pimpl1);
    ModuleResultPIMPL pimpl3(std::move(pimpl1));
    REQUIRE(pimpl3 == pimpl2);
}

TEST_CASE("ModuleResultPIMPL : move assignment") {
    ModuleResultPIMPL pimpl1;
    pimpl1.set_type(typeid(double));
    auto da_any = std::make_shared<type::any>(make_SDEAny<double>(3.14));
    pimpl1.change(da_any);
    ModuleResultPIMPL pimpl2;
    ModuleResultPIMPL pimpl3(pimpl1);
    auto* ptr = &(pimpl2 = std::move(pimpl1));
    REQUIRE(pimpl3 == pimpl2);
    REQUIRE(ptr == &pimpl2);
}
