#include <SDE/ModuleInput.hpp>
//#include <SDE/detail_/PropertyTypeInputBuilder.hpp>
#include <catch2/catch.hpp>
using namespace SDE;
using namespace SDE::detail_;

/* This file contains tests for the ModuleInput and ModuleInputBuilder classes.
 * The ModuleInputBuilder class is dependent on the ModuleInput class hence we
 * first establish that the ModuleInput class is working correctly before
 * testing the ModuleInputBuilder class. Testing the latter is accomplished by
 * reusing the check_state functions from the ModuleInput class tests to check
 * the state of the ModuleInput instance we are building.
 */

static void check_state(ModuleInput& input, std::string desc = "",
                        bool opt = false, bool trans = false) {
    SECTION("Description") { REQUIRE(input.description() == desc); }
    SECTION("Is Optional?") { REQUIRE(input.is_optional() == opt); }
    SECTION("Is Transparent?") { REQUIRE(input.is_transparent() == trans); }
}

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
            input.change(value);
            check_state<int>(input, value);
            int& pvalue = input.value<int&>();
            SECTION("Makes a Copy") { REQUIRE(&pvalue != &value); }
            SECTION("Can be modified") {
                pvalue = four;
                check_state<int>(input, four);
            }
        }
        SECTION("Stored By Reference") {
            input.set_type<const int&>();
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
}

// TEST_CASE("PropertyTypeInputBuilder Class") {
//    SECTION("Default Ctor") {
//        PropertyTypeInputBuilder<> builder;
//        auto inputs = builder.finalize();
//        REQUIRE(inputs.size() == 0);
//    }
//
//    PropertyTypeInputBuilder<> builder;
//
//    SECTION("Add an input") {
//        auto new_builder = builder.add_input<int>("key");
//        using new_type   = decltype(new_builder);
//        using corr_type  = PropertyTypeInputBuilder<int>;
//        static_assert(std::is_same_v<new_type, corr_type>,
//                      "Resulting type is wrong.");
//        auto inputs = new_builder.finalize();
//        REQUIRE(inputs[0].first == "key");
//        check_state(inputs[0].second);
//    }
//
//    auto new_builder = builder.add_input<int>("key");
//
//    SECTION("Public Member Variables") {
//        SECTION("Description") {
//            new_builder.description("Hi");
//            auto inputs = new_builder.finalize();
//            check_state(inputs[0].second, "Hi");
//        }
//        SECTION("Optional") {
//            new_builder.optional();
//            auto inputs = new_builder.finalize();
//            check_state(inputs[0].second, "", true);
//        }
//        SECTION("Required") {
//            SECTION("By Itself") {
//                new_builder.required();
//                auto inputs = new_builder.finalize();
//                check_state(inputs[0].second, "", false);
//            }
//            SECTION("Undoes Optional") {
//                new_builder.optional().required();
//                auto inputs = new_builder.finalize();
//                check_state(inputs[0].second, "", false);
//            }
//        }
//        SECTION("Transparent") {
//            new_builder.transparent();
//            auto inputs = new_builder.finalize();
//            check_state(inputs[0].second, "", false, true);
//        }
//        SECTION("Opaque") {
//            SECTION("By Itself") {
//                new_builder.opaque();
//                auto inputs = new_builder.finalize();
//                check_state(inputs[0].second, "", false, false);
//            }
//            SECTION("Undoes Transparent") {
//                new_builder.transparent().opaque();
//                auto inputs = new_builder.finalize();
//                check_state(inputs[0].second, "", false, false);
//            }
//        }
//    }
//
//    SECTION("Default Value") {
//        new_builder.default_value(int{3});
//        auto inputs = new_builder.finalize();
//        check_state<int>(inputs[0].second, 3);
//    }
//
//    SECTION("Check") {
//        using validity_check = typename ModuleInput::validity_check<int>;
//        validity_check check([](const int& x) { return x == 3; });
//        new_builder.check(check);
//        auto inputs = new_builder.finalize();
//        SECTION("Valid value") { REQUIRE(inputs[0].second.is_valid(int{3})); }
//        SECTION("Invalid value") {
//            REQUIRE(!inputs[0].second.is_valid(int{4}));
//        }
//    }
//
//    SECTION("Can declare an API") {
//        auto newest_builder = new_builder.add_input<double>("key2")
//                                .description("Hi")
//                                .add_input<std::string>("key3")
//                                .optional();
//        using final_type = decltype(newest_builder);
//        using corr_type  = PropertyTypeInputBuilder<int, double, std::string>;
//        static_assert(std::is_same_v<final_type, corr_type>, "Type is wrong");
//        auto inputs = newest_builder.finalize();
//        check_state(inputs[0].second);
//        REQUIRE(inputs[1].first == "key2");
//        check_state(inputs[1].second, "Hi");
//        REQUIRE(inputs[2].first == "key3");
//        check_state(inputs[2].second, "", true);
//    }
//}
