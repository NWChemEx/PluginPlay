#include "sde/detail_/archive.hpp"
#include <catch2/catch.hpp>
#include <sstream>
#include <fstream>
#include <tiledarray.h>
#include <catch2/catch.hpp>
#include <vector>
#include <madness/world/MADworld.h>
#include <madness/world/worlddc.h>
#include <iostream>
#include <bphash/Hasher.hpp>
#include <bphash/types/All.hpp>

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
  REQUIRE(vec.size()==vec2.size());
}

TEST_CASE("BinaryFstreamArchive: TA::TArrayD") {
  auto& world = TA::get_default_world();
  TA::TiledRange trange = 
      { { 0, 10, 20},
        { 0, 10, 20} };
  TA::TArrayD A(world, trange);
  TA::TArrayD B(world, trange);
  A.fill_local(3.0);
  B.fill_local(0.0); 
  const char *f = "test.dat";
  sde::BinaryFstreamOutputArchive oarchive( f );
  oarchive & A;
  oarchive.close();
  sde::BinaryFstreamInputArchive iarchive( f );
  iarchive & B;
  iarchive.close();
  REQUIRE(A.size()==B.size());
}

TEST_CASE("BufferArchive: TA::TArrayD") {
  auto& world = TA::get_default_world();
  TA::TiledRange trange = 
      { { 0, 10, 20},
        { 0, 10, 20} };
  TA::TArrayD A(world, trange);
  TA::TArrayD B(world, trange);
  A.fill_local(3.0);
  B.fill_local(0.0); 
  unsigned char buf[32768];  
  sde::BufferOutputArchive oarchive( buf, sizeof(buf) );
  oarchive & A;
  std::size_t nbyte = oarchive.size();
  oarchive.close();
  sde::BufferInputArchive iarchive( buf, nbyte );
  iarchive & B;
  iarchive.close();
  REQUIRE(A.size()==B.size());

}

TEST_CASE("BufferArchiveHash: TA::TArrayD") {
  auto& world = TA::get_default_world();
  TA::TiledRange trange = 
      { { 0, 10, 20},
        { 0, 10, 20} };
  TA::TArrayD A(world, trange);
  TA::TArrayD B(world, trange);
  A.fill_local(3.0);
  B.fill_local(0.0); 
  unsigned char buf[32768];  
  sde::BufferOutputArchive oarchive( buf, sizeof(buf) );
  oarchive & A;
  std::size_t nbyte = oarchive.size();
  oarchive.close();
  // MADNESS hash value varies at each run, but constant within the run
  auto hash1 = madness::hash_value(buf);
  std::cout << "MADNESS hash value: " << madness::hash_value(buf) << std::endl;
  auto hash2 = madness::hash_value(buf);
  REQUIRE(hash1==hash2);
  REQUIRE_FALSE(hash1==322879428);
  sde::BufferInputArchive iarchive( buf, nbyte );
  iarchive & B;
  iarchive.close();
  REQUIRE(A.size()==B.size());
}

