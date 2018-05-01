#include <Utilities/Any.hpp>
#include <catch/catch.hpp>
#include <vector>

using namespace Utilities;

template<typename T>
void check_state(SDEAny& da_any, std::initializer_list<T> contents) {
    // Const reference for checking functions on const instances
    const SDEAny& const_da_any = da_any;

    REQUIRE(da_any.has_value() == contents.size());
    REQUIRE(const_da_any.has_value() == contents.size());

    REQUIRE(da_any.type() == typeid(T));
    REQUIRE(const_da_any.type() == typeid(T));
}

TEST_CASE("Constructors") {
    SDEAny defaulted;
    check_state(defaulted, {});

    SDEAny copy_default(defaulted);
    check_state(copy_defaulted, {});

    SDEAny wrap_double(double{3.14});
    check_state(wrap_double, )
}

TEST_CASE("Defaulted Any and Basic operations") {
Any defaulted{};
REQUIRE(!defaulted.has_value());
defaulted.emplace<int>(2);
REQUIRE(defaulted.has_value());
REQUIRE(AnyCast<int>(defaulted) == 2);
REQUIRE_THROWS_AS(AnyCast<std::vector<int>>(defaulted), std::bad_cast);
defaulted.reset();
REQUIRE(!defaulted.has_value());
}

TEST_CASE("Test non-defaulted Any construction") {
int two = 2, three = 3;
Any wrapped_two = MakeAny<int>(2);
Any wrapped_three(three);
int* ptr_2_two   = &(AnyCast<int>(wrapped_two));
int* ptr_2_three = &(AnyCast<int>(wrapped_three));

SECTION("Copy constructor") {
Any copied_three(wrapped_three);
int* ptr_2_three2 = &(AnyCast<int>(copied_three));
REQUIRE(ptr_2_three2 != ptr_2_three);
REQUIRE(*ptr_2_three2 == 3);
}

SECTION("Move constructor") {
Any moved_three(std::move(wrapped_three));
REQUIRE(&(AnyCast<int>(moved_three)) == ptr_2_three);
}

SECTION("Copy assignment") {
Any* ptr = &(wrapped_two = wrapped_three);
REQUIRE(ptr == &wrapped_two);
int* ptr_2_three2 = &(AnyCast<int>(wrapped_two));
REQUIRE(ptr_2_three != ptr_2_three2);
REQUIRE(*ptr_2_three2 == 3);
}

SECTION("Move assignment") {
Any* ptr = &(wrapped_two = std::move(wrapped_three));
REQUIRE(ptr == &wrapped_two);
REQUIRE(&(AnyCast<int>(wrapped_two)) == ptr_2_three);
}

SECTION("Swap") {
wrapped_two.swap(wrapped_three);
REQUIRE(&(AnyCast<int>(wrapped_two)) == ptr_2_three);
REQUIRE(&(AnyCast<int>(wrapped_three)) == ptr_2_two);
}
}

TEST_CASE("Wrapping Pointers") {
std::vector<int> vals({1, 2, 3});
int* ptr = vals.data();
Any wrapped_vals(ptr);
REQUIRE(AnyCast<int*>(wrapped_vals) == ptr);
}

TEST_CASE("MakeAny function") {
auto an_any = MakeAny<std::vector<int>>(3, 4);
std::vector<int> corr(3, 4);
REQUIRE(AnyCast<std::vector<int>>(an_any) == corr);
}
