#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>
#include <parallelzone/runtime.hpp>

int main(int argc, char* argv[]) {
    auto rt = parallelzone::Runtime(argc, argv);

    int res = Catch::Session().run(argc, argv);

    return res;
}
