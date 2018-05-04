#include<SDE/Module.hpp>
#include<catch/catch.hpp>

using namespace SDE;
using namespace detail_;

// Structs for testing my meta-programming structs
struct A {bool run_(){return true;}};
struct B {std::vector<double> run_(int) { return {};}};
struct C {
    int x;
    int & run_(const int&, int) {return x;}
};

TEST_CASE("ExtractReturnType") {

    using type1 = ExtractReturnType<A>;
    REQUIRE(std::is_same<bool, typename type1::type>::value);

    using type2 = ExtractReturnType<B>;
    REQUIRE(std::is_same<std::vector<double>, typename type2::type>::value);

    using type3 = ExtractReturnType<C>;
    REQUIRE(std::is_same<int &, typename type3::type>::value);

    using type4 = ExtractArgTypes<A>;
    REQUIRE(std::is_same<std::tuple<>, typename type4::type>::value);

    using type5 = ExtractArgTypes<B>;
    REQUIRE(std::is_same<std::tuple<int>, typename type5::type>::value);

    using type6 = ExtractArgTypes<C>;
    REQUIRE(std::is_same<std::tuple<const int&,int>, typename
      type6::type>::value);

}
