#include "property_types.hpp"
#include <pluginplay/module_base.hpp>
#include <pluginplay/module_manager.hpp>

namespace test_pluginplay {

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
DECLARE_MODULE(ReadyModule);
inline MODULE_CTOR(ReadyModule) { satisfies_property_type<OptionalInput>(); }
inline MODULE_RUN(ReadyModule) {
    auto [opt1] = OptionalInput::unwrap_inputs(inputs);
    auto rv     = results();
    return OptionalInput::wrap_results(rv, opt1);
}

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

template<typename T>
auto make_module() {
    pluginplay::ModuleManager mm(nullptr);
    mm.add_module<T>("a_mod");
    return std::make_shared<pluginplay::Module>(mm.at("a_mod"));
}

} // namespace test_pluginplay
