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

#pragma once
#include "unit_testing_pts.hpp"
#include <pluginplay/detail_/module_pimpl.hpp>
#include <pluginplay/module.hpp>
#include <pluginplay/module_base.hpp>
#include <pluginplay/utility/uuid.hpp>

namespace testing {

////////////////////////////////////////////////////////////////////////////////
//////////                                                            //////////
//////////                      Modules                               //////////
//////////                                                            //////////
////////////////////////////////////////////////////////////////////////////////

// Module with no property type
DECLARE_MODULE(NoPTModule);
inline MODULE_CTOR(NoPTModule) {}
inline MODULE_RUN(NoPTModule) { return results(); }

// Module with just a property type
DECLARE_MODULE(NullModule);
inline MODULE_CTOR(NullModule) { satisfies_property_type<NullPT>(); }
inline MODULE_RUN(NullModule) { return results(); }

// Module with a description "A description"
DECLARE_MODULE(DescModule);
inline MODULE_CTOR(DescModule) {
    satisfies_property_type<NullPT>();
    description("A description");
}
inline MODULE_RUN(DescModule) { return results(); }

// Module with a citation "A citation"
DECLARE_MODULE(CiteModule);
inline MODULE_CTOR(CiteModule) {
    satisfies_property_type<NullPT>();
    citation("A citation");
}
inline MODULE_RUN(CiteModule) { return results(); }

// Module which takes a polymorphic input
DECLARE_MODULE(PolymorphicModule);
inline MODULE_CTOR(PolymorphicModule) {
    satisfies_property_type<PolymorphicOptions>();
}
inline MODULE_RUN(PolymorphicModule) { return results(); }

// A module with int input "Option 1"
DECLARE_MODULE(NotReadyModule);
inline MODULE_CTOR(NotReadyModule) { satisfies_property_type<OneIn>(); }
inline MODULE_RUN(NotReadyModule) { return results(); }

// A module with a defaulted int option
struct ReadyModule : pluginplay::ModuleBase {
    ReadyModule() : pluginplay::ModuleBase(this) {
        satisfies_property_type<OptionalInput>();
    }
    pluginplay::type::result_map run_(
      pluginplay::type::input_map inputs,
      pluginplay::type::submodule_map) const override {
        auto [opt1] = OptionalInput::unwrap_inputs(inputs);
        auto rv     = results();
        return OptionalInput::wrap_results(rv, opt1);
    }
};

// Has property type int input "Option 1" and another int input "Option 2"
DECLARE_MODULE(NotReadyModule2);
inline MODULE_CTOR(NotReadyModule2) {
    satisfies_property_type<OneIn>();
    add_input<int>("Option 2");
}
inline MODULE_RUN(NotReadyModule2) { return results(); }

// One int result "Result 1" set to 4
DECLARE_MODULE(ResultModule);
inline MODULE_CTOR(ResultModule) { satisfies_property_type<OneOut>(); }
inline MODULE_RUN(ResultModule) {
    auto rv = results();
    return OneOut::wrap_results(rv, int{4});
}

// Submodule "Submodule 1" of property type NullPT
DECLARE_MODULE(SubModModule);
inline MODULE_CTOR(SubModModule) {
    satisfies_property_type<NullPT>();
    add_submodule<NullPT>("Submodule 1");
}
inline MODULE_RUN(SubModModule) { return results(); }

// Module using every feature
DECLARE_MODULE(RealDeal);
inline MODULE_CTOR(RealDeal) {
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
inline MODULE_RUN(RealDeal) {
    auto rv = results();
    return OneOut::wrap_results(rv, int{4});
}

// Wraps the creation of a module pimpl w/o going through a module manager
template<typename T>
auto make_module_pimpl() {
    auto ptr = std::make_shared<T>();
    return pluginplay::detail_::ModulePIMPL(ptr);
}

// Wraps the creation of a module pimpl (with a cache) w/o going through a
// module manager
template<typename T>
auto make_module_pimpl_with_cache() {
    auto ptr = std::make_shared<T>();
    pluginplay::cache::ModuleManagerCache m_caches;
    auto pcache = m_caches.get_or_make_module_cache("foo");
    ptr->set_uuid(pluginplay::utility::generate_uuid());
    return pluginplay::detail_::ModulePIMPL(ptr, pcache);
}

// Wraps the creation of a module w/o going through a module manager
template<typename T>
auto make_module() {
    return std::make_shared<pluginplay::Module>(
      std::make_unique<pluginplay::detail_::ModulePIMPL>(
        make_module_pimpl<T>()));
}

// Wraps the creation of a module (with a cache) w/o going through a module
// manager
template<typename T>
auto make_module_with_cache() {
    return std::make_shared<pluginplay::Module>(
      std::make_unique<pluginplay::detail_::ModulePIMPL>(
        make_module_pimpl_with_cache<T>()));
}

} // namespace testing
