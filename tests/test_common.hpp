#pragma once
#include "sde/detail_/module_pimpl.hpp"
#include <sde/module.hpp>
#include <sde/module_base.hpp>
#include <sde/property_type.hpp>

namespace testing {

// Simplest property type possible
struct NullPT : sde::PropertyType<NullPT> {
    auto inputs_() { return sde::declare_input(); }
    auto results_() { return sde::declare_result(); }
};

// Property type for a module that isn't ready
struct NotReadyPT : sde::PropertyType<NotReadyPT> {
    auto inputs_() { return sde::declare_input().add_field<int>("Option 1"); }
    auto results_() { return sde::declare_result(); }
};

// Simplest module possible, satisfies property type NullPT
struct NullModule : sde::ModuleBase {
    NullModule() : sde::ModuleBase(this) { satisfies_property_type<NullPT>(); }
    sde::type::result_map run_(sde::type::input_map,
                               sde::type::submodule_map) const override {
        return results();
    }
};

// A module that is not ready because it's input is not set.
struct NotReadyModule : sde::ModuleBase {
    NotReadyModule() : sde::ModuleBase(this) {
        satisfies_property_type<NotReadyPT>();
    }
    sde::type::result_map run_(sde::type::input_map,
                               sde::type::submodule_map) const override {
        return results();
    }
};

// A module that is not ready because it's input is not set. Has extra input
struct NotReadyModule2 : sde::ModuleBase {
    NotReadyModule2() : sde::ModuleBase(this) {
        satisfies_property_type<NotReadyPT>();
        add_input<int>("Option 2");
    }
    sde::type::result_map run_(sde::type::input_map,
                               sde::type::submodule_map) const override {
        return results();
    }
};

// Wraps the creation of a module w/o going through a module manager
template<typename T>
auto make_module() {
    auto ptr = std::make_shared<T>();
    return std::make_shared<sde::Module>(
      std::make_unique<sde::detail_::ModulePIMPL>(ptr));
}

} // namespace testing
