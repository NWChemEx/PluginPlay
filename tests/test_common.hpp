#pragma once
#include "sde/detail_/module_pimpl.hpp"
#include <sde/module.hpp>
#include <sde/module_base.hpp>
#include <sde/property_type.hpp>

namespace testing {

// Simplest property type possible
DECLARE_PROPERTY_TYPE(NullPT);
PROPERTY_TYPE_INPUTS(NullPT) { return sde::declare_input(); }
PROPERTY_TYPE_RESULTS(NullPT) { return sde::declare_result(); }

// Property type for module with one option
DECLARE_PROPERTY_TYPE(OneIn);
PROPERTY_TYPE_INPUTS(OneIn) {
    return sde::declare_input().add_field<int>("Option 1");
}
PROPERTY_TYPE_RESULTS(OneIn) { return sde::declare_result(); }


// Property type for module with one result
DECLARE_PROPERTY_TYPE(OneOut);
PROPERTY_TYPE_INPUTS(OneOut) { return sde::declare_input(); }
PROPERTY_TYPE_RESULTS(OneOut) {
    return sde::declare_result().add_field<int>("Result 1");
}

// Property type for module with two results
DECLARE_PROPERTY_TYPE(TwoOut);
PROPERTY_TYPE_INPUTS(TwoOut) { return sde::declare_input(); }
PROPERTY_TYPE_RESULTS(TwoOut) {
    return sde::declare_result()
      .add_field<int>("Result 1")
      .add_field<char>("Result 2");
}


////////////////////////////////////////////////////////////////////////////////
//////////                                                            //////////
//////////                      Modules                               //////////
//////////                                                            //////////
////////////////////////////////////////////////////////////////////////////////

// Module with no property type
DECLARE_MODULE(NoPTModule);
inline MODULE_CTOR(NoPTModule){}
inline MODULE_RUN(NoPTModule, , ) { return results(); }


// Module with just a property type
DECLARE_MODULE(NullModule);
inline MODULE_CTOR(NullModule){ satisfies_property_type<NullPT>(); }
inline MODULE_RUN(NullModule, , ) { return results(); }

// Module with a description "A description"
DECLARE_MODULE(DescModule);
inline MODULE_CTOR(DescModule) {
    satisfies_property_type<NullPT>();
    description("A description");
}
inline MODULE_RUN(DescModule, , ) { return results(); }

// Module with a citation "A citation"
DECLARE_MODULE(CiteModule);
inline MODULE_CTOR(CiteModule) {
    satisfies_property_type<NullPT>();
    citation("A citation");
}
inline MODULE_RUN(CiteModule, ,) { return results(); }


// A module with int input "Option 1"
DECLARE_MODULE(NotReadyModule);
inline MODULE_CTOR(NotReadyModule) { satisfies_property_type<OneIn>(); }
inline MODULE_RUN(NotReadyModule, ,) { return results(); }

// Has property type int input "Option 1" and another int input "Option 2"
DECLARE_MODULE(NotReadyModule2);
inline MODULE_CTOR(NotReadyModule2) {
    satisfies_property_type<OneIn>();
    add_input<int>("Option 2");
}
inline MODULE_RUN(NotReadyModule2, ,){ return results(); }

// One int result "Result 1" set to 4
DECLARE_MODULE(ResultModule);
inline MODULE_CTOR(ResultModule) { satisfies_property_type<OneOut>(); }
inline MODULE_RUN(ResultModule, , ){
    auto rv = results();
    return OneOut::wrap_results(rv, int{4});
}

// Submodule "Submodule 1" of property type NullPT
DECLARE_MODULE(SubModModule);
inline MODULE_CTOR(SubModModule) {
    satisfies_property_type<NullPT>();
    add_submodule<NullPT>("Submodule 1");
}
inline MODULE_RUN(SubModModule, , ) { return results(); }

// Module using every feature
DECLARE_MODULE(RealDeal);
inline MODULE_CTOR(RealDeal){
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
inline MODULE_RUN(RealDeal, , ) { return results(); }

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
