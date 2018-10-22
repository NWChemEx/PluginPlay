#include <SDE/ModuleManager.hpp>
#include <catch/catch.hpp>

using namespace SDE;
using key_type       = typename ModuleManager::key_type;
using key_set        = std::set<key_type>;
using module_pointer = typename ModuleManager::module_pointer;
using loader_type    = typename ModuleManager::loader_type;
using size_type      = typename ModuleManager::size_type;

DEFINE_PROPERTY_TYPE(FakePropertyType, int, int);

class FakeModule : public FakePropertyType {
    int run(int i) override { return i + 1; }
};

module_pointer test_loader() { return std::make_shared<FakeModule>(); }

void check_state(ModuleManager& mm, key_set keys) {
    REQUIRE_THROWS_AS(mm.insert("", test_loader), std::invalid_argument);
    REQUIRE_THROWS_AS(mm.at("Not an actual key"), std::out_of_range);
    REQUIRE(!mm.count("Not an actual key"));
    for(const auto& x : keys) {
        REQUIRE(mm.count(x));
        REQUIRE_THROWS_AS(mm.insert(x, test_loader), std::range_error);
        auto new_key = mm.duplicate(x);
        REQUIRE(mm.count(new_key));
        REQUIRE_THROWS_AS(mm.duplicate(x, new_key), std::range_error);
    }
}

TEST_CASE("ModuleManager Class") {
    SECTION("Typedefs") {
        using corr_key    = std::string;
        using corr_ptr    = typename ModuleBase::module_pointer;
        using corr_loader = std::function<module_pointer()>;
        using corr_size   = std::size_t;
        REQUIRE(std::is_same<key_type, corr_key>::value);
        REQUIRE(std::is_same<module_pointer, corr_ptr>::value);
        REQUIRE(std::is_same<loader_type, corr_loader>::value);
        REQUIRE(std::is_same<size_type, corr_size>::value);
    }

    ModuleManager mm;
    SECTION("Default Ctor") { check_state(mm, key_set{}); }

    const std::string a_key{"a key"};
    mm.insert(a_key, test_loader);

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

} // end TEST_CASE
