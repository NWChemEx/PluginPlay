#include <SDE/Module.hpp>
#include <catch/catch.hpp>

using namespace SDE;

// Dummy module to test modules taking no arguments
struct ModuleA : public ModuleBaseImpl<ModuleA, bool> {
    bool run_() override { return true; }
};

SDE_NEW_MODULE_TYPE(ModuleBBase, int, int);

struct ModuleB : ModuleBBase {
    int run_(int x) override { return x; }
};

// Dummy module to test modules taking references
struct ModuleC : public ModuleBaseImpl<ModuleC, bool, double&, double*> {
    bool run_(double& d, double* p) override { return &d == p; }
};

template<typename ModType, typename ReturnType, typename... Args>
void test_module(ModType& mod, ReturnType corr_result, Args&&... args) {
    // Check typedefs
    REQUIRE(std::is_same<std::shared_ptr<const ReturnType>,
                         typename ModType::shared_return>::value);
    REQUIRE(std::is_same<ReturnType, typename ModType::return_type>::value);
    REQUIRE(typeid(ModType) == mod.type());
    auto result = mod(std::forward<Args>(args)...);
    REQUIRE(corr_result == *result);
}

TEST_CASE("ModuleImpl") {
    SECTION("bool run_(void)") {
        ModuleA mod;
        test_module(mod, true);
    }

    SECTION("int run_(int)") {
        ModuleB mod;
        test_module(static_cast<ModuleBBase&>(mod), 3, 3);
    }

    SECTION("bool run_(double&, double*)") {
        ModuleC mod;
        double pi{3.14};
        test_module(mod, true, pi, &pi);
    }
}

template<typename T, typename return_type, typename... Args>
void test_property(return_type rv, Args&&... args) {
    // Typedefs to shorten the next two checks
    using input_t = PropertyBase<T>;
    using ModPtr  = typename input_t::ModuleBasePtr;

    REQUIRE(std::is_same<std::unique_ptr<ModuleBase>, ModPtr>::value);
    REQUIRE(std::is_same<return_type, typename input_t::return_type>::value);

    input_t prop(std::make_unique<T>());
    REQUIRE(*prop(std::forward<Args>(args)...) == rv);
}

TEST_CASE("PropertyImpl") {
    SECTION("bool run_(void)") { test_property<ModuleA>(true); }

    SECTION("bool run_(double&, double*)") {
        double pi{3.14};
        test_property<ModuleC>(true, pi, &pi);
    }
}
