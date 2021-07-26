#include "pluginplay/detail_/module_input_pimpl.hpp"
#include <catch2/catch.hpp>

using namespace pluginplay;
using namespace pluginplay::detail_;
using check_fxn = typename ModuleInputPIMPL::any_check;

TEST_CASE("ModuleInputPIMPL : Default ctor") {
    ModuleInputPIMPL pimpl;
    REQUIRE_FALSE(pimpl.has_value());
    REQUIRE_FALSE(pimpl.has_type());
    REQUIRE_FALSE(pimpl.has_description());
    REQUIRE_FALSE(pimpl.is_optional());
    REQUIRE_FALSE(pimpl.is_transparent());
    REQUIRE_FALSE(pimpl.is_ready());
    REQUIRE_THROWS_AS(pimpl.is_valid(make_Any<double>(double{3})),
                      std::runtime_error);
}

TEST_CASE("ModuleInputPIMPL : has_type") {
    ModuleInputPIMPL p;
    SECTION("No type") { REQUIRE_FALSE(p.has_type()); }
    SECTION("Has type") {
        p.set_type(std::type_index(typeid(int)));
        REQUIRE(p.has_type());
    }
}

TEST_CASE("ModuleInputPIMPL : has_value") {
    ModuleInputPIMPL p;
    SECTION("No value") { REQUIRE_FALSE(p.has_value()); }
    SECTION("Has value") {
        p.set_type(std::type_index(typeid(int)));
        p.set_value(make_Any<int>(3));
        REQUIRE(p.has_value());
    }
}

TEST_CASE("ModuleInputPIMPL : has_description") {
    ModuleInputPIMPL p;
    SECTION("No description") { REQUIRE_FALSE(p.has_description()); }

    SECTION("Description") {
        p.set_description("hi");
        REQUIRE(p.has_description());
    }
}

TEST_CASE("ModuleInputPIMPL : is_optional") {
    ModuleInputPIMPL p;
    SECTION("Required") { REQUIRE_FALSE(p.is_optional()); }
    SECTION("Optional") {
        p.make_optional();
        REQUIRE(p.is_optional());
    }
}

TEST_CASE("ModuleInputPIMPL : is_transparent") {
    ModuleInputPIMPL p;
    SECTION("Opaque") { REQUIRE_FALSE(p.is_transparent()); }
    SECTION("Transparent") {
        p.make_transparent();
        REQUIRE(p.is_transparent());
    }
}

TEST_CASE("ModuleInputPIMPL : is_ready") {
    ModuleInputPIMPL p;
    SECTION("Not ready") { REQUIRE_FALSE(p.is_ready()); }
    SECTION("Optional") {
        p.make_optional();
        REQUIRE(p.is_ready());
    }
    SECTION("Has value") {
        p.set_type(std::type_index(typeid(int)));
        p.set_value(make_Any<int>(3));
        REQUIRE(p.is_ready());
    }
}

TEST_CASE("ModuleInputPIMPL : is_valid") {
    ModuleInputPIMPL p;
    REQUIRE_THROWS_AS(p.is_valid(make_Any<int>(3)), std::runtime_error);
    p.set_type(std::type_index(typeid(int)));

    SECTION("No checks") { REQUIRE(p.is_valid(make_Any<double>(1.1))); }

    SECTION("Has checks") {
        p.add_check([](const Any& any) { return any.has_value(); });
        REQUIRE(p.is_valid(make_Any<int>(3)));
        REQUIRE_FALSE(p.is_valid(Any{}));
    }
}

