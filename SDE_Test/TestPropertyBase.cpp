#include <SDE/PropertyBase.hpp>
#include <catch/catch.hpp>

using namespace SDE;
using module_ptr = typename ModuleBase::module_pointer;

// Declare some Module types
DEFINE_PROPERTY(TestProperty1, void, );
DEFINE_PROPERTY(TestProperty2, int, int);
DEFINE_PROPERTY(TestProperty3, int, int&);

// Implement those types
struct MyProp1 : TestProperty1API {
    void run() {}
};
struct MyProp2 : TestProperty2API {
    int run(int x) { return x + 1; }
};
struct MyProp3 : TestProperty3API {
    int run(int& x) { return x += 1; }
};

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

static std::map<std::string, std::string> memo_vals{
  {"Prop2", "7340346a82e8f5d165a016ba4a4acd74"},
  {"Prop3", "1b406e0514b9c5a9ce0caee34a167205"}};

template<typename PropType, typename ModuleAPI, typename corr_return,
         typename input_t>
PropType test_property(std::string propn, input_t inp) {
    using module_type    = typename PropType::module_type;
    using return_type    = typename PropType::return_type;
    using shared_return  = typename PropType::shared_return;
    using args_type      = typename PropType::args_type;
    using module_pointer = typename PropType::module_pointer;
    using cost_type      = typename PropType::cost_type;

    using corr_shared = std::shared_ptr<const corr_return>;
    REQUIRE(std::is_same<ModuleAPI, module_type>::value);
    REQUIRE(std::is_same<corr_return, return_type>::value);
    REQUIRE(std::is_same<corr_shared, shared_return>::value);
    REQUIRE(std::is_same<input_t, args_type>::value);
    REQUIRE(std::is_same<module_ptr, module_pointer>::value);
    REQUIRE(std::is_same<std::size_t, cost_type>::value);

    auto mod  = mm_facade(propn);
    auto prop = PropType(mod);

    constexpr auto max_n = std::numeric_limits<cost_type>::max();
    auto& val            = std::get<0>(inp);
    REQUIRE(prop.cost(Resource::time, val) == max_n);
    REQUIRE(prop.cost(Resource::memory, val) == max_n);
    REQUIRE(prop.cost(Resource::disk, val) == max_n);
    REQUIRE(prop.cost(Resource::processes, val) == max_n);
    REQUIRE(prop.cost(Resource::threads, val) == max_n);
    auto hv = bphash::hash_to_string(prop.memoize(val));
    REQUIRE(hv == memo_vals.at(propn));

    return prop;
}

TEST_CASE("PropertyBase") {
    // SECTION("void run()"){
    // TestProperty1 prop(mm_facade("Prop1"));
    // auto rv = prop();
    //}

    SECTION("int run(int)") {
        auto prop =
          test_property<TestProperty2, TestProperty2API, int, std::tuple<int>>(
            "Prop2", std::make_tuple(3));
        REQUIRE(*prop(2) == 3);
    }

    SECTION("int* run(int&)") {
        int two = 2;
        std::tuple<int&> two_tuple(two);
        auto prop =
          test_property<TestProperty3, TestProperty3API, int, std::tuple<int&>>(
            "Prop3", two_tuple);
        REQUIRE(*prop(two) == 3);
        REQUIRE(two == 3);
    }

    SECTION("Ctor error checking") {
        REQUIRE_THROWS_AS(TestProperty1(mm_facade("Prop2")), std::bad_cast);
    }
}
