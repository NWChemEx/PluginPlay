#include <SDE/ModuleManager.hpp>
#include <catch/catch.hpp>

using namespace SDE;
using key_set = std::set<std::string>;

SDE_NEW_MODULE_TYPE(FakeModuleType, bool, int);

class FakeModule : public FakeModuleType {
    bool run_(int) { return true; }
};

using TestLoader = ModuleLoader<FakeModule>;

TEST_CASE("Typedefs") {
    using key_type = typename ModuleManager::key_type;
    REQUIRE(std::is_same<key_type, std::string>::value);

    using loader_type = typename ModuleManager::loader_type;
    REQUIRE(std::is_same<loader_type, ModuleLoaderBase>::value);

    using module_pointer = typename ModuleManager::module_pointer;
    REQUIRE(std::is_same<module_pointer, std::unique_ptr<ModuleBase>>::value);
}

void check_state(ModuleManager& mm, key_set keys) {
    REQUIRE_THROWS_AS(mm.insert("", TestLoader{}), std::invalid_argument);
    REQUIRE_THROWS_AS(mm.is_locked("Not an actual key"), std::out_of_range);
    REQUIRE(!mm.count("Not an actual key"));
    for(const auto& x : keys) {
        REQUIRE(mm.count(x));
        REQUIRE_THROWS_AS(mm.insert(x, TestLoader{}), std::range_error);
        auto mod = mm.get_module(x);
        REQUIRE(mm.is_locked(x));
        auto new_key = mm.duplicate(x);
        REQUIRE(mm.count(new_key));
        REQUIRE(!mm.is_locked(new_key));
        REQUIRE_THROWS_AS(mm.duplicate(x, new_key), std::range_error);
    }
}

TEST_CASE("ModuleManager") {
    ModuleManager mm;
    SECTION("Default Ctor") { check_state(mm, key_set{}); }

    const std::string a_key{"a key"};
    mm.insert(a_key, TestLoader{});

    SECTION("Filled MM") { check_state(mm, key_set{a_key}); }

    SECTION("Move Ctor") {
        ModuleManager mm2(std::move(mm));
        check_state(mm2, key_set{a_key});
    }

    SECTION("Move Assign") {
        ModuleManager mm2;
        mm2 = std::move(mm);
        check_state(mm2, key_set{a_key});
    }
}
