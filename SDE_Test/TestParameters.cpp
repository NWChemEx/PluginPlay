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
}

TEST_CASE("Options") {
    Option opt{4, "Any positive number", {GreaterThan<int>{-1}}};

    REQUIRE(detail_::SDEAnyCast<int>(opt.value) == 4);
    opt.value = detail_::SDEAny(5);
    REQUIRE(opt.is_valid() == true);
    opt.value = detail_::SDEAny(-1);
    REQUIRE(opt.is_valid() == false);

    Hasher h(HashType::Hash128);
    opt.hash(h);
    auto hv = bphash::hash_to_string(h.finalize());
    REQUIRE(hv == "507d9f3cedf0b4a0aaf0312b8a767d0c");

    REQUIRE_THROWS(Option{4, "Any negative number", {LessThan<int>{0}}});
    REQUIRE_NOTHROW(Option{-1, "Any negative number", {LessThan<int>{0}}});
}

TEST_CASE("Parameters") {
    Parameters params;

    Option opt(3, "some description");
    // insert() and at()
    params.insert("The number 3", opt);
    REQUIRE(params.at<int>("The number 3") == 3);
    // Using at<Option<T>>()
    REQUIRE(detail_::SDEAnyCast<int>(params.at<Option>("The number 3").value) ==
            3);
    // change() and track_changes()
    params.track_changes();
    params.change("The number 3", 2);
    REQUIRE(params.at<int>("The number 3") == 2);
    auto new_traits = params.get_traits("The number 3");
    REQUIRE(new_traits.count(OptionTraits::non_default));

    // checking range_checks and change()
    params.insert("Not a negative number",
                  Option{4, "Any positive number", {GreaterThan<int>{-1}}});
    REQUIRE_THROWS(params.change("Not a negative number", -1));
    REQUIRE_NOTHROW(params.change("Not a negative number", 6));

    // count()
    REQUIRE(params.count("Hello World") == false);
    params.insert("Hello World", Option{std::string{"Hello world"}});
    REQUIRE(params.count("Hello World") == true);

    Hasher h(HashType::Hash128);
    params.hash(h);
    auto hv = bphash::hash_to_string(h.finalize());
    REQUIRE(hv == "c2007ca976923cbe8fc6fba066282913");

    // This insertion should not change the hash value
    Hasher h2(HashType::Hash128);
    params.insert(
      "Hash blind",
      Option{2, "Transparent thing", {}, {OptionTraits::transparent}});
    params.hash(h2);
    hv = bphash::hash_to_string(h2.finalize());
    REQUIRE(hv == "c2007ca976923cbe8fc6fba066282913");

    // This insertion should change the hash (i.e. the Option is not
    // transparent)
    Hasher h3(HashType::Hash128);
    params.insert("Hash modifying", Option{2, "Opaque thing", {}, {}});
    params.hash(h3);
    hv = bphash::hash_to_string(h2.finalize());
    REQUIRE(hv == "ca89719299107675e821851b2ec20cba");
}
// pybind11 objects
/*TEST_CASE("PyParameters") {
    Parameters pyparams;
    pyparams.insert("Double", Option{3.14});
    pyobject double_obj = pyparams.at_python("Double");
    REQUIRE(double_obj.cast<double>() == 3.14);

    pyobject new_obj = pybind11::cast(1.57);
    pyparams.change_python("Double", new_obj);
    REQUIRE(pyparams.at<double>("Double") == 1.57);
}*/
