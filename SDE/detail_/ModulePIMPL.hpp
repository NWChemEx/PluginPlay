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
    using input_type  = typename Module::input_type;
    using input_map   = typename Module::input_map;
    using output_type = typename Module::output_type;
    using output_map  = typename Module::output_map;
    using submod_type = typename Module::submod_type;
    using submod_map  = typename Module::submod_map;
    using base_type   = ModuleBase;
    using base_ptr    = std::shared_ptr<ModuleBase>;
    using cache_type  = std::map<std::string, output_map>;
    using cache_ptr   = std::shared_ptr<cache_type>;

    ModulePIMPL()                       = default;
    ModulePIMPL(const ModulePIMPL& rhs) = default;
    ModulePIMPL& operator=(const ModulePIMPL& rhs) = default;
    ModulePIMPL(ModulePIMPL&& rhs)                 = default;
    ModulePIMPL& operator=(ModulePIMPL&& rhs) = default;
    ModulePIMPL(base_ptr base, cache_ptr cache = cache_ptr{}) :
      base_(base),
      cache_(cache),
      inputs_(std::move(base->inputs())),
      submods_(std::move(base->submods())) {}
    ~ModulePIMPL() = default;

    output_map run(input_map ps) const {
        bphash::Hasher h(bphash::HashType::Hash128);
        base_->memoize(h, ps, submods_);
        auto hv = bphash::hash_to_string(h.finalize());
        if(cache_ && cache_->count(hv)) return cache_->at(hv);
        auto rv = base_->run(std::move(ps), submods_);
        if(!cache_) return rv;

        // cache_ is internal detail so okay to break const
        std::const_pointer_cast<cache_type>(cache_)->emplace(hv, std::move(rv));

        return cache_->at(hv);
    }

    void hash(bphash::Hasher& h) const { base_->memoize(h, inputs_, submods_); }

    input_map& inputs() { return inputs_; }
    submod_map& submods() { return submods_; }
    const output_map& outputs() const { return base_->outputs(); }

private:
    base_ptr base_;
    cache_ptr cache_;
    input_map inputs_;
    submod_map submods_;
};
} // namespace detail_
} // namespace SDE
