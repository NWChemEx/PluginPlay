#include <SDE/attic/Parameters.hpp>
#include <catch2/catch.hpp>

using namespace SDE;
using corr_traits_set = std::set<OptionTraits>;
using corr_vector     = std::vector<std::function<bool(const double&)>>;
using corr_size       = std::size_t;

TEST_CASE("Range Checks") {
    GreaterThan<int> gt(3);
    REQUIRE(gt(4) == true);
    REQUIRE(gt(2) == false);

    LessThan<int> lt(3);
    REQUIRE(lt(2) == true);
    REQUIRE(lt(4) == false);
}

template<typename T>
void check_option(Option& opt, T value, std::string desc) {
    REQUIRE(opt.get<T>() == value);
    REQUIRE(opt.description == desc);
}

template<typename T>
using check_function_vector = typename Option::check_function_vector<T>;

TEST_CASE("Options") {
    using traits_set_type = typename Option::traits_set_type;

    SECTION("Typedefs") {
        REQUIRE(std::is_same<traits_set_type, corr_traits_set>::value);
        REQUIRE(
          std::is_same<check_function_vector<double>, corr_vector>::value);
    }

    const std::string desc = "Any positive number";
    const check_function_vector<int> checks{GreaterThan<int>{-1}};
    SECTION("Invalid Ctor Arguments") {
        REQUIRE_THROWS_AS(Option(-1, desc, checks), std::invalid_argument);
    }

    Option opt{4, desc, checks};
    check_option(opt, 4, desc);

    SECTION("Comparison operators") {
        SECTION("Same option") {
            Option opt2{4, desc, checks};
            REQUIRE(opt2 == opt);
            REQUIRE(!(opt2 != opt));
        }

        SECTION("Different values") {
            Option opt2{5, desc, checks};
            REQUIRE(!(opt2 == opt));
            REQUIRE(opt2 != opt);
        }

        SECTION("Different descriptions") {
            Option opt2{4, "Some other desc", checks};
            REQUIRE(!(opt2 == opt));
            REQUIRE(opt2 != opt);
        }

	SECTION("String literal and std::string") {
	  const std::string some_desc = "Some description";	  
	  Option opt_cstr{"A string", some_desc};
	  Option opt_stdstr{std::string("A string"), some_desc};
	  REQUIRE(opt_cstr==opt_stdstr);
	}

    }

    SECTION("Additional Ctors") {
        SECTION("Copy ctor") {
            Option opt2(opt);
            REQUIRE(opt2 == opt);
        }
        SECTION("Move ctor") {
            Option opt2(opt);
            Option opt3(std::move(opt));
            REQUIRE(opt3 == opt2);
        }
        SECTION("Copy assignment") {
            Option opt2;
            opt2 = opt;
            REQUIRE(opt2 == opt);
        }
        SECTION("Move assignment") {
            Option opt2(opt);
            Option opt3;
            opt3 = std::move(opt);
            REQUIRE(opt3 == opt2);
        }
    }

    SECTION("is_valid") {
        REQUIRE(opt.is_valid(4) == true);
        REQUIRE(opt.is_valid(-1) == false);
    }

    SECTION("change") {
        opt.change(5);
        check_option(opt, 5, desc);
        REQUIRE_THROWS_AS(opt.change(-1), std::invalid_argument);
    }

    SECTION("hash") {
        Hasher h(HashType::Hash128);
        opt.hash(h);
        auto hv = bphash::hash_to_string(h.finalize());
        REQUIRE(hv == "4e45e04d0a266e2c319c9528549312fe");
    }
}

