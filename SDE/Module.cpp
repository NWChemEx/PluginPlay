#include "SDE/Module.hpp"

namespace SDE {

using not_ready_return = typename ModuleBase::not_ready_return;

not_ready_return ModuleBase::not_ready() {
    not_ready_return rv;
    // Check parameters first before recursion in submodules

    for(const auto& si : submodules_) {
        if(!si.second) // Submod isn't set
            rv.push_back(
              std::make_pair(module_pointer{}, ModuleProperty::submodules));
        else if(this != si.second.get()) { // Recurse so long as it's not us
            auto submod_r = si.second->not_ready();
            // Join it with our map taking into account submod nullptr's need
            // replaced
            for(auto x : submod_r)
                rv.push_back(
                  std::make_pair(!x.first ? si.second : x.first, x.second));
        }
    }
    return rv;
};

} // namespace SDE
