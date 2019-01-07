#include <SDE/ModuleOutput.hpp>
//#include <SDE/detail_/PropertyTypeOutputBuilder.hpp>
#include <catch2/catch.hpp>
using namespace SDE;
using namespace SDE::detail_;

static void check_state(ModuleOutput& output, std::string desc = "") {
    SECTION("Description") { REQUIRE(output.description() == desc); }
}

template<typename U, typename T>
static void check_state(ModuleOutput& output, T value, std::string desc = "") {
    check_state(output, desc);
    SECTION("Value") { REQUIRE(output.value<U>() == value); }
    SECTION("Shared Pointer to Const Value") {
        auto shared = output.value<std::shared_ptr<const U>>();
        REQUIRE(*shared == value);
        REQUIRE(shared.get() == &output.value<const U&>());
    }
    SECTION("Shared Pointer to Any") {
        auto shared      = output.value<typename ModuleOutput::shared_any>();
        const U& pshared = detail_::SDEAnyCast<const U&>(*shared);
        REQUIRE(pshared == value);
        REQUIRE(&pshared == &output.value<const U&>());
    }
}

TEST_CASE("ModuleOutput Class") {
    SECTION("Default Ctor") {
        ModuleOutput output;
        check_state(output);
    }

    ModuleOutput output;

    SECTION("Description") {
        output.set_description("Hi");
        check_state(output, "Hi");
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

    SECTION("Can chain") {
        ModuleOutput chain;
        chain.set_type<int>().set_description("Hi").change(three);
        check_state<int>(chain, three, "Hi");
    }
}

// TEST_CASE("PropertyTypeOutputBuilder Class") {
//    SECTION("Default Ctor") {
//        PropertyTypeOutputBuilder builder;
//        auto map = builder.finalize();
//        REQUIRE(map.size() == 0);
//    }
//
//    PropertyTypeOutputBuilder builder;
//
//    SECTION("Add an Output") {
//        auto map = builder.add_output<int>("Key").finalize();
//        REQUIRE(map[0].first == "Key");
//        check_state(map[0].second);
//    }
//
//    auto new_builder = builder.add_output<int>("Key");
//
//    SECTION("Description") {
//        auto map = new_builder.description("Hi").finalize();
//        check_state(map[0].second, "Hi");
//    }
//
//    SECTION("Can Chain") {
//        auto map = new_builder.add_output<double>("Key2")
//                     .description("Hi")
//                     .add_output<int>("Key3")
//                     .description("Bye")
//                     .finalize();
//        check_state(map[0].second);
//        REQUIRE(map[1].first == "Key2");
//        check_state(map[1].second, "Hi");
//        REQUIRE(map[2].first == "Key3");
//        check_state(map[2].second, "Bye");
//    }
//}
