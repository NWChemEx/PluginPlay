#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>
#include <madness/world/MADworld.h>

int main(int argc, char* argv[]) {
  auto& world = madness::initialize(argc, argv);

  int res = Catch::Session().run(argc, argv);

  madness::finalize();
  return res;
}

