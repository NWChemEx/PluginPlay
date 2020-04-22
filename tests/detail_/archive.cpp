#include <catch2/catch.hpp>
#include "sde/detail_/archive.hpp"
#include <sstream>
#include <fstream>
#include <iostream>
#include <tiledarray.h>
#include <madness/world/MADworld.h>
#include <madness/world/worlddc.h>


TEST_CASE("Archive fundamental types: cereal::BinaryOutputArchive") {
    bool b = true, b2;
    char c = 'a', c2;
    int i = 0, i2;
    float f = 0.0, f2;
    double d = 0.0, d2;
    std::stringstream ss;
    {
      sde::BinaryOutputArchive oarchive(ss);
      oarchive(b,c,d,f,i);
    }
    sde::BinaryInputArchive iarchive(ss);
    iarchive(b2,c2,d2,f2,i2);
    REQUIRE(b==b2);
    REQUIRE(c==c2);
    REQUIRE(d==d2);
    REQUIRE(f==f2);
    REQUIRE(i==i2);
}

TEST_CASE("Archive fundamental types: cereal::PortableBinaryOutputArchive") {
    bool b = true, b2;
    char c = 'a', c2;
    int i = 0, i2;
    float f = 0.0, f2;
    double d = 0.0, d2;
    std::stringstream ss;
    {
      sde::PortableBinaryOutputArchive oarchive(ss);
      oarchive(b,c,d,f,i);
    }
    sde::PortableBinaryInputArchive iarchive(ss);
    iarchive(b2,c2,d2,f2,i2);
    REQUIRE(b==b2);
    REQUIRE(c==c2);
    REQUIRE(d==d2);
    REQUIRE(f==f2);
    REQUIRE(i==i2);
}

TEST_CASE("Archive fundamental types: cereal::XMLOutputArchive") {
    bool b = true, b2;
    char c = 'a', c2;
    int i = 0, i2;
    float f = 0.0, f2;
    double d = 0.0, d2;
    std::stringstream ss;
    {
      sde::XMLOutputArchive oarchive(ss);
      oarchive(b,c,d,f,i);
    }
    sde::XMLInputArchive iarchive(ss);
    iarchive(b2,c2,d2,f2,i2);
    REQUIRE(b==b2);
    REQUIRE(c==c2);
    REQUIRE(d==d2);
    REQUIRE(f==f2);
    REQUIRE(i==i2);
}

TEST_CASE("Archive fundamental types: cereal::JSONOutputArchive") {
    bool b = true, b2;
    char c = 'a', c2;
    int i = 0, i2;
    float f = 0.0, f2;
    double d = 0.0, d2;
    std::stringstream ss;
    {
      sde::JSONOutputArchive oarchive(ss);
      oarchive(b,c,d,f,i);
    }
    sde::JSONInputArchive iarchive(ss);
    {iarchive(b2,c2,d2,f2,i2);}
    REQUIRE(b==b2);
    REQUIRE(c==c2);
    REQUIRE(d==d2);
    REQUIRE(f==f2);
    REQUIRE(i==i2);
}

TEST_CASE("Archive fundamental types: madness::BinaryFstreamArchive") {
    bool b = true, b2;
    char c = 'a', c2;
    double d = 0.0, d2;
    float f = 0.0, f2;
    int i = 0, i2;
    const char *file = "test.dat";
    sde::BinaryFstreamOutputArchive oarchive( file );
    oarchive & b & c & d & f & i;
    oarchive.close();
    sde::BinaryFstreamInputArchive iarchive( file );
    iarchive & b2 & c2 & d2 & f2 & i2;;
    iarchive.close();
    REQUIRE(b==b2);
    REQUIRE(c==c2);
    REQUIRE(d==d2);
    REQUIRE(f==f2);
    REQUIRE(i==i2);
}

