#include "sde/detail_/serialization.hpp"
#include <catch2/catch.hpp>

using namespace sde::detail_;

TEST_CASE("Serializer<BinaryArchive>") {
    std::stringstream ss;

    SECTION("Plain-old-data") {
        {
            typename Serializer::binary_archive ar(ss);
            Serializer::variant_archive var{&ar};

            Serializer s(var);
            s << int{42} << double{3.14} << char{'R'};
        }

        int i;
        double d;
        char c;

        {
            typename Deserializer::binary_archive ar(ss);
            Deserializer::variant_archive var{&ar};
            Deserializer ds(var);
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
            typename Serializer::binary_archive ar(ss);
            Serializer::variant_archive var{&ar};

            Serializer s(var);
            s << v << m;
        }

        std::vector<int> v2;
        std::map<std::string, double> m2;
        {
            typename Deserializer::binary_archive ar(ss);
            Deserializer::variant_archive var{&ar};

            Deserializer d(var);
            d >> v2 >> m2;
        }
        REQUIRE(v2 == v);
        REQUIRE(m2 == m);
    }
}