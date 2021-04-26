#include <catch2/catch.hpp>
#include <sde/detail_/sde_any_wrapper.hpp>
#include <sstream>

using namespace sde::detail_;
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
        auto h  = sde::make_hasher();
        auto h2 = sde::make_hasher();
        h(corr);
        h2(w);
        auto corr_hv = bphash::hash_to_string(h.finalize());
        auto hv      = bphash::hash_to_string(h2.finalize());
        REQUIRE(corr_hv == hv);
    }
}

TEST_CASE("SDEAnyWrapper<POD>(value)") {
    using three_type = decltype(3);
    using any_type   = SDEAnyWrapper<three_type>;
    SDEAnyWrapper w(3);
    STATIC_REQUIRE(std::is_same_v<decltype(w), any_type>);
    compare_value<three_type>(w, 3);

    SECTION("Serialize via base class") {
        std::stringstream ss;

        SDEAnyWrapperBase* pw = &w;
        {
            typename Serializer::binary_archive ar(ss);
            Serializer s(ar);
            pw->serialize(s);
        }

        std::unique_ptr<SDEAnyWrapperBase> pw2;
        {
            typename Deserializer::binary_archive ar(ss);
            Deserializer d(ar);
            auto temp = SDEAnyWrapperBase::deserialize(d);
            pw2.swap(temp);
        }
        REQUIRE(*pw == *pw2);
    }
}

TEST_CASE("SDEAnyWrapper<POD>(reference") {
    int x = 3;
    SDEAnyWrapper w(x);
    STATIC_REQUIRE(std::is_same_v<decltype(w), SDEAnyWrapper<int>>);
    compare_value<int>(w, 3);
}

TEST_CASE("SDEAnyWrapper<POD>(const reference") {
    const int x = 3;
    SDEAnyWrapper w(x);
    STATIC_REQUIRE(std::is_same_v<decltype(w), SDEAnyWrapper<const int>>);
    compare_value<const int>(w, 3);
}

TEST_CASE("SDEAnyWrapper<non-POD>(move)") {
    using vector_t = std::vector<double>;
    vector_t v{1.1, 2.2, 3.3};
    vector_t v2(v);
    const double* pv = v.data();

    SDEAnyWrapper w(std::move(v));
    STATIC_REQUIRE(std::is_same_v<decltype(w), SDEAnyWrapper<vector_t>>);
    compare_value<vector_t>(w, v2);

    REQUIRE(w.cast<vector_t&>().data() == pv);

    SECTION("Serialize via base class") {
        std::stringstream ss;

        SDEAnyWrapperBase* pw = &w;
        {
            typename Serializer::binary_archive ar(ss);
            Serializer s(ar);
            pw->serialize(s);
        }

        std::unique_ptr<SDEAnyWrapperBase> pw2;
        {
            typename Deserializer::binary_archive ar(ss);
            Deserializer d(ar);
            auto temp = SDEAnyWrapperBase::deserialize(d);
            pw2.swap(temp);
        }
        REQUIRE(*pw == *pw2);
    }
}

TEST_CASE("SDEAnyWrapper Comparisons") {
    SDEAnyWrapper w(3);

    SECTION("Identical") {
        SDEAnyWrapper w2(3);
        REQUIRE(w == w2);
        REQUIRE_FALSE(w != w2);
    }

    SECTION("Different value") {
        SDEAnyWrapper w2(4);
        REQUIRE(w != w2);
        REQUIRE_FALSE(w == w2);
    }

    SECTION("Different type") {
        SDEAnyWrapper w2(1.234);
        REQUIRE(w != w2);
        REQUIRE_FALSE(w == w2);
    }

    SECTION("Different const-ness") {
        const int x = 3;
        SDEAnyWrapper w2(x);
        REQUIRE(w == w2);
        REQUIRE_FALSE(w != w2);
    }
}

struct NotPrintable {
    void hash(sde::Hasher&) const noexcept {}

    bool operator==(const NotPrintable&) const noexcept { return true; }
};

TEST_CASE("SDEAnyWrapper : str") {
    SECTION("printable type") {
        SDEAnyWrapper w(int{3});
        REQUIRE(w.str() == "3");
    }
    SECTION("non-printable") {
        SDEAnyWrapper w(NotPrintable{});
        auto* p = &w.cast<NotPrintable&>();
        std::stringstream ss;
        ss << "<" << typeid(NotPrintable).name() << " " << p << ">";
        REQUIRE(w.str() == ss.str());
    }
}

TEST_CASE("SDEAnyWrapper : cast") {
    SECTION("non-const wrapper") {
        SDEAnyWrapper w(int{3});
        REQUIRE(w.cast<int>() == 3);
        REQUIRE(w.cast<int&>() == 3);
        REQUIRE(w.cast<const int&>() == 3);
        REQUIRE_THROWS_AS(w.cast<double>(), std::bad_any_cast);
    }
    SECTION("const wrapper") {
        const SDEAnyWrapper w(int{3});
        REQUIRE(w.cast<int>() == 3);
        // The following line should trip a static assert
        // w.cast<int&>()
        REQUIRE(w.cast<const int&>() == 3);
        REQUIRE_THROWS_AS(w.cast<double>(), std::bad_any_cast);
    }
}
