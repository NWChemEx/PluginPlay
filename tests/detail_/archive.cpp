#include <catch2/catch.hpp>
#include "sde/detail_/archive.hpp"
#include <sstream>
#include <fstream>
#include <iostream>


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
