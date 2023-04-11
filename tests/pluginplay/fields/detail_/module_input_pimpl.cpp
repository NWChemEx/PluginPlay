/*
 * Copyright 2022 NWChemEx-Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <catch2/catch.hpp>
#include <pluginplay/fields/detail_/module_input_pimpl.hpp>

using namespace pluginplay;
using namespace pluginplay::detail_;
using check_fxn = typename ModuleInputPIMPL::any_check;

TEST_CASE("ModuleInputPIMPL") {
    ModuleInputPIMPL pimpl;

    SECTION("Default ctor") {
        REQUIRE_FALSE(pimpl.has_value());
        REQUIRE_FALSE(pimpl.has_type());
        REQUIRE_FALSE(pimpl.has_description());
        REQUIRE_FALSE(pimpl.is_optional());
        REQUIRE_FALSE(pimpl.is_transparent());
        REQUIRE_FALSE(pimpl.is_ready());
        auto an_any = any::make_any_field<double>(double{3});
        REQUIRE_THROWS_AS(pimpl.is_valid(an_any), std::runtime_error);
    }

    SECTION("has_type") {
        SECTION("No type") { REQUIRE_FALSE(pimpl.has_type()); }
        SECTION("Has type") {
            pimpl.set_type(std::type_index(typeid(int)));
            REQUIRE(pimpl.has_type());
        }
    }

    SECTION("has_value") {
        SECTION("No value") { REQUIRE_FALSE(pimpl.has_value()); }
        SECTION("Has value") {
            pimpl.set_type(std::type_index(typeid(int)));
            pimpl.set_value(any::make_any_field<int>(3));
            REQUIRE(pimpl.has_value());
        }
    }

    SECTION("has_description") {
        SECTION("No description") { REQUIRE_FALSE(pimpl.has_description()); }

        SECTION("Description") {
            pimpl.set_description("hi");
            REQUIRE(pimpl.has_description());
        }
    }

    SECTION("is_optional") {
        SECTION("Required") { REQUIRE_FALSE(pimpl.is_optional()); }
        SECTION("Optional") {
            pimpl.make_optional();
            REQUIRE(pimpl.is_optional());
        }
    }

    SECTION("is_transparent") {
        SECTION("Opaque") { REQUIRE_FALSE(pimpl.is_transparent()); }
        SECTION("Transparent") {
            pimpl.make_transparent();
            REQUIRE(pimpl.is_transparent());
        }
    }

    SECTION("is_ready") {
        SECTION("Not ready") { REQUIRE_FALSE(pimpl.is_ready()); }
        SECTION("Optional") {
            pimpl.make_optional();
            REQUIRE(pimpl.is_ready());
        }
        SECTION("Has value") {
            pimpl.set_type(std::type_index(typeid(int)));
            pimpl.set_value(any::make_any_field<int>(3));
            REQUIRE(pimpl.is_ready());
        }
    }

    SECTION("is_valid") {
        REQUIRE_THROWS_AS(pimpl.is_valid(any::make_any_field<int>(3)),
                          std::runtime_error);
        pimpl.set_type(std::type_index(typeid(int)));

        SECTION("No checks") {
            REQUIRE(pimpl.is_valid(any::make_any_field<double>(1.1)));
        }

        SECTION("Has checks") {
            pimpl.add_check(
              [](const type::any& any) { return any.has_value(); });
            REQUIRE(pimpl.is_valid(any::make_any_field<int>(3)));
            REQUIRE_FALSE(pimpl.is_valid(type::any{}));
        }
    }

    SECTION("set_type") {
        const auto t1 = std::type_index(typeid(int));
        const auto t2 = std::type_index(typeid(double));
        SECTION("Can set type") {
            pimpl.set_type(t1);
            REQUIRE(pimpl.has_type());
            REQUIRE(pimpl.type() == t1);
        }
        SECTION("Can change type as long as no value") {
            pimpl.set_type(t1);
            pimpl.set_type(t2);
            REQUIRE(pimpl.type() == t2);
        }
        SECTION("Can change type to same type even w/value") {
            pimpl.set_type(t1);
            pimpl.set_value(any::make_any_field<int>(3));
            pimpl.set_type(t1);
            REQUIRE(pimpl.type() == t1);
        }
        SECTION("Can't change type after value has been set") {
            pimpl.set_type(t1);
            pimpl.set_value(any::make_any_field<int>(3));
            REQUIRE_THROWS_AS(pimpl.set_type(t2), std::runtime_error);
        }
    }

    SECTION("set_value") {
        const auto t1 = std::type_index(typeid(int));
        auto any      = any::make_any_field<int>(3);
        SECTION("Can't set value w/o setting type") {
            REQUIRE_THROWS_AS(pimpl.set_value(any), std::runtime_error);
        }
        SECTION("Can set to valid value") {
            pimpl.set_type(t1);
            pimpl.set_value(any);
            REQUIRE(pimpl.value() == any);
        }
        SECTION("Can't set to an invalid value") {
            pimpl.set_type(t1);
            pimpl.add_check([](const type::any& a) { return a.has_value(); });
            REQUIRE_THROWS_AS(pimpl.set_value(type::any{}),
                              std::invalid_argument);
        }
    }

    SECTION("set_description") {
        ModuleInputPIMPL p;
        p.set_description("Hello world");

        REQUIRE(p.description() == "Hello world");
    }

    SECTION("add_check") {
        ModuleInputPIMPL p;
        p.set_type(std::type_index(typeid(int)));
        auto any = any::make_any_field<int>(4);
        auto l = [](const type::any& a) { return any::any_cast<int>(a) != 4; };
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

    SECTION("make_optional") {
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

    SECTION("make_required") {
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

    SECTION("make_transparent") {
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

    SECTION("make_opaque") {
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

    SECTION("type") {
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

    SECTION("value") {
        ModuleInputPIMPL p;
        p.set_type(std::type_index(typeid(int)));
        auto any = any::make_any_field<int>(3);

        SECTION("Throws if value is not set") {
            REQUIRE_THROWS_AS(p.value(), std::runtime_error);
        }

        SECTION("Can get value") {
            p.set_value(any);
            REQUIRE(p.value() == any);
        }
    }

    SECTION("description") {
        ModuleInputPIMPL p;
        SECTION("Throws if description is not set") {
            REQUIRE_THROWS_AS(p.description(), std::bad_optional_access);
        }
        SECTION("Can get the value") {
            p.set_description("Hello world");
            REQUIRE(p.description() == "Hello world");
        }
    }

    SECTION("check_descriptions") {
        ModuleInputPIMPL p;
        using set_t = typename ModuleInputPIMPL::check_description_type;
        auto l      = [](const type::any&) { return true; };
        SECTION("No descriptions") {
            REQUIRE(p.check_descriptions() == set_t{});
        }
        SECTION("No description provided") {
            p.add_check(l);
            REQUIRE(p.check_descriptions() == set_t{""});
        }
        SECTION("Description provided") {
            p.add_check(l, "hi");
            REQUIRE(p.check_descriptions() == set_t{"hi"});
        }
    }

    SECTION("Equality comparisons") {
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
            p2.set_value(any::make_any_field<int>(4));
            p.set_value(any::make_any_field<int>(3));
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
}
