#include <catch2/catch.hpp>
#include <sstream>
#include "sde/detail_/memoization.hpp"
#include <tiledarray.h>

TEST_CASE("BPHASH: TA::TArrayD") {
  auto& world = TA::get_default_world();
  TA::TiledRange trange = 
      { { 0, 10, 20},
        { 0, 10, 20} };
  TA::TiledRange trange2 = 
      { { 1, 3, 5, 7, 9},
        { 2, 4} };
  TA::TArrayD A(world, trange);
  TA::TArrayD B(world, trange);
  TA::TArrayD C(world, trange);
  TA::TArrayD D(world, trange2);

  A.fill_local(3.0);
  B.fill_local(3.0); 
  C.fill_local(0.0); 
  D.fill_local(0.0);
  std::ostringstream ssA, ssB, ssC, ssD, ssE;
  ssA << A;
  ssB << B;
  ssC << C;
  ssD << D;
  auto hashA = bphash::make_hash(bphash::HashType::Hash128, ssA.str());
  auto hashB = bphash::make_hash(bphash::HashType::Hash128, ssB.str());
  auto hashC = bphash::make_hash(bphash::HashType::Hash128, ssC.str());
  auto hashD = bphash::make_hash(bphash::HashType::Hash128, ssD.str());

//   std::cout << "BPHASH A(3.0): " << bphash::hash_to_string(hashA) << std::endl;
//   std::cout << "BPHASH B(3.0): " << bphash::hash_to_string(hashB) << std::endl;
//   std::cout << "BPHASH C(0.0): " << bphash::hash_to_string(hashC) << std::endl;
//   std::cout << "BPHASH D(0.0): " << bphash::hash_to_string(hashD) << std::endl;

  REQUIRE(hashA==hashB);
  REQUIRE(hashA!=hashC);
  REQUIRE(hashC!=hashD);
  REQUIRE(bphash::hash_to_string(hashA)=="a97c3593b8fef55c24f078af2c9183fd");
  REQUIRE(bphash::hash_to_string(hashC)=="5f28089c317e91b11543eedb18fd41b3");
  REQUIRE(bphash::hash_to_string(hashD)=="f06cdea494e1bba827add406a9325a2e");
}

TEST_CASE("MADNESS Hash: TA::TArrayD") {
  auto& world = TA::get_default_world();
  TA::TiledRange trange = 
      { { 0, 10, 20},
        { 0, 10, 20} };
  TA::TiledRange trange2 = 
      { { 1, 3, 5, 7, 9},
        { 2, 4} };
  TA::TArrayD A(world, trange);
  TA::TArrayD B(world, trange);
  TA::TArrayD C(world, trange);
  TA::TArrayD D(world, trange2);

  A.fill_local(3.0);
  B.fill_local(3.0); 
  C.fill_local(0.0); 
  D.fill_local(0.0);
  std::ostringstream ssA, ssB, ssC, ssD, ssE;
  ssA << A;
  ssB << B;
  ssC << C;
  ssD << D;
  auto hashA = madness::hash_value(ssA.str());
  auto hashB = madness::hash_value(ssB.str());
  auto hashC = madness::hash_value(ssC.str());
  auto hashD = madness::hash_value(ssD.str());

//   std::cout << "madness::hash_value A(3.0): " << hashA << std::endl;
//   std::cout << "madness::hash_value B(3.0): " << hashB << std::endl;
//   std::cout << "madness::hash_value C(0.0): " << hashC << std::endl;
//   std::cout << "madness::hash_value D(0.0): " << hashD << std::endl;

  REQUIRE(hashA==hashB);
  REQUIRE(hashA!=hashC);
  REQUIRE(hashC!=hashD);
  REQUIRE(hashA==2385643958);
  REQUIRE(hashC==1156223083);
  REQUIRE(hashD==941256217);
}