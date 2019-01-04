#pragma once
#include "SDE/Module.hpp"
#include "SDE/ModuleBase.hpp"

namespace SDE {
namespace detail_ {

/** @brief The class that actually contains a module's state.
 *
 *  Design Points:
 *  - The actual list of submodules, inputs, and outputs a module will use are
 *    stored in this class
 *    - The ones returned from ModuleBase are the defaults specified by
 *      the developer and are used to populate the original lists
 *  - Once a ModuleBase has been constructed there's no way to change it
 *    - Avoids contaminating the developer's defaults
 *    - Avoids modules having "hidden variables"
 *
 */
class ModulePIMPL {
public:
    using output_type   = typename Module::output_type;
    using output_map    = typename Module::output_map;
    using input_type    = typename Module::input_type;
    using input_map     = typename Module::input_map;
    using submodule_map = typename Module::submodule_map;

    ModulePIMPL(module_base_ptr base) :
      inputs(std::move(base->inputs())),
      outputs(std::move(base->outputs())),
      submodule_map(std::move(base->)),
      cache_(cache),
      pimpl(base) {}

    /**
     *
     * @param inputs
     * @return
     * @throw std::invalid_argument if any of the parameters
     */
    auto run(input_map inputs) {
        // Make sure all the inputs are valid
        for(auto & [k, v] : inputs)
            if(!inputs_.count(k))
                throw std::invalid_argument("Unrecognized input: " + k);

        // Swap inputs into inputs_

        // Hash inputs and submodules
        if(!cache_->count(hash)) {
            auto output = pimpl_->run(inputs_, submodules_);
            cache_->insert(hash, output);
        }
        // Swap original values

        // return cached value
        return cache_->at(hash);
    }

private:
    input_map inputs_;
    output_map outputs_;
    submodule_map submodules_;
    std::shared_ptr<Cache> cache_;
    std::shared_ptr<const ModuleBase> pimpl_;
};
} // namespace detail_
} // namespace SDE