TEST_CASE("Archive fundamental types: madness::TextFstreamArchive") {
    bool b = true, b2;
    char c = 'a', c2;
    double d = 0.0, d2;
    float f = 0.0, f2;
    int i = 0, i2;
    const char *file = "test.dat";
    sde::TextFstreamOutputArchive oarchive( file );
    oarchive & b & c & d & f & i;
    oarchive.close();
    sde::TextFstreamInputArchive iarchive( file );
    iarchive & b2 & c2 & d2 & f2 & i2;;
    iarchive.close();
    REQUIRE(b==b2);
    REQUIRE(c==c2);
    REQUIRE(d==d2);
    REQUIRE(f==f2);
    REQUIRE(i==i2);
}

TEST_CASE("Archive fundamental types: madness::BufferArchive") {
    bool b = true, b2;
    char c = 'a', c2;
    double d = 0.0, d2;
    float f = 0.0, f2;
    int i = 0, i2;
    unsigned char buf[32768];  
    sde::BufferOutputArchive oarchive(buf, sizeof(buf) );
    oarchive & b & c & d & f & i;
    oarchive.close();
    sde::BufferInputArchive iarchive(buf, oarchive.size() );
    iarchive & b2 & c2 & d2 & f2 & i2;;
    iarchive.close();
    REQUIRE(b==b2);
    REQUIRE(c==c2);
    REQUIRE(d==d2);
    REQUIRE(f==f2);
    REQUIRE(i==i2);
}

TEST_CASE("Archive TA::TArrayD: madness::BinaryFstreamArchive") {
  auto& world = TA::get_default_world();
  TA::TiledRange trange = 
      { { 0, 10, 20},
        { 0, 10, 20} };
  TA::TArrayD A(world, trange);
  TA::TArrayD B(world, trange);
  A.fill_local(3.0);
  B.fill_local(0.0); 
  std::ostringstream ssA, ssB;
  ssA << A;
  const char *f = "test.dat";
  sde::BinaryFstreamOutputArchive oarchive( f );
  oarchive & A;
  oarchive.close();
  sde::BinaryFstreamInputArchive iarchive( f );
  iarchive & B;
  ssB << B;
  iarchive.close();
  REQUIRE(ssA.str()==ssB.str());
}

TEST_CASE("Archive TA::TArrayD: madness::TextFstreamArchive") {
  auto& world = TA::get_default_world();
  TA::TiledRange trange = 
      { { 0, 10, 20},
        { 0, 10, 20} };
  TA::TArrayD A(world, trange);
  TA::TArrayD B(world, trange);
  A.fill_local(3.0);
  B.fill_local(0.0); 
  std::ostringstream ssA, ssB;
  ssA << A;
  const char *f = "test.dat";
  sde::TextFstreamOutputArchive oarchive( f );
  oarchive & A;
  oarchive.close();
  // Fails
  // sde::TextFstreamInputArchive iarchive( f );
  // iarchive & B;
  // ssB << B;
  // iarchive.close();
  // REQUIRE(ssA.str()==ssB.str());
}

TEST_CASE("Archive TA::TArrayD: madness::BufferArchive") {
  auto& world = TA::get_default_world();
  TA::TiledRange trange = 
      { { 0, 10, 20},
        { 0, 10, 20} };
  TA::TArrayD A(world, trange);
  TA::TArrayD B(world, trange);
  A.fill_local(3.0);
  B.fill_local(0.0); 
  std::ostringstream ssA, ssB;
  ssA << A;
  unsigned char buf[32768];  
  sde::BufferOutputArchive oarchive(buf, sizeof(buf) );
  oarchive & A;
  oarchive.close();
  sde::BufferInputArchive iarchive(buf, oarchive.size() );
  iarchive & B;
  ssB << B;
  iarchive.close();
  REQUIRE(ssA.str()==ssB.str());
}

TEST_CASE("Archive TA::TArrayD: madness::CerealBinaryArchive") {
  auto& world = TA::get_default_world();
  TA::TiledRange trange = 
      { { 0, 10, 20},
        { 0, 10, 20} };
  TA::TArrayD A(world, trange);
  TA::TArrayD B(world, trange);
  A.fill_local(3.0);
  B.fill_local(0.0); 
  std::ostringstream ssA, ssB;
  ssA << A;
  std::stringstream ss;
  {
    sde::CerealBinaryOutputArchive oarchive(ss);
    oarchive & A ;
  }  
  sde::CerealBinaryInputArchive iarchive(ss);
  iarchive & B;
  ssB << B;
  iarchive.close();
  REQUIRE(ssA.str()==ssB.str());
}

