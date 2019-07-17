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

// Property type for module with one option
struct OneIn : sde::PropertyType<OneIn> {
    auto inputs_() { return sde::declare_input().add_field<int>("Option 1"); }
    auto results_() { return sde::declare_result(); }
};

// Property type for module with one result
struct OneOut : sde::PropertyType<OneOut> {
    auto inputs_() { return sde::declare_input(); }
    auto results_() { return sde::declare_result().add_field<int>("Result 1"); }
};

////////////////////////////////////////////////////////////////////////////////
//////////                                                            //////////
//////////                      Modules                               //////////
//////////                                                            //////////
////////////////////////////////////////////////////////////////////////////////

// Module with no property type
struct NoPTModule : sde::ModuleBase {
    NoPTModule() : sde::ModuleBase(this) {}
    sde::type::result_map run_(sde::type::input_map,
                               sde::type::submodule_map) const override {
        return results();
    }
};

// Module with just a property type
struct NullModule : sde::ModuleBase {
    NullModule() : sde::ModuleBase(this) { satisfies_property_type<NullPT>(); }
    sde::type::result_map run_(sde::type::input_map,
                               sde::type::submodule_map) const override {
        return results();
    }
};

// Module with a description "A description"
struct DescModule : sde::ModuleBase {
    DescModule() : sde::ModuleBase(this) {
        satisfies_property_type<NullPT>();
        description("A description");
    }
    sde::type::result_map run_(sde::type::input_map,
                               sde::type::submodule_map) const override {
        return results();
    }
};

// Module with a citation "A citation"
struct CiteModule : sde::ModuleBase {
    CiteModule() : sde::ModuleBase(this) {
        satisfies_property_type<NullPT>();
        citation("A citation");
    }
    sde::type::result_map run_(sde::type::input_map,
                               sde::type::submodule_map) const override {
        return results();
    }
};

// A module with int input "Option 1"
struct NotReadyModule : sde::ModuleBase {
    NotReadyModule() : sde::ModuleBase(this) {
        satisfies_property_type<OneIn>();
    }
    sde::type::result_map run_(sde::type::input_map,
                               sde::type::submodule_map) const override {
        return results();
    }
};

// Has property type int input "Option 1" and another int input "Option 2"
struct NotReadyModule2 : sde::ModuleBase {
    NotReadyModule2() : sde::ModuleBase(this) {
        satisfies_property_type<OneIn>();
        add_input<int>("Option 2");
    }
    sde::type::result_map run_(sde::type::input_map,
                               sde::type::submodule_map) const override {
        return results();
    }
};

// One int result "Result 1" set to 4
struct ResultModule : sde::ModuleBase {
    ResultModule() : sde::ModuleBase(this) {
        satisfies_property_type<OneOut>();
    }
    sde::type::result_map run_(sde::type::input_map,
                               sde::type::submodule_map) const override {
        auto rv = results();
        return OneOut::wrap_results(rv, int{4});
    }
};

// Submodule "Submodule 1" of property type NullPT
struct SubModModule : sde::ModuleBase {
    SubModModule() : sde::ModuleBase(this) {
        satisfies_property_type<NullPT>();
        add_submodule<NullPT>("Submodule 1");
    }
    sde::type::result_map run_(sde::type::input_map,
                               sde::type::submodule_map) const override {
        return results();
    }
};

// Module using every feature
struct RealDeal : sde::ModuleBase {
    RealDeal() : sde::ModuleBase(this) {
        satisfies_property_type<NullPT>();
        satisfies_property_type<OneIn>();
        satisfies_property_type<OneOut>();

        description("This module is the real deal. It does math stuff like:\n\n"
                    ".. math::\n\n"
                    "   \\sum_{i=0}^N i = \\frac{N(N+1)}{2}\n\n"
                    "Okay it's not that cool...");

        citation("A. Person. *The Best Article*. A Journal "
                 "You Have Never Heard Of. 1 (2008).");
        citation("B. Person. *A So-So Article*. A Journal Everyone Has Heard"
                 "Of. 1 (2009).");
    }
    sde::type::result_map run_(sde::type::input_map,
                               sde::type::submodule_map) const override {
        return results();
    }
};

// Wraps the creation of a module pimpl w/o going through a module manager
template<typename T>
auto make_module_pimpl() {
    auto ptr = std::make_shared<T>();
    return sde::detail_::ModulePIMPL(ptr);
}

// Wraps the creation of a module w/o going through a module manager
template<typename T>
auto make_module() {
    return std::make_shared<sde::Module>(
      std::make_unique<sde::detail_::ModulePIMPL>(make_module_pimpl<T>()));
}

} // namespace testing
