#include <catch2/catch.hpp>
#include <sde/detail_/rest_printer.hpp>
#include <sstream>
using namespace sde::detail_;

TEST_CASE("reSTPrinter(ss)") {
    std::stringstream ss;
    std::stringstream corr;
    reSTPrinter p(ss, "*=", "* ");
    REQUIRE(ss.str() == corr.str());

    SECTION("Start a section") {
        REQUIRE_THROWS_AS(p.finish_section(), std::runtime_error);
        p.start_section("a section");
        corr << "*********" << std::endl
             << "a section" << std::endl
             << "*********" << std::endl;
        REQUIRE(ss.str() == corr.str());

        SECTION("Start subsection") {
            p.start_section("another section");
            REQUIRE_THROWS_AS(p.start_section(""), std::runtime_error);
            corr << "another section" << std::endl
                 << "===============" << std::endl;
            REQUIRE(ss.str() == corr.str());
            p.finish_section();

            p.start_section("same level section");
            corr << "same level section" << std::endl
                 << "==================" << std::endl;
            REQUIRE(ss.str() == corr.str());

            SECTION("Stop subsection and outer section") {
                p.finish_section();
                p.finish_section();
                p.start_section("a third section");
                corr << "***************" << std::endl
                     << "a third section" << std::endl
                     << "***************" << std::endl;
            }
        }
    }
}

TEST_CASE("reSTPrinter : operator<<") {
    std::stringstream ss;
    std::stringstream corr;
    reSTPrinter p(ss);
    SECTION("Short sentence") {
        const auto sen = "This is a short sentence under 80 characters long";
        p << sen;
        corr << sen;
        REQUIRE(ss.str() == corr.str());
    }
    SECTION("Long sentence") {
        const std::string p1 =
          "This is a long sentence that exceeds 80 characters in length because"
          " I am not";
        const auto p2 = "being even remotely terse while writing it out on the "
                        "computer screen.";
        p << p1 + " " + p2;
        corr << p1 << std::endl << p2;
        REQUIRE(ss.str() == corr.str());
    }
    SECTION("80+ character word") {
        const auto p1 = "This sentence has a long string in it";
        const std::string p2(85, 'x');
        p << p1 << " " << p2;
        corr << p1 << std::endl << p2;
        REQUIRE(ss.str() == corr.str());
    }
    SECTION("Sentence with endline") {
        const std::string p1 = "This sentence has a linebreak in it.\n";
        const auto p2        = "This sentence should appear on the next line.";
        p << p1 + p2;
        corr << p1 << p2;
        REQUIRE(ss.str() == corr.str());
    }
}
