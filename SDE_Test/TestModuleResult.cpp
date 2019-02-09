#include <SDE/ModuleResult.hpp>
#include <catch2/catch.hpp>
using namespace SDE;
using namespace SDE::detail_;

static void check_state(ModuleResult& output, std::string desc = "") {
    SECTION("Description") { REQUIRE(output.description() == desc); }
}

template<typename U, typename T>
static void check_state(ModuleResult& output, T value, std::string desc = "") {
    check_state(output, desc);
    SECTION("Value") { REQUIRE(output.value<U>() == value); }
    SECTION("Shared Pointer to Const Value") {
        auto shared = output.value<std::shared_ptr<const U>>();
        REQUIRE(*shared == value);
        REQUIRE(shared.get() == &output.value<const U&>());
    }
    SECTION("Shared Pointer to Any") {
        auto shared      = output.value<typename ModuleResult::shared_any>();
        const U& pshared = detail_::SDEAnyCast<const U&>(*shared);
        REQUIRE(pshared == value);
        REQUIRE(&pshared == &output.value<const U&>());
    }
}

TEST_CASE("ModuleResult Class") {
    SECTION("Default Ctor") {
        ModuleResult output;
        check_state(output);
    }

    ModuleResult output;

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
        ModuleResult copy(output);
        check_state<int>(copy, three);
    }

    SECTION("Copy Assignment") {
        ModuleResult copy;
        auto& pcopy = (copy = output);
        check_state<int>(copy, three);
        SECTION("Can Chain") { REQUIRE(&pcopy == &copy); }
    }

    SECTION("Move Ctor") {
        ModuleResult move(output);
        check_state<int>(move, three);
    }

    SECTION("Move Assignment") {
        ModuleResult move;
        auto& pmove = (move = output);
        check_state<int>(move, three);
        SECTION("Can Chain") { REQUIRE(&pmove == &move); }
    }

    SECTION("Can chain") {
        ModuleResult chain;
        chain.set_type<int>().set_description("Hi").change(three);
        check_state<int>(chain, three, "Hi");
    }

    SECTION("Comparison operators") {
        ModuleResult copy(output);
        REQUIRE(copy == output);
        REQUIRE(!(copy != output));

        SECTION("Different values") {
            copy.change(4);
            REQUIRE(copy != output);
            REQUIRE(!(copy == output));
        }

        SECTION("Different descriptions") {
            copy.set_description("Different description");
            REQUIRE(copy != output);
            REQUIRE(!(copy == output));
        }
    }
}