TEST_CASE("ModuleInputPIMPL : hash") {
    ModuleInputPIMPL p;
    p.set_type(std::type_index(typeid(int)));
    SECTION("No value") {
        REQUIRE(pluginplay::hash_objects(p) ==
                "cbc357ccb763df2852fee8c4fc7d55f2");
    }
    SECTION("has value") {
        p.set_value(make_Any<int>(3));
        REQUIRE(pluginplay::hash_objects(p) ==
                "9a4294b64e60cc012c5ed48db4cd9c48");
    }
    SECTION("is transparent") {
        p.set_value(make_Any<int>(3));
        p.make_transparent();
        REQUIRE(pluginplay::hash_objects(p) ==
                "00000000000000000000000000000000");
    }
}

TEST_CASE("ModuleInputPIMPL : set_type") {
    ModuleInputPIMPL p;
    const auto t1 = std::type_index(typeid(int));
    const auto t2 = std::type_index(typeid(double));
    SECTION("Can set type") {
        p.set_type(t1);
        REQUIRE(p.has_type());
        REQUIRE(p.type() == t1);
    }
    SECTION("Can change type as long as no value") {
        p.set_type(t1);
        p.set_type(t2);
        REQUIRE(p.type() == t2);
    }
    SECTION("Can change type to same type even w/value") {
        p.set_type(t1);
        p.set_value(make_Any<int>(3));
        p.set_type(t1);
        REQUIRE(p.type() == t1);
    }
    SECTION("Can't change type after value has been set") {
        p.set_type(t1);
        p.set_value(make_Any<int>(3));
        REQUIRE_THROWS_AS(p.set_type(t2), std::runtime_error);
    }
}

TEST_CASE("ModuleInputPIMPL : set_value") {
    ModuleInputPIMPL p;
    const auto t1 = std::type_index(typeid(int));
    auto any      = make_Any<int>(3);
    SECTION("Can't set value w/o setting type") {
        REQUIRE_THROWS_AS(p.set_value(any), std::runtime_error);
    }
    SECTION("Can set to valid value") {
        p.set_type(t1);
        p.set_value(any);
        REQUIRE(p.value() == any);
    }
    SECTION("Can't set to an invalid value") {
        p.set_type(t1);
        p.add_check([](const type::any& a) { return a.has_value(); });
        REQUIRE_THROWS_AS(p.set_value(type::any{}), std::invalid_argument);
    }
}

TEST_CASE("ModuleInputPIMPL : set_description") {
    ModuleInputPIMPL p;
    p.set_description("Hello world");

    REQUIRE(p.description() == "Hello world");
}

TEST_CASE("ModuleInputPIMPL : add_check") {
    ModuleInputPIMPL p;
    p.set_type(std::type_index(typeid(int)));
    auto any = make_Any<int>(4);
    auto l   = [](const Any& a) { return a.cast<int>() != 4; };
    SECTION("Add a check") {
        p.add_check(l, "a check");
        REQUIRE(p.check_descriptions() == std::set<std::string>{"a check"});
        REQUIRE_FALSE(p.is_valid(any));
    }
    SECTION("Can't add a check if value fails it") {
        p.set_value(any);
        REQUIRE_THROWS_AS(p.add_check(l), std::invalid_argument);
    }
}

TEST_CASE("ModuleInputPIMPL : make_optional") {
    ModuleInputPIMPL p;
    REQUIRE_FALSE(p.is_optional());
    SECTION("Changes required to optional") {
        p.make_optional();
        REQUIRE(p.is_optional());
    }
    SECTION("Does nothing if already optional") {
        p.make_optional();
        p.make_optional();
        REQUIRE(p.is_optional());
    }
}

TEST_CASE("ModuleInputPIMPL : make_required") {
    ModuleInputPIMPL p;
    REQUIRE_FALSE(p.is_optional());
    SECTION("Undoes optional") {
        p.make_optional();
        REQUIRE(p.is_optional());
        p.make_required();
        REQUIRE_FALSE(p.is_optional());
    }
    SECTION("Does nothing if already required") {
        p.make_required();
        REQUIRE_FALSE(p.is_optional());
    }
}