void check_parameters(Parameters& params, std::map<std::string, Option> ops) {
    REQUIRE(params.size() == ops.size());
    REQUIRE(params.count("Not a real key") == 0);
    REQUIRE_THROWS_AS(params.change("Not a real key", 2), std::out_of_range);
    REQUIRE_THROWS_AS(params.at<int>("Not a real key"), std::out_of_range);
    using not_type = std::set<int>;
    for(const auto& x : ops) {
        REQUIRE(params.count(x.first));
        REQUIRE(params.at<Option>(x.first) == x.second);
        REQUIRE(params.get_description(x.first) == x.second.description);
        REQUIRE(params.get_traits(x.first) == x.second.traits);
        if(x.first == "The number 3")
            REQUIRE(params.at<int>(x.first) == x.second.get<int>());
        else
            REQUIRE(params.at<std::string>(x.first) ==
                    x.second.get<std::string>());
    }
}

TEST_CASE("Parameters") {
    SECTION("Typedefs") {
        REQUIRE(std::is_same<typename Parameters::size_type, corr_size>::value);
        REQUIRE(std::is_same<typename Parameters::traits_set_type,
                             corr_traits_set>::value);
    }

    Option opt1{3, "Positive number", {GreaterThan<int>{-1}}};
    Option opt2{std::string("Hello World")};
    std::map<std::string, Option> opts{{"The number 3", opt1}};
    Parameters params;
    params.insert("The number 3", opt1);
    check_parameters(params, opts);
    REQUIRE_THROWS_AS(params.change("The number 3", -1), std::invalid_argument);

    SECTION("Comparisons") {
        Parameters params2;
        SECTION("Different") {
            REQUIRE(params != params2);
            REQUIRE(!(params == params2));
        }

        params2.insert("The number 3", opt1);

        SECTION("The same") {
            REQUIRE(params == params2);
            REQUIRE(!(params != params2));
        }
    }

    params.insert("Hello World", opt2);
    opts["Hello World"] = opt2;
    check_parameters(params, opts);

    SECTION("Additional ctors") {
        SECTION("Explicit options") {
            Parameters params2("The number 3", opt1, "Hello World", opt2);
            REQUIRE(params2 == params);
        }

        SECTION("Copy ctor") {
            Parameters params2(params);
            REQUIRE(params2 == params);
        }

        SECTION("Move ctor") {
            Parameters params2(params);
            Parameters params3(std::move(params));
            REQUIRE(params2 == params3);
        }

        SECTION("Copy assignment") {
            Parameters params2;
            params2 = params;
            REQUIRE(params2 == params);
        }

        SECTION("Move assignment") {
            Parameters params2(params);
            Parameters params3;
            params3 = std::move(params);
            REQUIRE(params3 == params2);
        }
    }

    SECTION("Tracking changes") {
        params.track_changes();
        SECTION("Change to same value") {
            params.change("The number 3", 3);
            REQUIRE(params.get_traits("The number 3")
                      .count(OptionTraits::non_default));
        }
        SECTION("Change to different value") {
            params.change("The number 3", 4);
            REQUIRE(params.get_traits("The number 3")
                      .count(OptionTraits::non_default));
        }
    }

    SECTION("Hashing") {
        auto corr_hv = "e9a79951a3d94e52de405dc0e321e2e8";
        SECTION("Before transparent") {
            Hasher h(HashType::Hash128);
            params.hash(h);
            auto hv = bphash::hash_to_string(h.finalize());
            REQUIRE(hv == corr_hv);
        }

        SECTION("After transparent") {
            // This insertion should not change the hash value
            Hasher h(HashType::Hash128);
            params.insert(
              "Hash blind",
              Option{2, "Transparent thing", {}, {OptionTraits::transparent}});
            params.hash(h);
            auto hv = bphash::hash_to_string(h.finalize());
            REQUIRE(hv == corr_hv);
        }

        SECTION("Non-transparent") {
            // This insertion should change the hash (i.e. the Option is not
            // transparent)
            Hasher h(HashType::Hash128);
            params.insert("Hash modifying", Option{2, "Opaque thing", {}, {}});
            params.hash(h);
            auto hv = bphash::hash_to_string(h.finalize());
            REQUIRE(hv != corr_hv);
        }
    }
}
