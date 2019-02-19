#include <SDE/ModuleInput.hpp>
#include <catch2/catch.hpp>

using namespace SDE;
using namespace SDE::detail_;

// Checks everything, but the value
static void check_state(ModuleInput& input, std::string desc = "",
                        bool opt = false, bool trans = false) {
    SECTION("Description") { REQUIRE(input.description() == desc); }
    SECTION("Is Optional?") { REQUIRE(input.is_optional() == opt); }
    SECTION("Is Transparent?") { REQUIRE(input.is_transparent() == trans); }
}

// Additionally checks the value
template<typename U, typename T>
static void check_state(ModuleInput& input, T value, std::string desc = "",
                        bool opt = false, bool trans = false) {
    check_state(input, desc, opt, trans);
    SECTION("Value") { REQUIRE(input.value<U>() == value); }
}

TEST_CASE("ModuleInput Class : Default ctor") {
    ModuleInput input;
    SECTION("State") {
        REQUIRE(!input.is_optional());
        REQUIRE(!input.is_transparent());
        REQUIRE(!input.ready());
        REQUIRE(input.description() == "");
    }
    SECTION("Error checking") {
        REQUIRE_THROWS_AS(input.value<double>(), std::runtime_error);
    }
}

TEST_CASE("ModuleInput Class : set_type") {
    ModuleInput input;

    int three = 3;
    double pi = 3.14;
    SECTION("Can't set type to read/write reference") {
        // Should fail to compile, uncomment to check
        // input.set_type<int&>();
    }
    SECTION("Set type to by value") {
        auto* ptr = &(input.set_type<int>());
        SECTION("Correct return") { REQUIRE(ptr == &input); }
        SECTION("Valid value") { REQUIRE(input.is_valid(three)); }
        SECTION("Invalid value") { REQUIRE(!input.is_valid(pi)); }
    }
    SECTION("Set type to read-only reference") {
        input.set_type<const int&>();
        SECTION("Valid value") { REQUIRE(input.is_valid(three)); }
        SECTION("Invalid value") { REQUIRE(!input.is_valid(pi)); }
    }
}

TEST_CASE("ModuleInput Class : add_check") {
    ModuleInput input;
    input.set_type<int>();
    const int three = 3;
    const int four  = 4;
    SECTION("Add Check") {
        using validity_check = typename ModuleInput::validity_check<int>;
        validity_check fxn   = [](const int& x) { return x == 3; };
        auto* ptr            = &(input.add_check(fxn));
        SECTION("Correct return") { REQUIRE(&input == ptr); }
        SECTION("Valid Value") { REQUIRE(input.is_valid(three)); }
        SECTION("Invalid Value") { REQUIRE(!input.is_valid(four)); }
    }
}

TEST_CASE("ModuleInput Class : change") {
    ModuleInput input;
    SECTION("Store by value") {
        input.set_type<int>();
        SECTION("Pass by reference") {
            int value = 3;
            auto* ptr = &(input.change(value));
            SECTION("Return") { REQUIRE(ptr == &input); }
            int& pvalue = input.value<int&>();
            SECTION("Correct value") { REQUIRE(pvalue == 3); }
            SECTION("Makes a Copy") { REQUIRE(&pvalue != &value); }
            SECTION("Can be modified") {
                pvalue = 4;
                REQUIRE(input.value<int&>() == 4);
            }
        }
        SECTION("Passed by value") {
            input.change(int{3});
            SECTION("Correct value") { REQUIRE(input.value<int>() == 3); }
            SECTION("Can be modified") {
                input.value<int&>() = 4;
                REQUIRE(input.value<int>() == 4);
            }
        }
    }
    SECTION("Stored by const reference") {
        input.set_type<const int&>();
        SECTION("Passed by reference") {
            int value = 3;
            input.change(value);
            SECTION("Correct value") { REQUIRE(input.value<int>() == 3); }
            SECTION("Can't get read/write reference") {
                REQUIRE_THROWS_AS(input.value<int&>(), std::bad_cast);
            }
            SECTION("Is by reference") {
                const int* pvalue = &(input.value<const int&>());
                REQUIRE(pvalue == &value);
            }
        }
        SECTION("Passed by value") {
            input.change(int{3});
            SECTION("Correct value") { REQUIRE(input.value<int>() == 3); }
            SECTION("Can't Get Read/Write Version") {
                REQUIRE_THROWS_AS(input.value<int&>(), std::bad_cast);
            }
        }
    }
}

