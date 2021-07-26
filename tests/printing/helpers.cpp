#include <catch2/catch.hpp>
#include <sstream>
#include <iostream>
#include "src/sde/printing/helpers.hpp"

using namespace sde::printing;

TEST_CASE("generate_index_string") {
    SECTION("Empty toc") {
	auto corr = ".. toctree::\n"
	            "   :maxdepth: 1\n";

	std::stringstream ss;
	std::vector<std::string> toc;

	generate_index_file(ss, toc);
	
        REQUIRE(ss.str() == corr);
    }

    SECTION("Full toc") {
	auto corr = ".. toctree::\n"
	            "   :maxdepth: 1\n"
	            "\n"
	            "   Coulomb's_Law\n"
	            "   Coulomb's_Law_with_screening\n"
	            "   Force\n"
	            "   Single-precision_Coulomb's_law";

	std::stringstream ss;
	std::vector<std::string> toc = {
	    "Coulomb's_Law",
	    "Coulomb's_Law_with_screening",
	    "Force",
	    "Single-precision_Coulomb's_law"
	};

	generate_index_file(ss, toc);
	
        REQUIRE(ss.str() == corr);
    }
}

TEST_CASE("replace_invalid_chars") {
    SECTION("Empty string") {
	auto corr = "";

	std::string clean_str = replace_invalid_chars("", "<>:\"|?*\\/ ", '_');

	REQUIRE(clean_str == corr);
    }

    SECTION("Empty invalid chars") {
	auto corr = "test";

	std::string clean_str = replace_invalid_chars("test", "", '_');

	REQUIRE(clean_str == corr);
    }
    
    SECTION("Replace all") {
	auto corr = "__________";

	std::string clean_str = replace_invalid_chars("<>:\"|?*\\/ ",
						      "<>:\"|?*\\/ ", '_');

	REQUIRE(clean_str == corr);
    }
}
