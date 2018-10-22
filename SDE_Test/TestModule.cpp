#include <SDE/Cache.hpp>
#include <SDE/Module.hpp>
#include <array>
#include <catch/catch.hpp>

using namespace SDE;
using module_pointer   = typename ModuleBase::module_pointer;
using metadata_type    = typename ModuleBase::metadata_type;
using submodule_list   = typename ModuleBase::submodule_list;
using traits_type      = typename ModuleBase::traits_type;
using not_ready_return = typename ModuleBase::not_ready_return;

// Declare some Module types
DEFINE_PROPERTY_TYPE(TestProperty, int, int);

static Parameters default_params() {
    Parameters ps;
    ps.insert("option 1", Option(3.14));
    return ps;
}

// A mock up of a module has a missing submodule
struct MyProp1 : TestProperty {
    MyProp1() {
        submodules_["Prop1"]          = nullptr;
        metadata_[MetaProperty::name] = "Prop1";
        parameters_                   = default_params();
    }
    int run(int x) { return x + 1; }
};

// Instance to be used as not ready submodule
static const auto prop1 = std::make_shared<MyProp1>();

// Mock up with a default submodule
struct MyProp2 : TestProperty {
    MyProp2() {
        submodules_["Prop1"] = prop1;
        parameters_          = default_params();
    }
    int run(int x) { return x + 1; }
};

// Module that's r2g
struct MyProp3 : TestProperty {
    MyProp3() = default;
    int run(int x) { return x + 1; }
};

// Instance to be used as ready submodule
static const auto prop3 = std::make_shared<MyProp3>();

// The correct hashes
static const std::array<std::string, 2> corr_hashes = {
  "e32f337e89481ed70947fa40a4f86e75", "6a12f500f5edc4bfcd41861dace13eef"};

// The correct memoization hashes
static const std::array<std::string, 2> corr_memo = {
  "466f630617f08d6e1fa2f195a3884fd2", "648ddb25b747fae7d2b39d2602a056d3"};

// Function that checks the state of a module
template<std::size_t i, typename PropertyType>
void test_module(metadata_type met, submodule_list subs) {
    PropertyType mod;

    // Check hash/memoization
    Hasher h(HashType::Hash128);
    mod.hash(h);
    auto hv = bphash::hash_to_string(h.finalize());
    REQUIRE(corr_hashes[i] == hv);
    hv = bphash::hash_to_string(mod.memoize(2));
    REQUIRE(corr_memo[i] == hv);

    // Check accessors
    REQUIRE(mod.parameters() == default_params());
    REQUIRE(mod.submodules() == subs);
    REQUIRE(mod.metadata() == met);

    // Check state
    REQUIRE(!mod.locked());
    REQUIRE(!mod.is_cached(2));
    REQUIRE(mod.not_ready().size() == 1);

    // Change submodule (makes it valid)
    mod.change_submodule("Prop1", prop3);
    subs["Prop1"] = prop3;
    REQUIRE(mod.submodules() == subs);
    mod.change_parameter("option 1", 1.1);
    auto p2 = default_params();
    p2.change("option 1", 1.1);
    REQUIRE(mod.parameters() == p2);

    // Repeat for parameters

    // Lock the module and check that we can't change state
    mod.lock();
    REQUIRE(mod.locked());
    REQUIRE_THROWS_AS(mod.change_submodule("Prop1", prop1), std::runtime_error);
    REQUIRE_THROWS_AS(mod.change_parameter("option 1", 7.57),
                      std::runtime_error);

    // Check we can run it
    REQUIRE(*mod.template run_as<PropertyType>(2) == 3);
}

TEST_CASE("ModuleBase Class") {
    SECTION("ModuleBase Typedefs") {
        using correct_pointer = std::shared_ptr<ModuleBase>;
        using correct_meta    = std::map<MetaProperty, std::string>;
        using correct_list    = Utilities::CaseInsensitiveMap<module_pointer>;
        using correct_traits  = std::set<ModuleTraits>;
        using correct_return =
          std::vector<std::pair<module_pointer, ModuleProperty>>;

        REQUIRE(std::is_same<correct_pointer, module_pointer>::value);
        REQUIRE(std::is_same<correct_meta, metadata_type>::value);
        REQUIRE(std::is_same<correct_list, submodule_list>::value);
        REQUIRE(std::is_same<correct_traits, traits_type>::value);
        REQUIRE(std::is_same<correct_return, not_ready_return>::value);
    }

    SECTION("ModuleBase") {
        metadata_type md_empty{};
        submodule_list sm_empty{};

        SECTION("Missing Submodule") {
            metadata_type md_full{{MetaProperty::name, "Prop1"}};
            submodule_list sm_full{{"Prop1", nullptr}};
            test_module<0, MyProp1>(md_full, sm_full);
        }

        SECTION("Not Ready Submodule") {
            submodule_list sm_full{{"Prop1", prop1}};
            test_module<1, MyProp2>(md_empty, sm_full);
        }

        SECTION("Cache Module Results") {
            struct MyProp3 mp;
            std::shared_ptr<Cache> cp = std::make_shared<Cache>();
            mp.set_cache(cp);
            REQUIRE(mp.is_cached(10) == 0);
            mp.run_as<MyProp3>(10);
            REQUIRE(mp.is_cached(10) == 1);
        }
    }
}
