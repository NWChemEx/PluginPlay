#include <SDE/Module.hpp>
#include <catch/catch.hpp>

using namespace SDE;
using module_ptr = typename ModuleBase::module_pointer;

// Declare some Module types
DEFINE_PROPERTY(TestProperty1, void, );
DEFINE_PROPERTY(TestProperty2, int, int);
DEFINE_PROPERTY(TestProperty3, int*, int&);

// Implement those types
struct MyProp1 : TestProperty1API {
    void run() {}
};
struct MyProp2 : TestProperty2API {
    int run(int x) { return x + 1; }
};
struct MyProp3 : TestProperty3API {
    int* run(int& x) { return &x; }
};

template<typename mod_type, typename corr_type>
void test_module() {
    mod_type mod;
    REQUIRE(mod.type() == typeid(corr_type));
    REQUIRE(mod.submodules().empty());
    // Check hash
}

TEST_CASE("ModuleBaseImpl and ModuleBase") {
    SECTION("void run()") { test_module<MyProp1, TestProperty1API>(); }

    SECTION("int run(int)") { test_module<MyProp2, TestProperty2API>(); }

    SECTION("int* run(int&)") { test_module<MyProp3, TestProperty3API>(); }
}

// Simulates getting module from ModuleManager
module_ptr mm_facade(const std::string& str) {
    if(str == "Prop1")
        return std::make_shared<MyProp1>();
    else if(str == "Prop2")
        return std::make_shared<MyProp2>();
    else if(str == "Prop3")
        return std::make_shared<MyProp3>();
    return {};
}

TEST_CASE("PropertyBase") {
    // SECTION("void run()"){
    // TestProperty1 prop(mm_facade("Prop1"));
    // auto rv = prop();
    //}

    SECTION("int run(int)") {
        TestProperty2 prop(mm_facade("Prop2"));
        auto rv = prop(2);
        REQUIRE(*rv == 3);
    }

    SECTION("int* run(int&)") {
        TestProperty3 prop(mm_facade("Prop3"));
        int two = 2;
        auto rv = prop(two);
        REQUIRE(*rv == &two);
    }

    SECTION("Ctor error checking") {
        REQUIRE_THROWS_AS(TestProperty1(mm_facade("Prop2")), std::bad_cast);
    }
}
