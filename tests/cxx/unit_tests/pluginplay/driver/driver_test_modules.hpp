/*
 * Copyright 2026 NWChemEx-Project
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

/* Modules and drivers used to test drivers.
 *
 * The call graph the drivers configure is
 *
 *   "Wrapper" -> "Adder" -> "Leaf"       (submodule "a")
 *                        -> "Other leaf" (submodule "b")
 *
 * "Wrapper" returns the value of "Adder", which returns the sum of its
 * submodules plus its "offset" input. Each leaf returns its "x" input. With
 * the defaults, "Wrapper" returns 1 + 1 + 0 = 2.
 */

#pragma once
#include <memory>
#include <pluginplay/pluginplay.hpp>
#include <string>
#include <vector>

namespace testing::drivers {

DECLARE_PROPERTY_TYPE(Value);
PROPERTY_TYPE_INPUTS(Value) { return pluginplay::declare_input(); }
PROPERTY_TYPE_RESULTS(Value) {
    return pluginplay::declare_result().add_field<int>("value");
}

/// Value, but with an input "x", passed on each call
DECLARE_PROPERTY_TYPE(ValueOfX);
PROPERTY_TYPE_INPUTS(ValueOfX) {
    return pluginplay::declare_input().add_field<int>("x");
}
PROPERTY_TYPE_RESULTS(ValueOfX) {
    return pluginplay::declare_result().add_field<int>("value");
}

/// Returns its input "x". Counts its calls, so memoization can be tested
struct Leaf : pluginplay::ModuleBase {
    static inline int calls = 0;
    Leaf() : pluginplay::ModuleBase(this) {
        satisfies_property_type<Value>();
        add_input<int>("x").set_default(1);
    }
    pluginplay::type::result_map run_(
      pluginplay::type::input_map inputs,
      pluginplay::type::submodule_map) const override {
        ++calls;
        auto rv = results();
        return Value::wrap_results(rv, inputs.at("x").value<int>());
    }
};

/// Same as Leaf, with its own count
struct OtherLeaf : pluginplay::ModuleBase {
    static inline int calls = 0;
    OtherLeaf() : pluginplay::ModuleBase(this) {
        satisfies_property_type<Value>();
        add_input<int>("x").set_default(1);
    }
    pluginplay::type::result_map run_(
      pluginplay::type::input_map inputs,
      pluginplay::type::submodule_map) const override {
        ++calls;
        auto rv = results();
        return Value::wrap_results(rv, inputs.at("x").value<int>());
    }
};

/// Returns "a" + "b" + "offset"
DECLARE_MODULE(Adder);
inline MODULE_CTOR(Adder) {
    satisfies_property_type<Value>();
    add_submodule<Value>("a");
    add_submodule<Value>("b");
    add_input<int>("offset").set_default(0);
}
inline MODULE_RUN(Adder) {
    auto a  = submods.at("a").run_as<Value>();
    auto b  = submods.at("b").run_as<Value>();
    auto rv = results();
    return Value::wrap_results(rv, a + b + inputs.at("offset").value<int>());
}

/// Returns the value of "inner"
DECLARE_MODULE(Wrapper);
inline MODULE_CTOR(Wrapper) {
    satisfies_property_type<Value>();
    add_submodule<Value>("inner");
}
inline MODULE_RUN(Wrapper) {
    auto rv = results();
    return Value::wrap_results(rv, submods.at("inner").run_as<Value>());
}

/// Code factorization for the drivers, which return the value of "root"
#define DRIVER_RUNS_ROOT(driver_name)                                       \
    inline DRIVER_RUN(driver_name) {                                        \
        auto rv = results();                                                \
        return Value::wrap_results(rv, submods.at("root").run_as<Value>()); \
    }

/// Forwards its "x" to "Leaf"'s "x"
DECLARE_DRIVER(XDriver);
inline DRIVER_CTOR(XDriver) {
    satisfies_property_type<Value>();
    add_submodule<Value>("root").set_default("Wrapper");
    add_input<int>("x").set_default(1).set_target("Leaf", "x");
}
DRIVER_RUNS_ROOT(XDriver)

/// XDriver, but the setting is required
DECLARE_DRIVER(RequiredXDriver);
inline DRIVER_CTOR(RequiredXDriver) {
    satisfies_property_type<Value>();
    add_submodule<Value>("root").set_default("Wrapper");
    add_input<int>("x").set_default(1).set_required_target("Leaf", "x");
}
DRIVER_RUNS_ROOT(RequiredXDriver)

/// Targets a module the ModuleManager does not have
DECLARE_DRIVER(UnknownModuleDriver);
inline DRIVER_CTOR(UnknownModuleDriver) {
    satisfies_property_type<Value>();
    add_submodule<Value>("root").set_default("Wrapper");
    add_input<int>("x").set_default(1).set_target("Not a module", "x");
}
DRIVER_RUNS_ROOT(UnknownModuleDriver)

/// Targets an input "Leaf" does not have
DECLARE_DRIVER(UnknownInputDriver);
inline DRIVER_CTOR(UnknownInputDriver) {
    satisfies_property_type<Value>();
    add_submodule<Value>("root").set_default("Wrapper");
    add_input<int>("x").set_default(1).set_target("Leaf", "Not an input");
}
DRIVER_RUNS_ROOT(UnknownInputDriver)

/// Chooses "Adder"'s "b" and "offset" with its "mode"
DECLARE_DRIVER(ChainDriver);
inline DRIVER_CTOR(ChainDriver) {
    satisfies_property_type<Value>();
    add_submodule<Value>("root").set_default("Wrapper");
    add_input<std::string>("mode").set_default("plain");
    add_input<int>("offset").set_default(0);

    when("mode", "plain")
      .set_submod("Adder", "b", "Other leaf")
      .otherwise("mode", "double")
      .set_submod("Adder", "b", "Leaf")
      .forward_input("Adder", "offset", "offset")
      .otherwise()
      .set_input("Adder", "offset", 100);
}
DRIVER_RUNS_ROOT(ChainDriver)

/// Forwards its submodule "b" to "Adder"'s "b"
DECLARE_DRIVER(SubmodDriver);
inline DRIVER_CTOR(SubmodDriver) {
    satisfies_property_type<Value>();
    add_submodule<Value>("root").set_default("Wrapper");
    add_submodule<Value>("b").set_target("Adder", "b");
}
DRIVER_RUNS_ROOT(SubmodDriver)

/// Turns memoization off, except for "Leaf"
DECLARE_DRIVER(MemoDriver);
inline DRIVER_CTOR(MemoDriver) {
    satisfies_property_type<Value>();
    add_submodule<Value>("root").set_default("Wrapper");
    wire_memoization(false, {"Leaf"});
}
DRIVER_RUNS_ROOT(MemoDriver)

/// Configures "X driver" through its input "x"
DECLARE_DRIVER(OuterDriver);
inline DRIVER_CTOR(OuterDriver) {
    satisfies_property_type<Value>();
    add_submodule<Value>("root").set_default("X driver");
    add_input<int>("x").set_default(7).set_target("X driver", "x");
}
DRIVER_RUNS_ROOT(OuterDriver)

/// Targets "Leaf", which is only below the nested "X driver"
DECLARE_DRIVER(EncapsulationDriver);
inline DRIVER_CTOR(EncapsulationDriver) {
    satisfies_property_type<Value>();
    add_submodule<Value>("root").set_default("X driver");
    add_input<int>("x").set_default(7).set_target("Leaf", "x");
}
DRIVER_RUNS_ROOT(EncapsulationDriver)

/// Sets "Leaf"'s "x" twice, under two keys which name the same module
DECLARE_DRIVER(SameModuleDriver);
inline DRIVER_CTOR(SameModuleDriver) {
    satisfies_property_type<Value>();
    add_submodule<Value>("root").set_default("Wrapper");
    add_input<int>("x").set_default(1).set_target("Leaf", "x");
    add_input<int>("y").set_default(2).set_target({"", "Leaf"}, "x");
}
DRIVER_RUNS_ROOT(SameModuleDriver)

/// Forwards its submodule "b" to "Adder"'s "b", only if "swap" is true
DECLARE_DRIVER(BranchSubmodDriver);
inline DRIVER_CTOR(BranchSubmodDriver) {
    satisfies_property_type<Value>();
    add_submodule<Value>("root").set_default("Wrapper");
    add_input<bool>("swap").set_default(false);
    add_submodule<Value>("b").set_default("Other leaf");
    when("swap", true).forward_submod("Adder", "b", "b");
}
DRIVER_RUNS_ROOT(BranchSubmodDriver)

/// Forwards "x" before it learns "x" is an input of a property type
DECLARE_DRIVER(LatePTInputDriver);
inline DRIVER_CTOR(LatePTInputDriver) {
    satisfies_property_type<Value>();
    add_submodule<Value>("root").set_default("Wrapper");
    add_input<int>("x").set_default(1).set_target("Leaf", "x");
    satisfies_property_type<ValueOfX>();
}
DRIVER_RUNS_ROOT(LatePTInputDriver)

/// An input which shares its value, so tests can see who holds copies of it
struct SharedInt {
    std::shared_ptr<int> value;
    bool operator==(const SharedInt& rhs) const noexcept {
        return value == rhs.value;
    }
    bool operator!=(const SharedInt& rhs) const noexcept {
        return !(*this == rhs);
    }
};

/// Returns the value of its input "x"
DECLARE_MODULE(PtrLeaf);
inline MODULE_CTOR(PtrLeaf) {
    satisfies_property_type<Value>();
    add_input<SharedInt>("x");
}
inline MODULE_RUN(PtrLeaf) {
    auto rv = results();
    return Value::wrap_results(rv, *inputs.at("x").value<SharedInt>().value);
}

/// Forwards its "x" to "Ptr leaf", which does not memoize
DECLARE_DRIVER(PtrDriver);
inline DRIVER_CTOR(PtrDriver) {
    satisfies_property_type<Value>();
    add_submodule<Value>("root").set_default("Ptr leaf");
    add_input<SharedInt>("x").set_target("Ptr leaf", "x");
    wire_memoization(false);
}
DRIVER_RUNS_ROOT(PtrDriver)

#undef DRIVER_RUNS_ROOT

/// Makes the call graph above, plus the drivers
inline pluginplay::ModuleManager make_mm() {
    Leaf::calls      = 0;
    OtherLeaf::calls = 0;

    pluginplay::ModuleManager mm(nullptr);
    mm.add_module<Leaf>("Leaf");
    mm.add_module<OtherLeaf>("Other leaf");
    mm.add_module<Adder>("Adder");
    mm.change_submod("Adder", "a", "Leaf");
    mm.change_submod("Adder", "b", "Other leaf");
    mm.add_module<Wrapper>("Wrapper");
    mm.change_submod("Wrapper", "inner", "Adder");

    mm.add_module<XDriver>("X driver");
    mm.add_module<RequiredXDriver>("Required X driver");
    mm.add_module<UnknownModuleDriver>("Unknown module driver");
    mm.add_module<UnknownInputDriver>("Unknown input driver");
    mm.add_module<ChainDriver>("Chain driver");
    mm.add_module<SubmodDriver>("Submod driver");
    mm.add_module<MemoDriver>("Memo driver");
    mm.add_module<OuterDriver>("Outer driver");
    mm.add_module<EncapsulationDriver>("Encapsulation driver");
    mm.add_module<SameModuleDriver>("Same module driver");
    mm.add_module<BranchSubmodDriver>("Branch submod driver");
    return mm;
}

/// Runs the module @p key of @p mm as Value
inline int run(pluginplay::ModuleManager& mm, const std::string& key) {
    return mm.run_as<Value>(key);
}

} // namespace testing::drivers