TEST_CASE("ModuleInputPIMPL : make_transparent") {
    ModuleInputPIMPL p;
    REQUIRE_FALSE(p.is_transparent());
    SECTION("Changes opaque to transparent") {
        p.make_transparent();
        REQUIRE(p.is_transparent());
    }
    SECTION("Does nothing if already transparent") {
        p.make_transparent();
        p.make_transparent();
        REQUIRE(p.is_transparent());
    }
}

TEST_CASE("ModuleInputPIMPL : make_opaque") {
    ModuleInputPIMPL p;
    REQUIRE_FALSE(p.is_transparent());
    SECTION("Undoes transparent") {
        p.make_transparent();
        REQUIRE(p.is_transparent());
        p.make_opaque();
        REQUIRE_FALSE(p.is_transparent());
    }
    SECTION("Does nothing if already required") {
        p.make_opaque();
        REQUIRE_FALSE(p.is_transparent());
    }
}

TEST_CASE("ModuleInputPIMPL : type") {
    ModuleInputPIMPL p;
    SECTION("Throws if type is not set") {
        REQUIRE_THROWS_AS(p.type(), std::runtime_error);
    }
    SECTION("Returns the type if set") {
        auto t = std::type_index(typeid(int));
        p.set_type(t);
        REQUIRE(p.type() == t);
    }
}

TEST_CASE("ModuleInputPIMPL : value") {
    ModuleInputPIMPL p;
    p.set_type(std::type_index(typeid(int)));
    auto any = make_Any<int>(3);

    SECTION("Throws if value is not set") {
        REQUIRE_THROWS_AS(p.value(), std::runtime_error);
    }

    SECTION("Can get value") {
        p.set_value(any);
        REQUIRE(p.value() == any);
    }
}

TEST_CASE("ModuleInputPIMPL : description") {
    ModuleInputPIMPL p;
    SECTION("Throws if description is not set") {
        REQUIRE_THROWS_AS(p.description(), std::bad_optional_access);
    }
    SECTION("Can get the value") {
        p.set_description("Hello world");
        REQUIRE(p.description() == "Hello world");
    }
}

TEST_CASE("ModuleInputPIMPL : check_descriptions") {
    ModuleInputPIMPL p;
    using set_t = typename ModuleInputPIMPL::check_description_type;
    auto l      = [](const Any&) { return true; };
    SECTION("No descriptions") { REQUIRE(p.check_descriptions() == set_t{}); }
    SECTION("No description provided") {
        p.add_check(l);
        REQUIRE(p.check_descriptions() == set_t{""});
    }
    SECTION("Description provided") {
        p.add_check(l, "hi");
        REQUIRE(p.check_descriptions() == set_t{"hi"});
    }
}

TEST_CASE("ModuleInputPIMPL : Equality comparisons") {
    ModuleInputPIMPL p, p2;

    SECTION("Defaults are equivalent") {
        REQUIRE(p == p2);
        REQUIRE_FALSE(p != p2);
    }
    SECTION("Different types") {
        p2.set_type(std::type_index(typeid(int)));
        REQUIRE_FALSE(p == p2);
        REQUIRE(p != p2);
    }
    SECTION("Different values") {
        p2.set_type(std::type_index(typeid(int)));
        p.set_type(std::type_index(typeid(int)));
        p2.set_value(make_Any<int>(4));
        p.set_value(make_Any<int>(3));
        REQUIRE_FALSE(p == p2);
        REQUIRE(p != p2);
    }
    SECTION("Different descriptions") {
        p2.set_description("hello world");
        REQUIRE_FALSE(p == p2);
        REQUIRE(p != p2);
    }
    SECTION("Different optional-ness") {
        p2.make_optional();
        REQUIRE_FALSE(p == p2);
        REQUIRE(p != p2);
    }
    SECTION("Different transparencies") {
        p2.make_transparent();
        REQUIRE_FALSE(p == p2);
        REQUIRE(p != p2);
    }
}
