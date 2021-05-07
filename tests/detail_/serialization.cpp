#include "sde/detail_/archive_wrapper.hpp"
#include <catch2/catch.hpp>

using namespace sde::detail_;

TEMPLATE_TEST_CASE("Serializer", "[serialization][serializer][deserializer]",
                   cereal::BinaryOutputArchive, cereal::JSONOutputArchive,
                   cereal::XMLOutputArchive) {
    using output = TestType;
    using input  = typename sde::get_input_from_output<output>::type;

    std::stringstream ss;

    SECTION("Plain-old-data") {
        {
            output ar(ss);
            Serializer s(ar);
            s(int{42})(double{3.14})(char{'R'});
        }

        int i;
        double d;
        char c;

        {
            input ar(ss);
            Deserializer ds(ar);
            ds(i)(d)(c);
        }
        REQUIRE(i == int{42});
        REQUIRE(d == double{3.14});
        REQUIRE_FALSE(d == float{3.14f});
        REQUIRE(c == char{'R'});
    }

    SECTION("Containers") {
        std::vector<int> v{1, 2, 3, 4};
        std::map<std::string, double> m{{"Hello", 1.23}, {"World", 3.14}};

        {
            output ar(ss);
            Serializer s(ar);
            s(v)(m);
        }

        std::vector<int> v2;
        std::map<std::string, double> m2;
        {
            input ar(ss);
            Deserializer d(ar);
            d(v2)(m2);
        }
        REQUIRE(v2 == v);
        REQUIRE(m2 == m);
    }
}