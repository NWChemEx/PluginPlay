#include <NWChemExRuntime/SerialDevice.hpp>
#define CATCH_CONFIG_MAIN
#include "catch/catch.hpp"
#include <tuple>
#include <functional> //For std::plus

/*
 * Testing of the Device base class makes use of the SerialDevice class, which
 * in turn is the simplest possible Device implementation.
 */

using namespace NWChemExRuntime;

int function1()
{
    return 1;
}

int function2(int i)
{
    return i;
}


TEST_CASE("Add Task")
{
    SerialDevice device;
    SECTION("Function no args")
    {
        auto f=device.add_task(function1);
        REQUIRE(f.get()==1);
    }

    SECTION("Function args")
    {
        auto f=device.add_task(function2,3);
        REQUIRE(f.get()==3);
    }

    SECTION("Lambda no args")
    {
        auto f=device.add_task([](){return 1;});
        REQUIRE(f.get()==1);
    }

    SECTION("LAMBDA args")
    {
        auto f=device.add_task([](int i){return i;},3);
        REQUIRE(f.get()==3);
    }
}

TEST_CASE("Foreach")
{
    SerialDevice device;
    for(auto x:{make_pair(std::vector<int>({}),"Empty range"),
                make_pair(std::vector<int>({1,2,3}),"Non-empty range")})
    {
        SECTION(x.second)
        {
            auto input=x.first;
            SECTION("function")
            {
                auto f=device.foreach(input.begin(),input.end(),function2);
                REQUIRE(f.get()==input);
            }

            SECTION("lamda")
            {
                auto f=device.foreach(input.begin(),input.end(),[](int i)
                {return i;});
                REQUIRE(f.get()==input);
            }
        }
    }
}

TEST_CASE("Reduction")
{
    SerialDevice device;
    //Note: auto x=int() will make x==0

    for(auto x:{make_tuple(std::vector<int>({}),"Empty range",0),
                make_tuple(std::vector<int>({1,2,3}),"Non-empty range",6)})
    {
        SECTION(std::get<1>(x))
        {
            auto input=std::get<0>(x);
            std::plus<int> reduce;
            SECTION("function")
            {
                auto f=device.reduce(input.begin(),input.end(),function2,
                                     reduce);
                REQUIRE(f.get()==std::get<2>(x));
            }

            SECTION("lamda")
            {
                auto f=device.reduce(input.begin(),input.end(),
                                     [](int i){return i;},reduce);
                REQUIRE(f.get()==std::get<2>(x));
            }
        }
    }
}