TEST_CASE("ModuleInput Class : description") {
    ModuleInput input;
    auto desc = "The description";
    auto* ptr = &(input.set_description(desc));
    SECTION("Return") { REQUIRE(ptr == &input); }
    SECTION("Value") { REQUIRE(input.description() == desc); }
}

TEST_CASE("ModuleInput Class: optional") {
    ModuleInput input;
    auto* ptr = &(input.make_optional());
    SECTION("Return") { REQUIRE(ptr == &input); }
    SECTION("Value") { REQUIRE(input.is_optional()); }
    SECTION("Undone by make_required") {
        auto* ptr2 = &(input.make_required());
        REQUIRE(ptr2 == &input);
        REQUIRE(!input.is_optional());
    }
}

TEST_CASE("ModuleInput Class: transparent") {
    ModuleInput input;
    auto* ptr = &(input.make_transparent());
    SECTION("Return") { REQUIRE(ptr == &input); }
    SECTION("Value") { REQUIRE(input.is_transparent()); }
    SECTION("Undone by make_opaque") {
        auto* ptr2 = &(input.make_opaque());
        REQUIRE(ptr2 == &input);
        REQUIRE(!input.is_transparent());
    }
}

TEST_CASE("ModuleInput Class : hash") {
    ModuleInput input;
    input.set_type<int>();
    input.change(int{3});
    Hasher h(bphash::HashType::Hash128);
    SECTION("Opaque") {
        input.hash(h);
        auto hv = bphash::hash_to_string(h.finalize());
        // std::cout << hv << std::endl;
        REQUIRE(hv == "9a4294b64e60cc012c5ed48db4cd9c48");
    }
    SECTION("Transparent") {
        input.make_transparent();
        input.hash(h);
        auto hv = bphash::hash_to_string(h.finalize());
        // std::cout << hv <<std::endl;
        REQUIRE(hv == "00000000000000000000000000000000");
    }
}

TEST_CASE("Equality comparisons") {
    ModuleInput input1, input2;
    SECTION("Default are equal") {
        REQUIRE(input1 == input2);
        REQUIRE(!(input1 != input2));
    }
    SECTION("Different types") {
        input1.set_type<int>();
        REQUIRE(input1 != input2);
        REQUIRE(!(input1 == input2));
    }
    SECTION("Different descriptions") {
        input1.set_description("Something different");
        REQUIRE(input1 != input2);
        REQUIRE(!(input1 == input2));
    }
    SECTION("Different optional-ness") {
        input1.make_optional();
        REQUIRE(input1 != input2);
        REQUIRE(!(input1 == input2));
    }
    SECTION("Different opaque-ness") {
        input1.make_transparent();
        REQUIRE(input1 != input2);
        REQUIRE(!(input1 == input2));
    }
    SECTION("Different values") {
        input1.set_type<int>();
        input2.set_type<int>();
        input1.change(int{4});
        REQUIRE(input1 != input2);
        REQUIRE(!(input1 == input2));
    }
}

TEST_CASE("ModuleInput Class : Copy Ctor") {
    ModuleInput input;
    input.set_type<int>();
    input.make_optional();
    ModuleInput copy(input);
    REQUIRE(copy == input);
}

TEST_CASE("ModuleInput Class : Copy Assignment") {
    ModuleInput input;
    input.set_type<int>();
    input.make_optional();
    ModuleInput copy;
    auto* ptr = &(copy = input);
    REQUIRE(copy == input);
    REQUIRE(ptr == &copy);
}

TEST_CASE("ModuleInput Class : Move Ctor") {
    ModuleInput input;
    input.set_type<int>();
    input.make_optional();
    ModuleInput copy(input);
    ModuleInput moved(std::move(input));
    REQUIRE(copy == moved);
}

TEST_CASE("ModuleInput Class : Move Assignment") {
    ModuleInput input;
    input.set_type<int>();
    input.make_optional();
    ModuleInput copy(input);
    ModuleInput moved;
    auto* ptr = &(moved = std::move(input));
    REQUIRE(moved == copy);
    REQUIRE(ptr == &moved);
}
