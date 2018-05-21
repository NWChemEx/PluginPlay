#include <SDE/Parameters.hpp>
#include <catch/catch.hpp>
#include <iostream>

using namespace SDE;

TEST_CASE("Basic Parameter Usage") {
    //Declare an instance
    Parameters params;
    //Add our options (this would be done by the module to specify its defaults)
    // Bare minimum way to add an option (no description or checks)
    params.insert("The number 3", Option<int>{3});

    // Better has description, but no check
    params.insert("Pi", Option<double>{3.14,
    "The ratio of a circle's circumfrence to its diameter."});

    // Best has description and checks

//    params.insert("Not a negative number", Option<int>{4, "Any positive number",
//    {GreaterThan<int>{-1}}});

    // See if params contains an option "Hello World"
    bool has_hello_world = params.count("Hello World");
    // Be malicious and change the value of pi (the world will never be the same)
    // Note: this would trigger the value checks (if any exist)
    params.change("Pi", 4.14);
    //Get the number 3
    auto the_number_3 = params.at<int>("The number 3");
    std::cout << the_number_3 << std::endl;
}
