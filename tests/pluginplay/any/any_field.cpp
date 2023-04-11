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

#include "pluginplay/any/any.hpp"
#include "test_any.hpp"
#include <map>
#include <sstream>

using namespace pluginplay::any;

/* Testing Strategy:
 *
 * For the most part creation of AnyField is usually done via `make_any_field`.
 * Similarly unwrapping the AnyField can only be done via `any_cast`.
 * `make_any_field` (as well as `any_cast`) are tested in any.cpp. We simply
 * assume they work here.
 */

TEMPLATE_LIST_TEST_CASE("AnyField", "", testing::types2test) {
    using type      = TestType;
    using rtti_type = AnyField::rtti_type;

    // type we use when we want to guarantee a different wrapped type
    // (assumes std::map<int, int> is not in testing::types2test)
    using map_type = std::map<int, int>;

    // The next lines get a non-default value and make some AnyFields
    auto value = testing::non_default_value<type>();

    AnyField defaulted; // Uses default ctor

    /// All three of these go through the value ctor
    auto default_val = make_any_field<type>(type{});
    auto by_value    = make_any_field<type>(value);
    auto by_cval     = make_any_field<const type>(value);
    auto by_cref     = make_any_field<const type&>(value);
    auto diff        = make_any_field<map_type>(map_type{});

    rtti_type rtti(typeid(type));

    SECTION("Ctors") {
        SECTION("Default Ctor") {
            REQUIRE_FALSE(defaulted.has_value());
            REQUIRE(defaulted.type() == rtti_type(typeid(nullptr)));
        }

        SECTION("Value Ctor") {
            REQUIRE(by_value.has_value());
            REQUIRE(by_value.type() == rtti);
            REQUIRE(any_cast<type>(by_value) == value);

            REQUIRE(by_cval.has_value());
            REQUIRE(by_cval.type() == rtti);
            REQUIRE(any_cast<const type>(by_cval) == value);

            REQUIRE(by_cref.has_value());
            REQUIRE(by_cref.type() == rtti);
            REQUIRE(&any_cast<const type&>(by_cref) == &value);
        }

        SECTION("Copy Ctor") {
            AnyField default_copy(defaulted);
            REQUIRE(default_copy == defaulted);

            AnyField val_copy(by_value);
            REQUIRE(val_copy == by_value);

            AnyField cval_copy(by_cval);
            REQUIRE(cval_copy == by_cval);

            AnyField cref_copy(by_cref);
            REQUIRE(cref_copy == by_cref);

            // Deep copies
            REQUIRE(&any_cast<const type&>(cref_copy) != &value);
        }

        SECTION("Move Ctor") {
            AnyField default_copy(defaulted);
            AnyField default_move(std::move(defaulted));
            REQUIRE(default_copy == default_move);

            AnyField val_copy(by_value);
            AnyField val_move(std::move(by_value));
            REQUIRE(val_copy == val_move);

            AnyField cval_copy(by_cval);
            AnyField cval_move(std::move(by_cval));
            REQUIRE(cval_copy == cval_move);

            AnyField cref_copy(by_cref);
            AnyField cref_move(std::move(by_cref));
            REQUIRE(cref_copy == cref_move);

            // Still aliases the same object (doesn't deep copy it)
            REQUIRE(&any_cast<const type&>(cref_move) == &value);
        }

        SECTION("Copy Assignment") {
            AnyField default_copy;
            auto p = &(default_copy = defaulted);
            REQUIRE(p == &default_copy);
            REQUIRE(default_copy == defaulted);

            AnyField val_copy;
            p = &(val_copy = by_value);
            REQUIRE(p == &val_copy);
            REQUIRE(val_copy == by_value);

            AnyField cval_copy;
            p = &(cval_copy = by_cval);
            REQUIRE(p == &cval_copy);
            REQUIRE(cval_copy == by_cval);

            AnyField cref_copy;
            p = &(cref_copy = by_cref);
            REQUIRE(cref_copy == by_cref);

            // Copy is deep copy
            REQUIRE(&any_cast<const type&>(cref_copy) != &value);
        }

        SECTION("Move Assignment") {
            AnyField default_copy(defaulted);
            AnyField default_move;
            auto p = &(default_move = std::move(defaulted));
            REQUIRE(p == &default_move);
            REQUIRE(default_copy == default_move);

            AnyField val_copy(by_value);
            AnyField val_move;
            p = &(val_move = std::move(by_value));
            REQUIRE(p == &val_move);
            REQUIRE(val_copy == val_move);

            AnyField cval_copy(by_cval);
            AnyField cval_move;
            p = &(cval_move = std::move(by_cval));
            REQUIRE(p == &cval_move);
            REQUIRE(cval_copy == cval_move);

            AnyField cref_copy(by_cref);
            AnyField cref_move;
            p = &(cref_move = std::move(by_cref));
            REQUIRE(p == &cref_move);
            REQUIRE(cref_copy == cref_move);

            // Still aliases the same object (doesn't deep copy it)
            REQUIRE(&any_cast<const type&>(cref_move) == &value);
        }
    }

    SECTION("swap") {
        defaulted.swap(by_value);
        REQUIRE(by_value.are_equal(AnyField{}));
        REQUIRE(defaulted.are_equal(make_any_field<type>(value)));

        defaulted.swap(by_cval);
        REQUIRE(defaulted.are_equal(make_any_field<const type>(value)));
        REQUIRE(by_cval.are_equal(make_any_field<type>(value)));

        defaulted.swap(by_cref);
        REQUIRE(defaulted.are_equal(make_any_field<const type&>(value)));
        REQUIRE(by_cref.are_equal(make_any_field<const type>(value)));
    }

    SECTION("type") {
        REQUIRE(defaulted.type() == rtti_type(typeid(nullptr)));
        REQUIRE(by_value.type() == rtti);
        REQUIRE(by_cval.type() == rtti);
        REQUIRE(by_cref.type() == rtti);
    }

    SECTION("is_convertible") {
        // No value, so this should be false for every type
        REQUIRE_FALSE(defaulted.is_convertible<TestType>());

        // The rest of these just forward to AnyFieldBase::is_convertible, so as
        // long as those unit tests work, these should work too. Here we just
        // spot check by_value
        REQUIRE(by_value.template is_convertible<TestType>());
        REQUIRE(by_value.template is_convertible<const TestType>());
        REQUIRE(by_value.template is_convertible<TestType&>());
        REQUIRE(by_value.template is_convertible<const TestType&>());
        REQUIRE_FALSE(by_value.template is_convertible<map_type>());
    }

    SECTION("operator==/operator!=") {
        // Two default AnyFields
        REQUIRE(defaulted == AnyField{});
        REQUIRE_FALSE(defaulted != AnyField{});

        // AnyFields with same values
        REQUIRE(by_value == make_any_field<type>(value));
        REQUIRE_FALSE(by_value != make_any_field<type>(value));

        // AnyFields with different values
        REQUIRE_FALSE(by_value == default_val);
        REQUIRE(by_value != default_val);

        // AnyFields with same read-only value
        REQUIRE(by_cval == make_any_field<const type>(value));
        REQUIRE_FALSE(by_cval != make_any_field<const type>(value));

        // AnyFields with same read-only reference
        REQUIRE(by_cref == make_any_field<const type&>(value));
        REQUIRE_FALSE(by_cref != make_any_field<const type&>(value));

        // AnyFields that hold the value differently
        REQUIRE(by_value == by_cval);
        REQUIRE_FALSE(by_value != by_cval);
        REQUIRE(by_value == by_cref);
        REQUIRE_FALSE(by_value != by_cref);
        REQUIRE(by_cval == by_cref);
        REQUIRE_FALSE(by_cval != by_cref);

        // AnyFields with different wrapped types
        REQUIRE_FALSE(by_value == diff);
        REQUIRE(by_value != diff);
    }

    SECTION("are_equal") {
        // Two default AnyFields
        REQUIRE(defaulted.are_equal(AnyField{}));

        // AnyFields with same values
        REQUIRE(by_value.are_equal(make_any_field<type>(value)));

        // AnyFields with different values
        REQUIRE_FALSE(by_value.are_equal(default_val));

        // AnyFields with same read-only value
        REQUIRE(by_cval.are_equal(make_any_field<const type>(value)));

        // AnyFields with same read-only reference
        REQUIRE(by_cref.are_equal(make_any_field<const type&>(value)));

        // AnyFields that hold the value differently
        REQUIRE_FALSE(by_value.are_equal(by_cval));
        REQUIRE_FALSE(by_value.are_equal(by_cref));
        REQUIRE_FALSE(by_cval.are_equal(by_cref));

        // AnyFields with different wrapped types
        REQUIRE_FALSE(by_value.are_equal(diff));
    }

    SECTION("print") {
        std::stringstream ss;

        SECTION("default AnyField") {
            auto pss = &defaulted.print(ss);
            REQUIRE(pss == &ss); // Returns ss for chaining
            REQUIRE(ss.str() == "");
        }

        SECTION("AnyField w/ value") {
            auto pss = &by_value.print(ss);
            REQUIRE(pss == &ss); // Returns ss for chaining

            if constexpr(std::is_same_v<type, int>) {
                REQUIRE(ss.str() == "42");
            } else if constexpr(std::is_same_v<type, double>) {
                REQUIRE(ss.str() == "3.14");
            } else if constexpr(std::is_same_v<type, std::string>) {
                REQUIRE(ss.str() == "Hello World");
            }
        }
    }

    SECTION("has_value") {
        REQUIRE_FALSE(defaulted.has_value());
        REQUIRE(by_value.has_value());
        REQUIRE(by_cval.has_value());
        REQUIRE(by_cref.has_value());
    }

    SECTION("owns_value") {
        REQUIRE_FALSE(defaulted.owns_value());
        REQUIRE(by_value.owns_value());
        REQUIRE(by_cval.owns_value());
        REQUIRE_FALSE(by_cref.owns_value());
    }
}

