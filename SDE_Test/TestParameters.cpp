#include <SDE/Parameters.hpp>
#include <catch/catch.hpp>

using namespace SDE;

TEST_CASE("Range Checks") {
    GreaterThan<int> gt(3);
    REQUIRE(gt(detail_::SDEAny(4)) == true);
    REQUIRE(gt(detail_::SDEAny(2)) == false);

    LessThan<int> lt(3);
    REQUIRE(lt(detail_::SDEAny(2)) == true);
    REQUIRE(lt(detail_::SDEAny(4)) == false);

    Between<int> bt(3, 5);
    REQUIRE(bt(detail_::SDEAny(4)) == true);
    REQUIRE(bt(detail_::SDEAny(2)) == false);
    REQUIRE(bt(detail_::SDEAny(6)) == false);
}


TEST_CASE("Options and Parameters") {

    SECTION("Options") {
        Option opt{4, "Any positive number", {GreaterThan<int>{-1}}};

        REQUIRE(detail_::SDEAnyCast<int>(opt.value) == 4);
        REQUIRE(opt.is_valid(5) == true);
        REQUIRE(opt.is_valid(-1) == false);

        Hasher h(HashType::Hash128);
        opt.hash(h);
        auto hv = bphash::hash_to_string(h.finalize());
        REQUIRE(hv == "232a945684f65900dc9a486c25ca1ac0");

        REQUIRE_THROWS(
          Option{4, "Any negative number", {LessThan<int>{0}}});
        REQUIRE_NOTHROW(
          Option{-1, "Any negative number", {LessThan<int>{0}}});
    }

    SECTION("Parameters") {

        Parameters params;

        Option opt(3,"some description");
        // insert() and at()
        params.insert("The number 3", opt);
        REQUIRE(params.at<int>("The number 3") == 3);
        // Using at<Option<T>>()
        //REQUIRE(params.at<Option>("The number 3").value == 3);
        // change()
        params.change("The number 3", 2);
        REQUIRE(params.at<int>("The number 3") == 2);

        // checking range_checks and change()
        params.insert(
          "Not a negative number",
          Option{4, "Any positive number", {GreaterThan<int>{-1}}});
        REQUIRE_THROWS(params.change("Not a negative number", -1));
        REQUIRE_NOTHROW(params.change("Not a negative number", 6));

        // count()
        REQUIRE(params.count("Hello World") == false);
        params.insert("Hello World", Option{"Hello world"});
        REQUIRE(params.count("Hello World") == true);

        Hasher h(HashType::Hash128);
        params.hash(h);
        auto hv = bphash::hash_to_string(h.finalize());
        REQUIRE(hv == "dbd8f8e5e3f56072ed120dec2214a8e5");

        // This insertion should not change the hash value
        Hasher h2(HashType::Hash128);
        params.insert(
          "Hash blind",
          Option{2, "Transparent thing", {}, {OptionTraits::transparent}});
        params.hash(h2);
        hv = bphash::hash_to_string(h2.finalize());
        REQUIRE(hv == "dbd8f8e5e3f56072ed120dec2214a8e5");

        // This insertion should change the hash (i.e. the Option is not
        // transparent)
        Hasher h3(HashType::Hash128);
        params.insert("Hash modifying", Option{2, "Opaque thing", {}, {}});
        params.hash(h3);
        hv = bphash::hash_to_string(h2.finalize());
        REQUIRE(hv != "dbd8f8e5e3f56072ed120dec2214a8e5");
        REQUIRE(hv == "8e1577928ba49f2dadbc0a606d51e1f5");
    }
}
