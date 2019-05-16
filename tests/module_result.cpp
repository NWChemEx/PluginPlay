#include <catch2/catch.hpp>
#include <sde/module_result.hpp>
using namespace sde;
using namespace sde::detail_;

TEST_CASE("ModuleResult Class : Default Ctor") {
    ModuleResult output;
    SECTION("Default state") { REQUIRE(output.description() == ""); }
    SECTION("Error checking") {
        SECTION("Can't call change") {
            REQUIRE_THROWS_AS(output.change(int{3}), std::runtime_error);
        }
    }
}

TEST_CASE("ModuleResult Class : Description") {
    ModuleResult output;
    auto ptr = &(output.set_description("Hello world"));
    SECTION("Return") { REQUIRE(ptr == &output); }
    SECTION("Value") { REQUIRE(output.description() == "Hello world"); }
}

TEST_CASE("ModuleResult Class : set_type") {
    ModuleResult output;
    SECTION("Must Be Unqualified Type") {
        // Will fail to compile, uncomment to check
        // output.set_type<int &>();
        // output.set_type<const int>();
    }
    SECTION("Valid Type") {
        auto ptr = &(output.set_type<int>());
        REQUIRE(ptr == &output);
    }
}

TEST_CASE("ModuleResult Class : change") {
    ModuleResult output;
    output.set_type<int>();
    int three = 3;
    SECTION("Valid value") {
        output.change(three);
        REQUIRE(output.value<int>() == 3);
    }
    SECTION("Throws if bad type") {
        REQUIRE_THROWS_AS(output.change(double{3.15}), std::invalid_argument);
    }
    SECTION("Can set to a shared pointer") {
        auto da_any = detail_::make_SDEAny<int>(three);
        auto shared = std::make_shared<type::any>(std::move(da_any));
        int* ptr    = &(SDEAnyCast<int&>(*shared));
        output.change(shared);
        auto value = output.value<std::shared_ptr<const int>>();
        REQUIRE(value.get() == ptr);
    }
}

TEST_CASE("ModuleResult Class: Equality") {
    ModuleResult output1, output2;
    SECTION("Defaults") {
        REQUIRE(output1 == output2);
        REQUIRE(!(output1 != output2));
    }
    SECTION("Different types") {
        output1.set_type<int>();
        REQUIRE(output1 != output2);
        REQUIRE(!(output1 == output2));
    }
    SECTION("Different descriptions") {
        output1.set_description("hello world");
        REQUIRE(output1 != output2);
        REQUIRE(!(output1 == output2));
    }
    SECTION("Different values") {
        output1.set_type<int>();
        output2.set_type<int>();
        output1.change(int{4});
        REQUIRE(output1 != output2);
        REQUIRE(!(output1 == output2));
    }
}

TEST_CASE("ModuleResult Class : Copy ctor") {
    ModuleResult output1;
    output1.set_type<int>();
    ModuleResult output2(output1);
    REQUIRE(output2 == output1);
}

TEST_CASE("ModuleResult Class : Copy assignment") {
    ModuleResult output1;
    output1.set_type<int>();
    ModuleResult output2;
    auto* ptr = &(output2 = output1);
    REQUIRE(output2 == output1);
    REQUIRE(ptr == &output2);
}

TEST_CASE("ModuleResult Class : Move ctor") {
    ModuleResult output1;
    output1.set_type<int>();
    ModuleResult output2(output1);
    ModuleResult output3(std::move(output1));
    REQUIRE(output3 == output2);
}

TEST_CASE("ModuleResult Class : Move assignment") {
    ModuleResult output1;
    output1.set_type<int>();
    ModuleResult output2(output1);
    ModuleResult output3;
    auto* ptr = &(output3 = std::move(output1));
    REQUIRE(output3 == output2);
    REQUIRE(ptr == &output3);
}
