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

TEST_CASE("ModuleInput Class") {
    SECTION("Default Ctor") {
        ModuleInput input;
        check_state(input);
    }

    ModuleInput input;

    int three = 3;
    double pi = 3.14;
    SECTION("Set Type") {
        SECTION("Read/Write Reference") {
            // Should fail to compile, uncomment to check
            // input.set_type<int&>();
        }
        SECTION("Value") {
            input.set_type<int>();
            SECTION("Valid Type") { REQUIRE(input.is_valid(three)); }
            SECTION("Invalid Type") { REQUIRE(!input.is_valid(pi)); }
        }
        SECTION("Read-Only Reference") {
            input.set_type<const int&>();
            SECTION("Valid Type") { REQUIRE(input.is_valid(three)); }
            SECTION("Invalid Type") { REQUIRE(!input.is_valid(pi)); }
        }
    }

    const int four = 4;
    SECTION("Add Check") {
        using validity_check = typename ModuleInput::validity_check<int>;
        input.add_check(validity_check([](const int& x) { return x == 3; }));
        SECTION("Valid Value") { REQUIRE(input.is_valid(three)); }
        SECTION("Invalid Value") { REQUIRE(!input.is_valid(four)); }
    }

    SECTION("Change Value") {
        int value = 3;
        SECTION("Can't set value if type isn't set") {
            REQUIRE_THROWS_AS(input.change(value), std::runtime_error);
        }
        SECTION("Stored By Value") {
            input.set_type<int>();
            SECTION("Passed by reference") {
                input.change(value); // std::forward will take this as a ref
                check_state<int>(input, value);
                int& pvalue = input.value<int&>();
                SECTION("Makes a Copy") { REQUIRE(&pvalue != &value); }
                SECTION("Can be modified") {
                    pvalue = four;
                    check_state<int>(input, four);
                }
            }
            SECTION("Passed by value") {
                input.change(int{3});
                check_state<int>(input, value);
                SECTION("Can be modified") {
                    input.value<int&>() = four;
                    check_state<int>(input, four);
                }
            }
        }
        SECTION("Stored By Reference") {
            input.set_type<const int&>();
            SECTION("Passed by reference") {
                input.change(value);
                check_state<const int&>(input, value);
                SECTION("Can't Get Read/Write Version") {
                    REQUIRE_THROWS_AS(input.value<int&>(), std::bad_cast);
                }
                SECTION("Is by reference") {
                    const int& pvalue = input.value<const int&>();
                    REQUIRE(&pvalue == &value);
                }
            }
            SECTION("Passed by value") {
                input.change(int{3});
                check_state<const int&>(input, value);
                SECTION("Can't Get Read/Write Version") {
                    REQUIRE_THROWS_AS(input.value<int&>(), std::bad_cast);
                }
            }
        }
    }
    SECTION("Description") {
        auto desc = "The description";
        input.set_description(desc);
        check_state(input, desc);
    }
    SECTION("Optional") {
        input.make_optional();
        check_state(input, "", true);

        input.make_required();
        check_state(input, "", false);

        // make sure it wasn't doing nothing the first time
        input.make_optional();
        check_state(input, "", true);
    }
    SECTION("Is Transparent") {
        input.make_transparent();
        check_state(input, "", false, true);

        input.make_opaque();
        check_state(input, "", false, false);

        input.make_transparent();
        check_state(input, "", false, true);
    }

    input.set_type<int>();
    input.change(three);

    SECTION("Hash") {
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

    SECTION("Copy Ctor") {
        ModuleInput copy(input);
        check_state<int>(copy, three);
    }

    SECTION("Copy Assignment") {
        ModuleInput copy;
        auto& pcopy = (copy = input);
        check_state<int>(copy, three);
        SECTION("Can be chained") { REQUIRE(&pcopy == &copy); }
    }

    SECTION("Move Ctor") {
        ModuleInput move(std::move(input));
        check_state<int>(move, three);
    }

    SECTION("Move Assignment") {
        ModuleInput move;
        auto& pmove = (move = std::move(input));
        check_state<int>(move, three);
        SECTION("Can be chained") { REQUIRE(&pmove == &move); }
    }

    SECTION("Equality/Inequality") {
        ModuleInput copy(input);
        REQUIRE(input == copy);
        REQUIRE(!(input != copy));
        SECTION("Different values") {
            copy.change(4);
            REQUIRE(copy != input);
            REQUIRE(!(copy == input));
        }
        SECTION("Different descriptions") {
            copy.set_description("Something different");
            REQUIRE(copy != input);
            REQUIRE(!(copy == input));
        }
        SECTION("Optional-ness") {
            copy.make_optional();
            REQUIRE(copy != input);
            REQUIRE(!(copy == input));
        }
        SECTION("Transparency") {
            copy.make_transparent();
            REQUIRE(copy != input);
            REQUIRE(!(copy == input));
        }
    }
} // TEST_CASE
