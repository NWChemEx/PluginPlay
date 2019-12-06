#include <catch2/catch.hpp>
#include <memory>
#include <sde/utility.hpp>

using namespace sde;

TEST_CASE("IsCString") {
    STATIC_REQUIRE_FALSE(detail_::IsCString<std::string>::value);
    STATIC_REQUIRE_FALSE(detail_::IsCString<int>::value);
    STATIC_REQUIRE(detail_::IsCString<decltype("a string literal")>::value);
}

TEST_CASE("IsConstRef") {
    STATIC_REQUIRE_FALSE(detail_::IsConstRef<int>::value);
    STATIC_REQUIRE_FALSE(detail_::IsConstRef<int&>::value);
    STATIC_REQUIRE_FALSE(detail_::IsConstRef<const int>::value);
    STATIC_REQUIRE(detail_::IsConstRef<const int&>::value);
}

TEST_CASE("IsSharedPtr") {
    STATIC_REQUIRE_FALSE(detail_::IsSharedPtr<int>::value);
    STATIC_REQUIRE_FALSE(detail_::IsSharedPtr<int*>::value);
    STATIC_REQUIRE(detail_::IsSharedPtr<std::shared_ptr<int>>::value);
}

TEST_CASE("Reference wrapper equality") {
    int ione = 1, itwo = 2;
    double done = 1.0, dtwo = 2.0;
    REQUIRE(detail_::operator==(std::cref(ione), std::cref(ione)));
    REQUIRE(!detail_::operator==(std::cref(ione), std::cref(itwo)));
    REQUIRE(detail_::operator==(std::cref(ione), std::cref(done)));
    REQUIRE(!detail_::operator==(std::cref(ione), std::cref(dtwo)));
}
