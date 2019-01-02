#include <SDE/ModuleOutput.hpp>
#include <SDE/detail_/ModuleOutputBuilder.hpp>
#include <SDE/detail_/PropertyTypeOutputBuilder.hpp>
#include <catch2/catch.hpp>
using namespace SDE;
using namespace SDE::detail_;

/* This file contains tests for the ModuleOutput and ModuleOutputBuilder
 * classes. The ModuleOutputBuilder class is dependent on the ModuleOutput class
 * hence we first establish that the ModuleOutput class is working correctly
 * before testing the ModuleOutputBuilder class. Testing the latter is
 * accomplished by reusing the check_state functions from the ModuleOutput class
 * tests to check the state of the ModuleOutput instance we are building.
 */

static void check_state(ModuleOutput& input, std::string desc = "") {
    SECTION("Description") { REQUIRE(input.desc == desc); }
}

template<typename U, typename T>
static void check_state(ModuleOutput& input, T value, std::string desc = "") {
    check_state(input, desc);
    SECTION("Value") { REQUIRE(input.value<U>() == value); }
    SECTION("Shared Pointer to Value") {
        REQUIRE(input.value<typename ModuleOutput::shared_any>().use_count());
    }
}

TEST_CASE("ModuleOutput Class") {
    SECTION("Default Ctor") {
        ModuleOutput output;
        check_state(output);
    }

    ModuleOutput output;

    SECTION("Public Member Variables") {
        SECTION("Description") {
            output.desc = "Hi";
            check_state(output, "Hi");
        }
    }

    SECTION("Set Type") {
        SECTION("Must Be Unqualified Type") {
            // Will fail to compile, uncomment to check
            // output.set_type<int&>();
            // output.set_type<const int>();
        }
        SECTION("Valid Type") {
            output.set_type<int>();
            check_state(output);
        }
    }

    int three = 3;
    SECTION("Set Value") {
        SECTION("Fails if set_type hasn't been called") {
            REQUIRE_THROWS_AS(output.change(three), std::runtime_error);
        }
        output.set_type<int>();
        SECTION("Can set to a value of correct type") {
            output.change(three);
            check_state<int>(output, three);
        }
        SECTION("Throws if bad type") {
            double pi = 3.14;
            REQUIRE_THROWS_AS(output.change(pi), std::invalid_argument);
        }
        SECTION("Can set to a shared pointer") {
            auto an_any = detail_::make_SDEAny<int>(three);
            auto shared = std::make_shared<detail_::SDEAny>(an_any);
            output.change(shared);
            check_state<int>(output, three);
            REQUIRE(shared.use_count() == 2);
        }
    }
    output.set_type<int>();
    output.change(three);

    SECTION("Copy Ctor") {
        ModuleOutput copy(output);
        check_state<int>(copy, three);
    }

    SECTION("Copy Assignment") {
        ModuleOutput copy;
        auto& pcopy = (copy = output);
        check_state<int>(copy, three);
        SECTION("Can Chain") { REQUIRE(&pcopy == &copy); }
    }

    SECTION("Move Ctor") {
        ModuleOutput move(output);
        check_state<int>(move, three);
    }

    SECTION("Move Assignment") {
        ModuleOutput move;
        auto& pmove = (move = output);
        check_state<int>(move, three);
        SECTION("Can Chain") { REQUIRE(&pmove == &move); }
    }
}

TEST_CASE("ModuleOutputBuilder Class") {
    SECTION("Default Ctor") {
        ModuleOutput output;
        ModuleOutputBuilder builder(output);
        check_state(output);
    }

    ModuleOutput output;
    ModuleOutputBuilder builder(output);

    SECTION("Description") {
        builder.description("Hi");
        check_state(output, "Hi");
    }
    int three = 3;
    SECTION("Type") {
        builder.type<int>();
        REQUIRE_NOTHROW(output.change(three));
    }

    SECTION("Can Chain") {
        builder.type<int>().description("Hi");
        output.change(three);
        check_state<int>(output, three, "Hi");
    }
}

TEST_CASE("PropertyTypeOutputBuilder Class") {
    SECTION("Default Ctor") {
        PropertyTypeOutputBuilder builder;
        auto map = builder.finalize();
        REQUIRE(map.size() == 0);
    }

    PropertyTypeOutputBuilder builder;

    SECTION("Add an Output") {
        auto map = builder.add_output<int>("Key").finalize();
        REQUIRE(map.count("Key") == 1);
        check_state(map.at("Key"));
    }

    auto new_builder = builder.add_output<int>("Key");

    SECTION("Description") {
        auto map = new_builder.description("Hi").finalize();
        check_state(map.at("Key"), "Hi");
    }

    SECTION("Can Chain") {
        auto map = new_builder.add_output<double>("Key2")
                     .description("Hi")
                     .add_output<int>("Key3")
                     .description("Bye")
                     .finalize();
        check_state(map.at("Key"));
        check_state(map.at("Key2"), "Hi");
        check_state(map.at("Key3"), "Bye");
    }
}
