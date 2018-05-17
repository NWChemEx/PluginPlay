#include <SDE/PropertyBase.hpp>
#include <array>
#include <catch/catch.hpp>

// We use PropertyBase header only b/c that has the macro for declaring propery
// module pairs

using namespace SDE;
using module_pointer = typename ModuleBase::module_pointer;
using meta_data_type = typename ModuleBase::meta_data_type;
using submodule_list = typename ModuleBase::submodule_list;

// Declare some Module types
DEFINE_PROPERTY(TestProperty1, void, );
DEFINE_PROPERTY(TestProperty2, int, int);
DEFINE_PROPERTY(TestProperty3, int, int&);

// Implement those types
struct MyProp1 : TestProperty1API {
    void run() {}
};
struct MyProp2 : TestProperty2API {
    MyProp2() { meta_data_[MetaProperty::name] = "Prop2"; }
    int run(int x) { return x + 1; }
};

static const auto prop2 = std::make_shared<MyProp2>();

struct MyProp3 : TestProperty3API {
    MyProp3() { submodules_["Prop2"] = prop2; }
    int run(int& x) { return x += 1; }
};

static const std::array<std::string, 3> corr_hashes = {
  "55f9d4398adfa39870952c434dd90bf6", "50770f7844350ad2e59d810ab1d61f06",
  "55b33b79a5850d2c5f87f6a66c6d5441"};

template<std::size_t i, typename mod_type, typename other_type>
void test_module(meta_data_type met, submodule_list subs) {
    mod_type mod;
    Hasher h(HashType::Hash128);
    mod.hash(h);
    auto hv = bphash::hash_to_string(h.finalize());
    REQUIRE(corr_hashes[i] == hv);
    REQUIRE(!mod.locked());
    REQUIRE(mod.submodules() == subs);
    REQUIRE(mod.meta_data() == met);
    auto new_ptr = std::make_shared<MyProp3>();
    for(auto x : subs) {
        mod.change_submodule(x.first, new_ptr);
        subs[x.first] = new_ptr;
    }
    REQUIRE(mod.submodules() == subs);
    mod.lock();
    REQUIRE(mod.locked());
    for(auto x : subs) {
        REQUIRE_THROWS_AS(mod.change_submodule(x.first, new_ptr),
                          std::runtime_error);
    }
}

TEST_CASE("ModuleBase Typedefs") {
    using correct_pointer = std::shared_ptr<ModuleBase>;
    using correct_meta    = std::map<MetaProperty, std::string>;
    using correct_list    = Utilities::CaseInsensitiveMap<module_pointer>;

    REQUIRE(std::is_same<correct_pointer, module_pointer>::value);
    REQUIRE(std::is_same<correct_meta, meta_data_type>::value);
    REQUIRE(std::is_same<correct_list, submodule_list>::value);
}

TEST_CASE("ModuleBase") {
    meta_data_type md_empty{};
    submodule_list sm_empty{};

    SECTION("void run()") {
        test_module<0, MyProp1, TestProperty1API>(md_empty, sm_empty);
    }

    SECTION("int run(int)") {
        meta_data_type md_full{{MetaProperty::name, "Prop2"}};
        test_module<1, MyProp2, TestProperty2API>(md_full, sm_empty);
    }

    SECTION("int run(int&)") {
        submodule_list sm_full{{"Prop2", prop2}};
        test_module<2, MyProp3, TestProperty3API>(md_empty, sm_full);
    }
}
