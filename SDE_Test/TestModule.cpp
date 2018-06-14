#include <SDE/Module.hpp>
#include <array>
#include <catch/catch.hpp>

using namespace SDE;
using module_pointer   = typename ModuleBase::module_pointer;
using metadata_type    = typename ModuleBase::metadata_type;
using submodule_list   = typename ModuleBase::submodule_list;
using not_ready_return = typename ModuleBase::not_ready_return;

// Declare some Module types
DEFINE_MODULE_TYPE(TestProperty1, void, );
DEFINE_MODULE_TYPE(TestProperty2, int, int);
DEFINE_MODULE_TYPE(TestProperty3, int, int);

// Implement those types
struct MyProp1 : TestProperty1 {
    void run() {}
};
struct MyProp2 : TestProperty2 {
    MyProp2() {
        submodules_["Prop1"]          = nullptr;
        metadata_[MetaProperty::name] = "Prop2";
    }
    int run(int x) { return x + 1; }
};

static const auto prop2 = std::make_shared<MyProp2>();

struct MyProp3 : TestProperty3 {
    MyProp3() { submodules_["Prop2"] = prop2; }
    int run(int x) { return x - 1; }
};

static const std::array<std::string, 3> corr_hashes = {
  "55f9d4398adfa39870952c434dd90bf6", "8934a350b19eaff22fb70161c3fb9b4f",
  "535a7785c396caa2c440b05f30a7a1f0"};

template<std::size_t i, typename mod_type, typename other_type>
void test_module(metadata_type met, submodule_list subs /*,Parameters params*/
) {
    mod_type mod;
    Hasher h(HashType::Hash128);
    mod.hash(h);
    auto hv = bphash::hash_to_string(h.finalize());
    REQUIRE(corr_hashes[i] == hv);
    REQUIRE(!mod.locked());
    REQUIRE(mod.submodules() == subs);
    REQUIRE(mod.metadata() == met);
    // REQUIRE(mod.parameters() == params);
    auto new_ptr = std::make_shared<MyProp3>();
    for(auto x : subs) {
        mod.change_submodule(x.first, new_ptr);
        subs[x.first] = new_ptr;
    }
    REQUIRE(mod.submodules() == subs);

    /*
    for(auto x : subs) {
        mod.change_parameter(x.first, <value>);
        params.change(x.first, <value>);
     }
     REQUIRE(mod.parameters() == params);
     */
    mod.lock();
    REQUIRE(mod.locked());
    for(auto x : subs) {
        REQUIRE_THROWS_AS(mod.change_submodule(x.first, new_ptr),
                          std::runtime_error);
    }

    /*
    for(auto x : subs)
    REQUIRE_THROWS_AS(mod.change_parameter(x.first, <value>),
                      std::runtime_error);
    */
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

    SECTION("void run()") {
        test_module<0, MyProp1, TestProperty1>(md_empty, sm_empty);
    }

    SECTION("int run(int)") {
        metadata_type md_full{{MetaProperty::name, "Prop2"}};
        submodule_list sm_full{{"Prop1", nullptr}};
        test_module<1, MyProp2, TestProperty2>(md_full, sm_full);
    }

    SECTION("int run(int&)") {
        submodule_list sm_full{{"Prop2", prop2}};
        test_module<2, MyProp3, TestProperty3>(md_empty, sm_full);
    }

    SECTION("not ready") {
        MyProp3 prop3;
        auto r1 = prop3.not_ready();
        REQUIRE(r1 == not_ready_return{{prop2, ModuleProperty::submodules}});

        auto r2 = prop2->not_ready();
        REQUIRE(r2 == not_ready_return{{nullptr, ModuleProperty::submodules}});
    }
}
