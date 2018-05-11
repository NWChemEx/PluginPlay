#include <SDE/ModuleLoader.hpp>
#include <catch/catch.hpp>

using namespace SDE;

SDE_NEW_MODULE_TYPE(FakeModuleType, bool, int);

class FakeModule : public FakeModuleType {
    bool run_(int) { return true; }
};

using TestLoader = ModuleLoader<FakeModule>;

TEST_CASE("Typedefs") {
    using module_pointer = typename TestLoader::module_pointer;
    using loader_pointer = typename TestLoader::loader_pointer;
    REQUIRE(std::is_same<std::unique_ptr<ModuleBase>, module_pointer>::value);
    REQUIRE(
      std::is_same<std::unique_ptr<ModuleLoaderBase>, loader_pointer>::value);
}

TEST_CASE("ModuleLoader") {
    TestLoader l;

    SECTION("get_module") {
        auto mod = l.get_module();
        REQUIRE(mod->type() == typeid(FakeModuleType));
    }
}
