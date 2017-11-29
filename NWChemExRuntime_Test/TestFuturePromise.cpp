#include <NWChemExRuntime/Promise.hpp> //Includes Future.hpp
#define CATCH_CONFIG_MAIN
#include "catch/catch.hpp"

using namespace NWChemExRuntime;

TEST_CASE("Promise to POD type")
{
    Promise<double> p;
    Future<double> f=std::move(p.get_future());

    SECTION("Future from Promise is valid")
    {
        REQUIRE(f.valid());
    }

    SECTION("Can't get two futures")
    {
        REQUIRE_THROWS_AS(p.get_future(),std::logic_error);
    }

    double value=3.1;
    SECTION("Future recieves value from Promise")
    {
        p.set_value(value);

        SECTION("Can't set value twice")
        {
            REQUIRE_THROWS_AS(p.set_value(value),std::logic_error);
        }

        REQUIRE(f.get()==value);
    }

    SECTION("Promise can be moved")
    {
        Promise<double> p2(std::move(p));
        p2.set_value(value);
        REQUIRE(f.get()==value);
    }

    SECTION("Future can be moved")
    {
        Future<double> f2;
        SECTION("Default future is invalid")
        {
            REQUIRE(!f2.valid());
        }

        f2=std::move(f);
        SECTION("Default future can be made valid")
        {
            REQUIRE(f2.valid());
        }

        p.set_value(value);
        REQUIRE(f2.get()==value);
    }
}

TEST_CASE("Promise to void")
{
    Promise<void> p;
    Future<void> f=std::move(p.get_future());
    p.set_value();
    REQUIRE_NOTHROW(f.wait());
}
