#include <SDE/detail_/ModuleIOOptions.hpp>
#include <catch2/catch.hpp>
using namespace SDE;

/* The ModuleIO, ModuleInput, and ModuleOutput classes are designed to be
 * constructed via the ModuleIOOptions, ModuleInputOptions, and
 * ModuleOutputOptions classes respectively. Thus each test case starts with us
 * respectively
 * 
 *
 */

TEST_CASE("ModuleIO Class") {
    ModuleIO io;
    detail_::ModuleIOOptions opt(io);

    SECTION("Set the type") {
        opt.type<int>();
        SECTION("Can set to valid type"){
            io.change(int{3});
            REQUIRE(io.value<int>() == 3);
        }
        SECTION("Prevents setting to other type"){
            REQUIRE_THROWS_AS(io.change(double{3.14}), std::invalid_argument);
        }
    }

//    SECTION("Set the description") {
//        opt.description("A description");
//        REQUIRE(io.desc == "A description");
//    }
//
//    SECTION("Set the default value"){
//        int value = 3;
//        opt.default_value(value);
//        REQUIRE(io.value<int>() == value);
//        REQUIRE(io.type() == typeid(int));
//    }
//
//    SECTION("Add a check"){
//        int value = 3, other = 4;
//        typename ModuleIO::value_checker<int> check =
//            [](const int& x){ return x == 3; };
//        opt.check(check);
//        REQUIRE(io.is_valid(value));
//        REQUIRE(!io.is_valid(other));
//        REQUIRE_THROWS_AS(io.change(other), std::invalid_argument);
//    }
//
//    int value = 3;
//    opt.default_value(value);
//
//    SECTION("Change") {
//        REQUIRE(io.value<int>() == value);
//        io.change(int{4});
//        REQUIRE(io.value<int>() == 4);
//    }
    
}
