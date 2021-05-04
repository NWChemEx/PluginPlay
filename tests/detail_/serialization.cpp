#include "sde/detail_/serialization.hpp"
#include <catch2/catch.hpp>

using namespace sde::detail_;

TEMPLATE_TEST_CASE(
  "Serializer", "[serialization][serializer][deserializer]",
  (std::pair<Serializer::binary_archive, Deserializer::binary_archive>),
  (std::pair<Serializer::json_archive, Deserializer::json_archive>),
  (std::pair<Serializer::xml_archive, Deserializer::xml_archive>)) {
    std::stringstream ss;

    SECTION("Plain-old-data") {
        {
            typename TestType::first_type ar(ss);
            Serializer s(ar);
            s << int{42} << double{3.14} << char{'R'};
        }

        int i;
        double d;
        char c;

        {
            typename TestType::second_type ar(ss);
            Deserializer ds(ar);
            ds >> i >> d >> c;
        }
        REQUIRE(i == int{42});
        REQUIRE(d == double{3.14});
        REQUIRE(c == char{'R'});
    }

    SECTION("Containers") {
        std::vector<int> v{1, 2, 3, 4};
        std::map<std::string, double> m{{"Hello", 1.23}, {"World", 3.14}};

        {
            typename TestType::first_type ar(ss);
            Serializer s(ar);
            s << v << m;
        }

        std::vector<int> v2;
        std::map<std::string, double> m2;
        {
            typename TestType::second_type ar(ss);
            Deserializer d(ar);
            d >> v2 >> m2;
        }
        REQUIRE(v2 == v);
        REQUIRE(m2 == m);
    }
}