namespace {

struct ABaseClass {
    int x;

    ABaseClass(int _x) : x(_x) {}

    bool operator==(const ABaseClass& rhs) const noexcept { return x == rhs.x; }
};

struct DerivedClass : ABaseClass {
    double y;

    DerivedClass(double _y, int _x) : y(_y), ABaseClass(_x) {}

    bool operator==(const DerivedClass& rhs) const noexcept {
        return y == rhs.y && ABaseClass::operator==(rhs);
    }
};

} // namespace

TEST_CASE("AnyField with polymorphic classes") {
    auto pbase = std::make_unique<ABaseClass>(1);

    auto pderived0             = std::make_unique<DerivedClass>(1.23, 0);
    ABaseClass* pderived0_base = pderived0.get();

    auto pderived1             = std::make_unique<DerivedClass>(1.23, 1);
    ABaseClass* pderived1_base = pderived1.get();

    SECTION("Sanity check classes") {
        REQUIRE(pbase->x == 1);
        REQUIRE(pderived0->x == 0);
        REQUIRE(pderived0->y == 1.23);
        REQUIRE(pderived1->x == 1);
        REQUIRE(pderived1->y == 1.23);
        REQUIRE_FALSE(*pbase == *pderived0);
        REQUIRE(*pbase == *pderived1);

        auto pother = std::make_unique<DerivedClass>(3.14, 1);
        REQUIRE_FALSE(*pderived0 == *pother);
        REQUIRE_FALSE(*pderived1 == *pother);

        ABaseClass* pother_base = pother.get();
        REQUIRE_FALSE(*pderived0_base == *pother_base);
        REQUIRE(*pderived1_base == *pother_base);
    }

    SECTION("Wrap by const ref to base") {
        using wrapped_type = const ABaseClass&;

        // Wrap the instance which is a base
        auto te_base = make_any_field<wrapped_type>(*pbase);
        // Verify it was wrapped by reference
        REQUIRE(&any_cast<wrapped_type>(te_base) == pbase.get());

        // Wrap derived0 as if it were a base instance
        auto te_derived0 = make_any_field<wrapped_type>(*pderived0);
        // Verify it was wrapped by ref to the base
        REQUIRE(&any_cast<wrapped_type>(te_derived0) == pderived0_base);

        // Wrap derived1 as if it were a base instance
        auto te_derived1 = make_any_field<wrapped_type>(*pderived1);
        // Verify it was wrapped by ref to the base
        REQUIRE(&any_cast<wrapped_type>(te_derived1) == pderived1_base);

        // Comparisons should only consider the base class
        REQUIRE_FALSE(te_base == te_derived0);
        REQUIRE(te_base == te_derived1);
        REQUIRE_FALSE(te_derived0 == te_derived1);

        // Copy slices
        AnyField base_copy(te_base);
        REQUIRE(&any_cast<wrapped_type>(base_copy) != pbase.get());

        AnyField d0_copy(te_derived0);
        REQUIRE(&any_cast<wrapped_type>(d0_copy) != pderived0_base);

        AnyField d1_copy(te_derived1);
        REQUIRE(&any_cast<wrapped_type>(d1_copy) != pderived1_base);
    }

    SECTION("Wrap by const ref to derived") {
        using wrapped_type = const DerivedClass&;

        auto te_derived0 = make_any_field<wrapped_type>(*pderived0);
        REQUIRE(&any_cast<wrapped_type>(te_derived0) == pderived0.get());
        auto te_derived1 = make_any_field<wrapped_type>(*pderived1);
        REQUIRE(&any_cast<wrapped_type>(te_derived1) == pderived1.get());

        REQUIRE_FALSE(te_derived0 == te_derived1);

        // Copy doesn't slice
        AnyField d0_copy(te_derived0);
        REQUIRE(&any_cast<wrapped_type>(d0_copy) != pderived0.get());
        REQUIRE(any_cast<wrapped_type>(d0_copy) == *pderived0);

        AnyField d1_copy(te_derived1);
        REQUIRE(&any_cast<wrapped_type>(d1_copy) != pderived1.get());
        REQUIRE(any_cast<wrapped_type>(d1_copy) == *pderived1);
    }
}
