#include "sde/detail_/archive.hpp"
#include <catch2/catch.hpp>
#include <sstream>
#include <fstream>
#include <tiledarray.h>

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
    REQUIRE(i==j);
}

TEST_CASE("Archive: TA::TSpArrayD") {
  auto& world = TA::get_default_world();
  TA::TSpArrayD vec(world, {1.0, 2.0, 3.0});
  const char *f = "test.dat";
  sde::BinaryFstreamOutputArchive oarchive( f );
  oarchive & vec;
  oarchive.close();
  sde::BinaryFstreamInputArchive iarchive( f );
  TA::TSpArrayD vec2(world, {0.0, 0.0, 0.0});
  iarchive & vec2;
  iarchive.close();
}

TEST_CASE("Archive: madness::archive::BinaryFstreamOutputArchive") {
  int i=2;
  const char *f = "test.dat";
  sde::BinaryFstreamOutputArchive oarchive( f );
  oarchive & i;
  oarchive.close();
  sde::BinaryFstreamInputArchive iarchive( f );
  int j;
  iarchive & j;
  iarchive.close();
  REQUIRE(i==j);
}