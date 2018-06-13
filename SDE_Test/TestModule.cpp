#include <SDE/Module.hpp>
#include <array>
#include <catch/catch.hpp>

using namespace SDE;
using module_pointer   = typename ModuleBase::module_pointer;
using metadata_type    = typename ModuleBase::metadata_type;
using submodule_list   = typename ModuleBase::submodule_list;
using not_ready_return = typename ModuleBase::not_ready_return;

// Declare some Module types
DEFINE_PROPERTY_TYPE(TestProperty, int, int);

// A mock up of a module has a missing submodule
struct MyProp1 : TestProperty {
    MyProp1() {
        submodules_["Prop1"]          = nullptr;
        metadata_[MetaProperty::name] = "Prop1";
    }
    int run(int x) { return x + 1; }
};

// Instance to be used as not ready submodule
static const auto prop1 = std::make_shared<MyProp1>();

// Mock up with a default submodule
struct MyProp2 : TestProperty {
    MyProp2() { submodules_["Prop1"] = prop1; }
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
  "c7c82f313a73782127326d1bcb2103e1", "bbf4d07c4a21e02e4c4ae4381a66b402"};

// The correct memoization hashes
static const std::array<std::string, 2> corr_memo = {
  "12ab816f03b73343c3d38764e539ed15", "5392ed79e6722269e1f82b7b6c0e2286"};

// Function that checks the state of a module
template<std::size_t i, typename PropertyType>
void test_module(metadata_type met, submodule_list subs) {
    PropertyType mod;

    // Check hash/memoization
    Hasher h(HashType::Hash128);
    mod.hash(h);
    auto hv = bphash::hash_to_string(h.finalize());
    // std::cout<< "hash" << hv <<std::endl;
    REQUIRE(corr_hashes[i] == hv);
    hv = bphash::hash_to_string(mod.memoize(2));
    // std::cout<< hv <<std::endl;
    REQUIRE(corr_memo[i] == hv);

    // Check accessors
    // REQUIRE(mod.parameters() == params);
    REQUIRE(mod.submodules() == subs);
    REQUIRE(mod.metadata() == met);

    // Check state
    REQUIRE(!mod.locked());
    REQUIRE(!mod.is_cached(2));
    REQUIRE(mod.not_ready().size() == 1);

    // Change submodule and check it went through
    mod.change_submodule("Prop1", prop3);
    subs["Prop1"] = prop3;
    REQUIRE(mod.submodules() == subs);

    // Repeat for parameters

    // Lock the module and check that we can't change state
    mod.lock();
    REQUIRE(mod.locked());
    REQUIRE_THROWS_AS(mod.change_submodule("Prop1", prop1), std::runtime_error);
    // check parameters

    // Check we can run it
    REQUIRE(*mod.template run_as<PropertyType>(2) == 3);
}

TEST_CASE("ModuleBase Typedefs") {
    using correct_pointer = std::shared_ptr<ModuleBase>;
    using correct_meta    = std::map<MetaProperty, std::string>;
    using correct_list    = Utilities::CaseInsensitiveMap<module_pointer>;
    using correct_return =
      std::vector<std::pair<module_pointer, ModuleProperty>>;

    REQUIRE(std::is_same<correct_pointer, module_pointer>::value);
    REQUIRE(std::is_same<correct_meta, metadata_type>::value);
    REQUIRE(std::is_same<correct_list, submodule_list>::value);
    REQUIRE(std::is_same<correct_return, not_ready_return>::value);
}

TEST_CASE("ModuleBase") {
    metadata_type md_empty{};
    submodule_list sm_empty{};

    SECTION("int run(int)") {
        metadata_type md_full{{MetaProperty::name, "Prop1"}};
        submodule_list sm_full{{"Prop1", nullptr}};
        test_module<0, MyProp1>(md_full, sm_full);
    }

    SECTION("int run(int)") {
        submodule_list sm_full{{"Prop1", prop1}};
        test_module<1, MyProp2>(md_empty, sm_full);
    }
}
