#include <SDE/attic/ModuleHelpers.hpp>
#include <catch2/catch.hpp>
#include <type_traits>

using namespace SDE::detail_;

struct A {
    void run() {}
};

struct B {
    int run(int) { return 1; }
};

struct C {
    int one = 1;
    int& run(int&) { return one; }
};

template<typename class_name, typename corr_return, typename corr_args>
void check_types() {
    using return_type = RunDetails_return_type<class_name>;
    using args_type   = RunDetails_args_type<class_name>;

    REQUIRE(std::is_same<return_type, corr_return>::value);
    REQUIRE(std::is_same<args_type, corr_args>::value);
};

TEST_CASE("RunDetails") {
    SECTION("void return, no args") { check_types<A, void, std::tuple<>>(); }

    SECTION("POD return, POD args") { check_types<B, int, std::tuple<int>>(); }

    SECTION("Reference return, reference args") {
        check_types<C, int&, std::tuple<int&>>();
    }
}