TEST_CASE("Archive TA::TArrayD: madness::CerealPortableBinaryArchive") {
  auto& world = TA::get_default_world();
  TA::TiledRange trange = 
      { { 0, 10, 20},
        { 0, 10, 20} };
  TA::TArrayD A(world, trange);
  TA::TArrayD B(world, trange);
  A.fill_local(3.0);
  B.fill_local(0.0); 
  std::ostringstream ssA, ssB;
  ssA << A;
  std::stringstream ss;
  {
    sde::CerealPortableBinaryOutputArchive oarchive(ss);
    oarchive & A ;
  }  
  sde::CerealPortableBinaryInputArchive iarchive(ss);
  iarchive & B;
  ssB << B;
  iarchive.close();
  REQUIRE(ssA.str()==ssB.str());
}

TEST_CASE("Archive TA::TArrayD: madness::CerealJSONArchive") {
  auto& world = TA::get_default_world();
  TA::TiledRange trange = 
      { { 0, 10, 20},
        { 0, 10, 20} };
  TA::TArrayD A(world, trange);
  TA::TArrayD B(world, trange);
  A.fill_local(3.0);
  B.fill_local(0.0); 
  std::ostringstream ssA, ssB;
  ssA << A;
  std::stringstream ss;
  {
    sde::CerealJSONOutputArchive oarchive(ss);
    oarchive & A ;
  }  
  sde::CerealJSONInputArchive iarchive(ss);
  iarchive & B;
  ssB << B;
  iarchive.close();
  REQUIRE(ssA.str()==ssB.str());
}

TEST_CASE("Archive TA::TArrayD: madness::CerealXMLArchive") {
  auto& world = TA::get_default_world();
  TA::TiledRange trange = 
      { { 0, 10, 20},
        { 0, 10, 20} };
  TA::TArrayD A(world, trange);
  TA::TArrayD B(world, trange);
  A.fill_local(3.0);
  B.fill_local(0.0); 
  std::ostringstream ssA, ssB;
  ssA << A;
  std::stringstream ss;
  {
    sde::CerealXMLOutputArchive oarchive(ss);
    oarchive & A ;
  }  
  sde::CerealXMLInputArchive iarchive(ss);
  iarchive & B;
  ssB << B;
  iarchive.close();
  REQUIRE(ssA.str()==ssB.str());
}

TEST_CASE("Archive TA::TSpArrayD: madness::CerealBinaryArchive") {
  auto& world = TA::get_default_world();
  TA::TSpArrayD A(world, {1.0, 2.0, 3.0});
  TA::TSpArrayD B(world, {0.0, 0.0, 0.0});
  std::ostringstream ssA, ssB;
  ssA << A;
  std::stringstream ss;
  {
    sde::CerealBinaryOutputArchive oarchive(ss);
    oarchive & A ;
  }  
  sde::CerealBinaryInputArchive iarchive(ss);
  iarchive & B;
  ssB << B;
  iarchive.close();
  REQUIRE(ssA.str()==ssB.str());
}

TEST_CASE("Archive TA::TSpArrayD: madness::BinaryFstreamArchive") {
  auto& world = TA::get_default_world();
  TA::TSpArrayD A(world, {1.0, 2.0, 3.0});
  TA::TSpArrayD B(world, {0.0, 0.0, 0.0});
  std::ostringstream ssA, ssB;
  ssA << A;
  const char *f = "test.dat";
  sde::BinaryFstreamOutputArchive oarchive( f );
  oarchive & A;
  oarchive.close();
  sde::BinaryFstreamInputArchive iarchive( f );
  iarchive & B;
  ssB << B;
  iarchive.close();
  REQUIRE(ssA.str()==ssB.str());
}