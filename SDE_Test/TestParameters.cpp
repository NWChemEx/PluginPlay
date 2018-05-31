#include "../SDE/Memoization.hpp"
#include <SDE/Parameters.hpp>
#include <catch/catch.hpp>

using namespace SDE;

TEST_CASE("Range Checks") {
    GreaterThan<int> gt(3);
    REQUIRE(gt(4) == true);
    REQUIRE(gt(2) == false);

    LessThan<int> lt(3);
    REQUIRE(lt(2) == true);
    REQUIRE(lt(4) == false);

    Between<int> bt(3, 5);
    REQUIRE(bt(4) == true);
    REQUIRE(bt(2) == false);
    REQUIRE(bt(6) == false);
}

TEST_CASE("Options and Parameters") {
    SECTION("Options") {
        Option<int> opt{4, "Any positive number", {GreaterThan<int>{-1}}};

        REQUIRE(opt.value == 4);
        REQUIRE(opt.is_valid(5) == true);
        REQUIRE(opt.is_valid(-1) == false);

        Hasher h(HashType::Hash128);
        opt.hash(h);
        auto hv = bphash::hash_to_string(h.finalize());
        REQUIRE(hv == "c506430fa7d74cb898940bdea794139a");

        REQUIRE_THROWS(
          Option<int>{4, "Any negative number", {LessThan<int>{0}}});
        REQUIRE_NOTHROW(
          Option<int>{-1, "Any negative number", {LessThan<int>{0}}});
    }

    SECTION("Parameters") {
        Parameters params;

        Option<int> opt(3);
        // insert() and at()
        params.insert("The number 3", opt);
        REQUIRE(params.at<int>("The number 3") == 3);
        // Using at<Option<T>>()
        REQUIRE(params.at<Option<int>>("The number 3").value == 3);
        // change()
        params.change("The number 3", 2);
        REQUIRE(params.at<int>("The number 3") == 2);

        params.insert(
          "Not a negative number",
          Option<int>{4, "Any positive number", {GreaterThan<int>{-1}}});
        REQUIRE_THROWS(params.change("Not a negative number", -1));
        REQUIRE_NOTHROW(params.change("Not a negative number", 6));

        REQUIRE(params.count("Hello World") == false);
        params.insert("Hello World", Option<std::string>{"Hello world"});
        REQUIRE(params.count("Hello World") == true);

        Hasher h(HashType::Hash128);
        params.hash(h);
        auto hv = bphash::hash_to_string(h.finalize());
        REQUIRE(hv == "9c559c4e96dc16f3bfbc40edd8ec76e8");

        // This insertion should not change the hash value
        Hasher h2(HashType::Hash128);
        params.insert(
          "Hash blind",
          Option<int>{2, "Transparent thing", {}, {OptionTraits::transparent}});
        params.hash(h2);
        hv = bphash::hash_to_string(h2.finalize());
        REQUIRE(hv == "9c559c4e96dc16f3bfbc40edd8ec76e8");

        // This insertion should change the hash (i.e. the Option is not
        // transparent)
        Hasher h3(HashType::Hash128);
        params.insert("Hash modifying", Option<int>{2, "Opaque thing", {}, {}});
        params.hash(h3);
        hv = bphash::hash_to_string(h2.finalize());
        REQUIRE(hv != "9c559c4e96dc16f3bfbc40edd8ec76e8");
        REQUIRE(hv == "77bcf73aba533f2f6342c4cf6ee1db85");
    }
}
