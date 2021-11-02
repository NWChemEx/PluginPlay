#include "pluginplay/detail_/any_input_wrapper.hpp"
#include <catch2/catch.hpp>
#include <sstream>

using namespace pluginplay::detail_;
using utilities::printing::operator<<;

template<typename corr_t, typename T>
inline static void compare_value(T&& w, corr_t corr) {
    SECTION("Type") { REQUIRE(w.type() == typeid(corr_t)); }

    SECTION("get by reference") {
        auto& value = w.template cast<corr_t&>();
        REQUIRE(value == corr);
    }

    SECTION("get by const reference") {
        auto& value = w.template cast<const corr_t&>();
        REQUIRE(value == corr);
    }

    SECTION("get by value") {
        auto value = w.template cast<corr_t>();
        REQUIRE(value == corr);
    }

    SECTION("Throws if wrong type") {
        REQUIRE_THROWS_AS(w.template cast<std::string>(), std::bad_any_cast);
    }

    SECTION("clone") {
        auto w2 = w.clone();
        REQUIRE(w == *w2);
    }

    SECTION("str") {
        std::stringstream ss;
        using utilities::printing::operator<<;
        ss << corr;
        REQUIRE(ss.str() == w.str());
    }

    SECTION("hashing") {
        REQUIRE(pluginplay::hash_objects(corr) == pluginplay::hash_objects(w));
    }
}

TEST_CASE("AnyInputWrapper<POD>(value)") {
    using three_type = decltype(3);
    AnyInputWrapper w(3);
    STATIC_REQUIRE(std::is_same_v<decltype(w), AnyInputWrapper<three_type>>);
    compare_value<three_type>(w, 3);
}

TEST_CASE("AnyInputWrapper<POD>(reference") {
    int x = 3;
    AnyInputWrapper w(x);
    STATIC_REQUIRE(std::is_same_v<decltype(w), AnyInputWrapper<int>>);
    compare_value<int>(w, 3);
}

TEST_CASE("AnyInputWrapper<POD>(const reference") {
    const int x = 3;
    AnyInputWrapper w(x);
    STATIC_REQUIRE(std::is_same_v<decltype(w), AnyInputWrapper<const int>>);
    compare_value<const int>(w, 3);
}

TEST_CASE("AnyInputWrapper<non-POD>(move)") {
    using vector_t = std::vector<double>;
    vector_t v{1.1, 2.2, 3.3};
    vector_t v2(v);
    const double* pv = v.data();

    AnyInputWrapper w(std::move(v));
    STATIC_REQUIRE(std::is_same_v<decltype(w), AnyInputWrapper<vector_t>>);
    compare_value<vector_t>(w, v2);

    REQUIRE(w.cast<vector_t&>().data() == pv);
}

TEST_CASE("AnyInputWrapper Comparisons") {
    AnyInputWrapper w(3);

    SECTION("Identical") {
        AnyInputWrapper w2(3);
        REQUIRE(w == w2);
        REQUIRE_FALSE(w != w2);
    }

    SECTION("Different value") {
        AnyInputWrapper w2(4);
        REQUIRE(w != w2);
        REQUIRE_FALSE(w == w2);
    }

    SECTION("Different type") {
        AnyInputWrapper w2(1.234);
        REQUIRE(w != w2);
        REQUIRE_FALSE(w == w2);
    }

    SECTION("Different const-ness") {
        const int x = 3;
        AnyInputWrapper w2(x);
        REQUIRE(w == w2);
        REQUIRE_FALSE(w != w2);
    }
}

struct NotPrintable {
    void hash(pluginplay::Hasher&) const noexcept {}

    bool operator==(const NotPrintable&) const noexcept { return true; }
};

TEST_CASE("AnyInputWrapper : str") {
    SECTION("printable type") {
        AnyInputWrapper w(int{3});
        REQUIRE(w.str() == "3");
    }
    SECTION("non-printable") {
        AnyInputWrapper w(NotPrintable{});
        auto* p = &w.cast<NotPrintable&>();
        std::stringstream ss;
        ss << "<" << typeid(NotPrintable).name() << " " << p << ">";
        REQUIRE(w.str() == ss.str());
    }
}

TEST_CASE("AnyInputWrapper : cast") {
    SECTION("non-const wrapper") {
        AnyInputWrapper w(int{3});
        REQUIRE(w.cast<int>() == 3);
        REQUIRE(w.cast<int&>() == 3);
        REQUIRE(w.cast<const int&>() == 3);
        REQUIRE_THROWS_AS(w.cast<double>(), std::bad_any_cast);
    }
    SECTION("const wrapper") {
        const AnyInputWrapper w(int{3});
        REQUIRE(w.cast<int>() == 3);
        // The following line should trip a static assert
        // w.cast<int&>()
        REQUIRE(w.cast<const int&>() == 3);
        REQUIRE_THROWS_AS(w.cast<double>(), std::bad_any_cast);
    }
}
