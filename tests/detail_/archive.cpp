#include "sde/detail_/archive.hpp"
#include <catch2/catch.hpp>
#include <sstream>
#include <fstream>


TEST_CASE("Archive: cereal::BinaryOutputArchive") {
    int i = 2;
    int j;
    std::stringstream ss;
    sde::BinaryOutputArchive oarchive( ss );
    oarchive(i);
    sde::BinaryInputArchive iarchive( ss );
    iarchive(j);
    REQUIRE(j==2);
    REQUIRE(i==j);
}

TEST_CASE("Archive: cereal::XMLOutputArchive") {
    {
    int i = 2;
    std::stringstream ss;
    std::ofstream os("data.xml");
    sde::XMLOutputArchive oarchive( os );
    oarchive(i);
    }
    {
    std::ifstream is("data.xml");
    sde::XMLInputArchive iarchive( is );
    int j;
    iarchive(j);
    REQUIRE(j==2);
    }
}

TEST_CASE("Archive: cereal::JSONOutputArchive") {
    {
    int i = 2;
    std::ofstream os("data.json");
    sde::JSONOutputArchive oarchive( os );
    oarchive(i);
    }
    {
    int j;
    std::ifstream is("data.json");
    sde::JSONInputArchive iarchive( is );
    iarchive(j);
    REQUIRE(j==2);
    }
}

TEST_CASE("Archive: cereal::PortableBinaryOutputArchive") {
    int i = 2;
    int j;
    std::stringstream ss;
    sde::PortableBinaryOutputArchive oarchive( ss );
    oarchive(i);
    sde::PortableBinaryInputArchive iarchive( ss );
    iarchive(j);
    REQUIRE(j==2);
    REQUIRE(i==j);
}