TEST_CASE("BufferArchiveBPHash: TA::TArrayD") {
  auto& world = TA::get_default_world();
  TA::TiledRange trange = 
      { { 0, 10, 20},
        { 0, 10, 20} };
  TA::TArrayD A(world, trange);
  TA::TArrayD B(world, trange);
  A.fill_local(3.0);
  B.fill_local(0.0); 
  //TODO: calculate the required buffer size
  // by fitting ~: A.size() * sizeof(A) * 56 + 384;
  unsigned char buf[4000];   
  sde::BufferOutputArchive oarchive( buf, sizeof(buf) );
  oarchive & A;
  std::size_t nbyte = oarchive.size();
  oarchive.close();
  //TODO: what should be the length for pointer hash?
  auto bufhash1 = bphash::make_hash(bphash::HashType::Hash128, bphash::hash_pointer(buf,1));
  auto bufhashsize = bphash::make_hash(bphash::HashType::Hash128, bphash::hash_pointer(buf,sizeof(A)));
  auto bufhash10 = bphash::make_hash(bphash::HashType::Hash128, bphash::hash_pointer(buf,10));
  auto bufhashnbyte = bphash::make_hash(bphash::HashType::Hash128, bphash::hash_pointer(buf,200));
  // std::cout << "Archive size in bytes: " << nbyte << std::endl;
  // std::cout << "size of A: " << sizeof(A) << std::endl;
  // std::cout << "A.size: " << A.size() << std::endl;
  // std::cout << "size of buf: " << sizeof(buf) << std::endl;
  // std::cout << "size of archive: " << sizeof(oarchive) << std::endl;
  // std::cout << "BPHASH buf: " << bphash::hash_to_string(bufhash1) << std::endl;
  // std::cout << "BPHASH buf: " << bphash::hash_to_string(bufhashsize) << std::endl;
  // std::cout << "BPHASH buf: " << bphash::hash_to_string(bufhash10) << std::endl;
  // std::cout << "BPHASH buf: " << bphash::hash_to_string(bufhashnbyte) << std::endl;

  REQUIRE(bphash::hash_to_string(bufhash1)=="d4635e87d33d7c1d3731a275e324a28e");
  REQUIRE(bphash::hash_to_string(bufhashsize)=="471333803411452ca07cd4c9bd0c477e");
}

TEST_CASE("TextFstreamOutputArchive: TA::TArrayD") {
  auto& world = TA::get_default_world();
  TA::TiledRange trange = 
      { { 0, 10, 20},
        { 0, 10, 20}};
  TA::TArrayD A(world, trange);
  TA::TArrayD B(world, trange);
  std::cout << "size of A: " << sizeof(A) << std::endl;
  std::cout << "A.size: " << A.size() << std::endl;
  A.fill_local(3.0);
 // std::cout<< "A:" << A << std::endl;
  //B.fill_local(0.0); 
  const char *f = "test.xml";
  sde::TextFstreamOutputArchive oarchive( f );
  oarchive & A;
  oarchive.close();
  //sde::TextFstreamInputArchive iarchive( f );
  //iarchive & B;
  //iarchive.close();
}

TEST_CASE("Archive: TA::TArrayD") {
  auto& world = TA::get_default_world();
  TA::TiledRange trange = 
      { { 0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100 },
        { 0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100 } };
  TA::TArrayD A(world, trange);
  TA::TArrayD B(world, trange);
  A.fill_local(3.0);
  B.fill_local(0.0); 
  const char *f = "test.dat";
  sde::BinaryFstreamOutputArchive oarchive( f );
  oarchive & A;
  oarchive.close();
  sde::BinaryFstreamInputArchive iarchive( f );
  iarchive & B;
  REQUIRE(B.size()==A.size());
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

TEST_CASE("MADNESS::hash_value test") {
  int i=3;  
  auto myhash1 = madness::hash_value(i);     
  auto myhash2 = madness::hash_value(i); 
  auto myhash3 = madness::hash_value(3);
  REQUIRE(myhash1==2568451407);
//   auto& world = TA::get_default_world();
//   TA::TiledRange trange = 
//       { { 0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100 },
//         { 0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100 } };
//   TA::TArrayD A(world, trange);
//   A.fill_local(3.0);
//  // auto myhashA = madness::hash_range(A.begin(),A.end());
//  //     std::cout << myhashA << std::endl;

}

TEST_CASE("hash test 2"){
  int i=3;
  int j;
  unsigned char buf[32768];
  sde::BufferOutputArchive oar(buf, sizeof(buf));
  oar & i;
  //auto myhash1 = madness::hash_value(oar);
  //auto myhash1 = sde::hash_objects(oar);
  //std::cout << myhash1 << std::endl;
  //std::size_t nbyte = oar.size();
  //oar.close();
  //sde::BufferInputArchive iar(buf, nbyte);
  //iar & j;
  //iar.close();
  //REQUIRE(i==j);
}