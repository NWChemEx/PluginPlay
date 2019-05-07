#include <catch2/catch.hpp>
#include <sde/detail_/module_input_pimpl.hpp>
using namespace sde;
using namespace sde::detail_;
using check_fxn = typename ModuleInputPIMPL::any_check;

/* Our testing strategy is to establish that default state is correct. Then we
 * establish that changing any member (aside from checks) causes equality to
 * fail. With this knowledge we check each function comparing the resulting
 * instance to an instance we have set to the correct state.
 *
 */

TEST_CASE("ModuleInputPIMPL : Default ctor") {
    ModuleInputPIMPL pimpl;
    SECTION("Default state") {
        REQUIRE(!pimpl.is_ready());
        REQUIRE(!pimpl.is_type_set());
        REQUIRE(pimpl.m_value == type::any{});
        REQUIRE(pimpl.m_desc == "");
        REQUIRE(!pimpl.m_optional);
        REQUIRE(!pimpl.m_transparent);
        REQUIRE(pimpl.m_checks.size() == 0);
        REQUIRE(pimpl.m_type == std::type_index(typeid(std::nullptr_t)));
    }

    SECTION("Error checking") {
        SECTION("Can't change the value") {
            REQUIRE_THROWS_AS(pimpl.change(type::any{}), std::runtime_error);
        }
        SECTION("Can't check validity") {
            REQUIRE_THROWS_AS(pimpl.is_valid(type::any{}), std::runtime_error);
        }
    }
}

TEST_CASE("ModuleInputPIMPL : Equality comparisons") {
    ModuleInputPIMPL pimpl1, pimpl2;

    SECTION("Defaults are equivalent") { REQUIRE(pimpl1 == pimpl2); }
    SECTION("Different types") {
        pimpl2.m_type = std::type_index(typeid(double));
        REQUIRE(pimpl1 != pimpl2);
    }
    SECTION("Different descriptions") {
        pimpl2.m_desc = "hello world";
        REQUIRE(pimpl1 != pimpl2);
    }
    SECTION("Different optional-ness") {
        pimpl2.m_optional = true;
        REQUIRE(pimpl1 != pimpl2);
    }
    SECTION("Different transparencies") {
        pimpl2.m_transparent = true;
        REQUIRE(pimpl1 != pimpl2);
    }
    SECTION("Different checks") {
        check_fxn fxn = [](const type::any&) { return false; };
        pimpl2.m_checks.emplace("A check", std::move(fxn));
        REQUIRE(pimpl1 == pimpl2);
    }
}

TEST_CASE("ModuleInputPIMPL : set_type") {
    ModuleInputPIMPL pimpl1, pimpl2;
    pimpl1.set_type(typeid(double));
    pimpl2.m_type = std::type_index(typeid(double));
    REQUIRE(pimpl1 == pimpl2);
    REQUIRE(pimpl1.is_type_set());

    SECTION("Can't change the type after it's set") {
        const auto& type = typeid(int);
        REQUIRE_THROWS_AS(pimpl1.set_type(type), std::runtime_error);
    }
}

TEST_CASE("ModuleInputPIMPL : add_check") {
    ModuleInputPIMPL pimpl1;
    pimpl1.set_type(typeid(double));
    check_fxn fxn = [](const type::any& in) {
        return SDEAnyCast<double>(in) > 1.0;
    };
    pimpl1.add_check(fxn);
    REQUIRE(pimpl1.m_checks.size() == 1);
    REQUIRE(pimpl1.m_checks.count("Check #0") == 1);

    SECTION("Can add a check with a description") {
        pimpl1.add_check(fxn, "A description");
        REQUIRE(pimpl1.m_checks.size() == 2);
        REQUIRE(pimpl1.m_checks.count("A description") == 1);
    }

    SECTION("Honors the check") {
        SECTION("Good value") {
            auto da_any = make_SDEAny<double>(double{1.2});
            REQUIRE(pimpl1.is_valid(da_any));
        }
        SECTION("Bad value") {
            auto da_any = make_SDEAny<double>(double{-1.2});
            REQUIRE(!pimpl1.is_valid(da_any));
        }
    }
}

TEST_CASE("ModuleInputPIMPL : change") {
    ModuleInputPIMPL pimpl1, pimpl2;
    pimpl1.set_type(typeid(double));
    pimpl2.set_type(typeid(double));
    auto da_any    = make_SDEAny<double>(3.14);
    pimpl2.m_value = da_any;

    SECTION("Change the value") {
        pimpl1.change(da_any);
        REQUIRE(pimpl1 == pimpl2);
        REQUIRE(pimpl1.is_ready());
    }
}

TEST_CASE("ModuleInputPIMPL : clone") {
    ModuleInputPIMPL pimpl1;
    pimpl1.set_type(typeid(double));
    auto da_any = make_SDEAny<double>(3.14);
    pimpl1.change(da_any);

    auto da_clone = pimpl1.clone();
    REQUIRE(*da_clone == pimpl1);
    REQUIRE(da_clone.get() != &pimpl1);
}

TEST_CASE("ModuleInputPIMPL : copy ctor") {
    ModuleInputPIMPL pimpl1;
    pimpl1.set_type(typeid(double));
    auto da_any = make_SDEAny<double>(3.14);
    pimpl1.change(da_any);
    ModuleInputPIMPL pimpl2(pimpl1);
    REQUIRE(pimpl1 == pimpl2);
}

TEST_CASE("ModuleInputPIMPL : copy assignment") {
    ModuleInputPIMPL pimpl1;
    pimpl1.set_type(typeid(double));
    auto da_any = make_SDEAny<double>(3.14);
    pimpl1.change(da_any);
    ModuleInputPIMPL pimpl2;
    auto* ptr = &(pimpl2 = pimpl1);
    REQUIRE(pimpl1 == pimpl2);
    REQUIRE(ptr == &pimpl2);
}

TEST_CASE("ModuleInputPIMPL : move ctor") {
    ModuleInputPIMPL pimpl1;
    pimpl1.set_type(typeid(double));
    auto da_any = make_SDEAny<double>(3.14);
    pimpl1.change(da_any);
    ModuleInputPIMPL pimpl2(pimpl1);
    ModuleInputPIMPL pimpl3(std::move(pimpl1));
    REQUIRE(pimpl3 == pimpl2);
}

TEST_CASE("ModuleInputPIMPL : move assignment") {
    ModuleInputPIMPL pimpl1;
    pimpl1.set_type(typeid(double));
    auto da_any = make_SDEAny<double>(3.14);
    pimpl1.change(da_any);
    ModuleInputPIMPL pimpl2;
    ModuleInputPIMPL pimpl3(pimpl1);
    auto* ptr = &(pimpl2 = std::move(pimpl1));
    REQUIRE(pimpl3 == pimpl2);
    REQUIRE(ptr == &pimpl2);
}
