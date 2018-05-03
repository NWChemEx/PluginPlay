#include<SDE/Module.hpp>
#include<catch/catch.hpp>

using namespace SDE;
using namespace detail_;

struct A {
    bool operator()(){return true;}
};

struct B {
    std::vector<double> operator()(int) { return {};}
};

TEST_CASE("ExtractReturnType") {

    using type1 = ExtractReturnType<A>;
    REQUIRE(std::is_same<bool, typename type1::type>::value);

    using type2 = ExtractReturnType<B>;
    REQUIRE(std::is_same<std::vector<double>, typename type2::type>::value);

}
