/* This file unit tests the FieldTupleTraits class.
 *
 * The FieldTupleTraits class is intended to be used to deduce the types for the
 * FieldTuple class. Hence we need to test it for:
 *
 * - FieldTuple<ModuleInput, ...>
 * - FieldTuple<ModuleResult, ...>
 *
 * where the ellipses can be 0 or more types. For ModuleInput we need to make
 * sure we test cv-qualified reference types (ModuleResults are unqualified).
 */

#include <catch2/catch.hpp>
#include <sde/detail_/field_tuple_traits.hpp>

// Forward declare field_type/ModuleResult so we can use them
namespace sde {
class MoculeInput;
class ModuleResult;
}

using namespace sde;
using namespace sde::detail_;

TEST_CASE("FieldTupleTraits<ModuleInput>"){
    using field_type = ModuleInput;
    using traits_type = FieldTupleTraits<field_type>;

    SECTION("new_field_tuple<int>"){
        using new_field_tuple = typename traits_type::new_field_tuple<int>;
        using corr = FieldTuple<field_type, int>;
        STATIC_REQUIRE(std::is_same_v<new_field_tuple, corr>);
    }

    SECTION("new_field_tuple<const int&>"){
        using new_field_tuple =
          typename traits_type::new_field_tuple<const int&>;
        using corr = FieldTuple<field_type, const int&>;
        STATIC_REQUIRE(std::is_same_v<new_field_tuple, corr>);
    }

    SECTION("nfields"){ STATIC_REQUIRE(traits_type::nfields == 0); }
    
    SECTION("tuple_of_fields"){
        using tuple_of_fields = typename traits_type::tuple_of_fields;
        using corr = std::tuple<>;
        STATIC_REQUIRE(std::is_same_v<tuple_of_fields, corr>);
    }
    
    SECTION("key_type"){
        using key_type = typename traits_type::key_type;
        using corr = sde::type::key;
        STATIC_REQUIRE(std::is_same_v<key_type, corr>);
    }

    SECTION("mapped_type"){
        using mapped_type = typename traits_type::mapped_type;
        using corr = field_type;
        STATIC_REQUIRE(std::is_same_v<mapped_type, corr>);
    }
    
    SECTION("value_type"){
        using value_type = typename traits_type::value_type;
        using corr = std::pair<sde::type::key, field_type>;
        STATIC_REQUIRE(std::is_same_v<value_type, corr>);
    }

    SECTION("field_array<0>"){
        using field_array = typename traits_type::field_array<0>;
        using corr = std::array<std::pair<type::key, field_type>, 0>;
        STATIC_REQUIRE(std::is_same_v<field_array, corr>);
    }

    SECTION("field_array<1>"){
        using field_array = typename traits_type::field_array<1>;
        using corr = std::array<std::pair<type::key, field_type>, 1>;
        STATIC_REQUIRE(std::is_same_v<field_array, corr>);
    }

    SECTION("field_array<2>"){
        using field_array = typename traits_type::field_array<2>;
        using corr = std::array<std::pair<type::key, field_type>, 2>;
        STATIC_REQUIRE(std::is_same_v<field_array, corr>);
    }

    SECTION("iterator"){
        using iterator = typename traits_type::iterator;
        using corr =
          typename std::array<std::pair<type::key, field_type>, 0>::iterator;
        STATIC_REQUIRE(std::is_same_v<iterator, corr>);
    }

    SECTION("const_iterator"){
        using const_iterator = typename traits_type::const_iterator;
        using corr =
          typename std::array<std::pair<type::key, field_type>, 0>::const_iterator;
        STATIC_REQUIRE(std::is_same_v<const_iterator, corr>);
    }
}

TEST_CASE("FieldTupleTraits<ModuleInput, int>"){
    using field_type = ModuleInput;
    using traits_type = FieldTupleTraits<field_type, int>;

    SECTION("new_field_tuple<int>"){
        using new_field_tuple = typename traits_type::new_field_tuple<int>;
        using corr = FieldTuple<field_type, int>;
        STATIC_REQUIRE(std::is_same_v<new_field_tuple, corr>);
    }

    SECTION("new_field_tuple<const int&>"){
        using new_field_tuple =
        typename traits_type::new_field_tuple<const int&>;
        using corr = FieldTuple<field_type, const int&>;
        STATIC_REQUIRE(std::is_same_v<new_field_tuple, corr>);
    }

    SECTION("nfields"){ STATIC_REQUIRE(traits_type::nfields == 0); }

    SECTION("tuple_of_fields"){
        using tuple_of_fields = typename traits_type::tuple_of_fields;
        using corr = std::tuple<>;
        STATIC_REQUIRE(std::is_same_v<tuple_of_fields, corr>);
    }

    SECTION("key_type"){
        using key_type = typename traits_type::key_type;
        using corr = sde::type::key;
        STATIC_REQUIRE(std::is_same_v<key_type, corr>);
    }

    SECTION("mapped_type"){
        using mapped_type = typename traits_type::mapped_type;
        using corr = field_type;
        STATIC_REQUIRE(std::is_same_v<mapped_type, corr>);
    }

    SECTION("value_type"){
        using value_type = typename traits_type::value_type;
        using corr = std::pair<sde::type::key, field_type>;
        STATIC_REQUIRE(std::is_same_v<value_type, corr>);
    }

    SECTION("field_array<0>"){
        using field_array = typename traits_type::field_array<0>;
        using corr = std::array<std::pair<type::key, field_type>, 0>;
        STATIC_REQUIRE(std::is_same_v<field_array, corr>);
    }

    SECTION("field_array<1>"){
        using field_array = typename traits_type::field_array<1>;
        using corr = std::array<std::pair<type::key, field_type>, 1>;
        STATIC_REQUIRE(std::is_same_v<field_array, corr>);
    }

    SECTION("field_array<2>"){
        using field_array = typename traits_type::field_array<2>;
        using corr = std::array<std::pair<type::key, field_type>, 2>;
        STATIC_REQUIRE(std::is_same_v<field_array, corr>);
    }

    SECTION("iterator"){
        using iterator = typename traits_type::iterator;
        using corr =
        typename std::array<std::pair<type::key, field_type>, 0>::iterator;
        STATIC_REQUIRE(std::is_same_v<iterator, corr>);
    }

    SECTION("const_iterator"){
        using const_iterator = typename traits_type::const_iterator;
        using corr =
        typename std::array<std::pair<type::key, field_type>, 0>::const_iterator;
        STATIC_REQUIRE(std::is_same_v<const_iterator, corr>);
    }
}