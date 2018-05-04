#include<SDE/Module.hpp>
#include<catch/catch.hpp>

using namespace SDE;

// Dummy module to test modules taking no arguments
struct ModuleA : public ModuleImpl<ModuleA, bool> {
    bool run_() override {return true;}
};

struct ModuleC : public ModuleImpl<ModuleC, bool, const double&, const double*> {
    bool run_(const double& d, const double* p) override {
        return &d == p;
    }
};


template<typename ModType, typename ReturnType, typename...Args>
void test_module(ModType& mod, ReturnType corr_result, Args&&...args){
    // Check typedefs
    REQUIRE(std::is_same<std::shared_ptr<const ReturnType>, typename
      ModType::shared_return>::value);
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

    SECTION("bool run_(const double&, double*)") {
        ModuleC mod;
        double pi{3.14};
        test_module(mod, true, pi, &pi);
    }
}
