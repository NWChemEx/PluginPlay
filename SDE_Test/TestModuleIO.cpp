#include <SDE/detail_/ModuleIOOptions.hpp>
#include <catch2/catch.hpp>
using namespace SDE;
using namespace SDE::detail_;

/* The ModuleIO, ModuleInput, and ModuleOutput classes are designed to be
 * constructed via the ModuleIOOptions, ModuleInputOptions, and
 * ModuleOutputOptions classes respectively. The ModuleIO/ModuleIOOptions pair
 * serves as code factorization for the latter two pairs. While the
 * ModuleIO/ModuleIOOptions pair of classes are not meant to be used on their
 * own in production code, they do impart the majority of the functionality and
 * are capable of being tested individually. For this reason the first test case
 * focuses on this pair. The second test case focuses on the
 * ModuleInput/ModuleInputOptions pair (at the moment the Output class is just a
 * typedef of the IO class).
 */

template<typename T>
void check_state(ModuleIO& io, std::string desc = "") {
    SECTION("Type"){
        REQUIRE(io.type() == typeid(T));
    }
    SECTION("Description"){
        REQUIRE(io.desc == desc);
    }
}

template<typename T>
void check_state(ModuleIO& io, T value, std::string desc = "") {
    check_state<T>(io, desc);
    SECTION("Correct value"){
        REQUIRE(io.value<T>() == value);
    }
}

void check_state(ModuleInput& inp, bool trans, bool opt) {
    SECTION("Is Transparent?"){
        REQUIRE(inp.is_transparent == trans);
    }
    SECTION("Is Optional"){
        REQUIRE(inp.is_optional == opt);
    }
}

TEST_CASE("ModuleIO/ModuleIOOptions Classes") {
    ModuleIO io;
    detail_::ModuleIOOptions opt(io);

    SECTION("Default Ctor") {
        check_state<std::nullptr_t>(io);
    }

    SECTION("Set the description") {
        auto desc = "A description";
        opt.description(desc);
        check_state<std::nullptr_t>(io, desc);
    }

    SECTION("Set the type") {
        opt.type<int>();
        check_state<int>(io);
    }

    opt.type<int>();
    int value = 3;

    SECTION("Change the value") {
        SECTION("Same type") {
            io.change(value);
            check_state(io, value);
        }
        SECTION("Prevents setting to other type") {
            REQUIRE_THROWS_AS(io.change(double{3.14}), std::invalid_argument);
        }
    }

    SECTION("Set the default value"){
        opt.default_value(value);
        check_state(io, value);
    }
    opt.default_value(value);
    int other = 4;
    SECTION("Add a check"){
        typename ModuleIOOptions::value_checker<int> check =
            [](const int& x){ return x == 3; };
        opt.check(check);
        SECTION("is_valid: valid argument") {
            REQUIRE(io.is_valid(value));
        }
        SECTION("is_valid: invalid argument") {
            REQUIRE(!io.is_valid(other));
        }
        SECTION("Change does not work if invalid") {
            REQUIRE_THROWS_AS(io.change(other), std::invalid_argument);
        }
    }

    SECTION("Copy Ctor") {
        ModuleIO copy(io);
        check_state(copy, value);
    }

    SECTION("Copy Assignment") {
        ModuleIO copy;
        auto& pcopy = (copy = io);
        check_state(copy, value);
        SECTION("Supports chaining") {
            REQUIRE(&pcopy == &copy);
        }
    }

    SECTION("Move Ctor") {
        ModuleIO moved(std::move(io));
        check_state(moved, value);
    }

    SECTION("Move Assignment") {
        ModuleIO moved;
        auto& pmoved = (moved = std::move(io));
        check_state(moved, value);
        SECTION("Supports chaining") {
            REQUIRE(&pmoved == &moved);
        }
    }

   SECTION("Works with references"){
        ModuleIO ref;
        ModuleIOOptions ref_opts(ref);
        SECTION("Read/Write References") {
            ref_opts.type<int &>();
            check_state<int &>(ref);
            ref.change(value);
            check_state<int &>(ref, value);
            REQUIRE(&(ref.value<int&>()) == &value);
        }
        SECTION("Read-Only References"){
            ref_opts.type<const int&>();
            check_state<const int&>(ref);
            ref.change(value);
            check_state<const int&>(ref, value);
            REQUIRE(&(ref.value<const int&>()) == &value);
        }
    }
}

TEST_CASE("ModuleInput/ModuleInputOptions Classes") {
    ModuleInput inp;
    ModuleInputOptions opt(inp);

    SECTION("Default Ctor") {
        check_state(inp, false, false);
        check_state<std::nullptr_t>(inp);
    }

    SECTION("Can make transparent"){
        opt.transparent();
        check_state(inp, true, false);
        check_state<std::nullptr_t>(inp);
    }

    SECTION("Can make optional"){
        opt.optional();
        check_state(inp, false, true);
        check_state<std::nullptr_t>(inp);
    }

    int value = 3;
    opt.type<int>();
    opt.default_value(value);
    opt.transparent();

    SECTION("Copy Ctor"){
        ModuleInput copy(inp);
        check_state(copy, true, false);
        check_state(copy, value);
    }

    SECTION("Copy Assignment"){
        ModuleInput copy;
        auto& pcopy = (copy = inp);
        check_state(copy, true, false);
        check_state(copy, value);
        SECTION("Can chain"){
            REQUIRE(&pcopy == &copy);
        }
    }

    SECTION("Move Ctor"){
        ModuleInput copy(std::move(inp));
        check_state(copy, true, false);
        check_state(copy, value);
    }

    SECTION("Move Assignment"){
        ModuleInput copy;
        auto& pcopy = (copy = std::move(inp));
        check_state(copy, true, false);
        check_state(copy, value);
        SECTION("Can chain"){
            REQUIRE(&pcopy == &copy);
        }
    }
}
