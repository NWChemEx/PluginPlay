#pragma once
#include "SDE/ModuleBase.hpp"

namespace SDE {
namespace detail_ {

/** @brief The class that actually contains a module's state.
 *
 *  Design Points:
 *  - The actual list of submodules, inputs, and results a module will use are
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
    using input_map  = typename Module::input_map;
    using result_map = typename Module::result_map;
    using submod_map = typename Module::submod_map;
    using base_ptr   = std::shared_ptr<const ModuleBase>;
    using cache_type = std::map<std::string, result_map>;
    using cache_ptr  = std::shared_ptr<cache_type>;

    ModulePIMPL()                       = default;
    ModulePIMPL(const ModulePIMPL& rhs) = default;
    ModulePIMPL& operator=(const ModulePIMPL& rhs) = default;
    ModulePIMPL(ModulePIMPL&& rhs)                 = default;
    ModulePIMPL& operator=(ModulePIMPL&& rhs) = default;

    ModulePIMPL(base_ptr base, cache_ptr cache = cache_ptr{}) :
      locked_(false),
      base_(base),
      cache_(cache),
      inputs_(base->inputs()),
      submods_(base->submods()) {}

    ~ModulePIMPL() = default;

    auto run(input_map ps) {
        lock();
        type::hasher h(bphash::HashType::Hash128);
        base_->memoize(h, ps, submods_);
        auto hv = bphash::hash_to_string(h.finalize());
        if(cache_ && cache_->count(hv)) return cache_->at(hv);
        auto rv = base_->run(std::move(ps), submods_);
        if(!cache_) return rv;
        cache_->emplace(hv, std::move(rv));
        return cache_->at(hv);
    }

    void hash(type::hasher& h) const {
        if(!ready()) throw std::runtime_error("Can't hash non-ready module.");
        base_->memoize(h, inputs_, submods_);
    }

    /** @brief Function for determining whether the instance is ready.
     *
     * A module is ready if it contains an algorithm (determined by member
     * `base` being non-null), all required inputs are set, and all submods are
     * set to modules that are ready. This function ensures that all of those
     * criteria are met.
     *
     * @return True if the module is ready and false otherwise
     * @throw none No throw guarantee.
     */
    bool ready() const noexcept {
        if(!base_) return false;
        for(const auto & [k, v] : inputs_)
            if(!v.is_ready()) return false;
        for(const auto & [k, v] : submods_)
            if(!v.value().ready()) return false;
        return true;
    }

    bool is_cached(const input_map& in_inputs, const submod_map& in_submods) {
        if(!cache_) return false;
        type::hasher h(bphash::HashType::Hash128);
        base_->memoize(h, in_inputs, in_submods);
        auto hv = bphash::hash_to_string(h.finalize());
        return cache_->count(hv);
    }
    void lock() {
        if(!ready()) throw std::runtime_error("Can't lock non-ready module.");
        for(auto & [k, v] : submods_) v.lock();
        locked_ = true;
    }

    auto problems() const {
        Utilities::CaseInsensitiveMap<std::set<type::key>> probs;
        if(!base_) probs["Algorithm"];
        for(const auto & [k, v] : inputs_)
            if(!v.is_ready()) probs["Inputs"].insert(k);
        for(const auto & [k, v] : submods_)
            if(!v.ready()) probs["Submodules"].insert(k);
        return probs;
    }

    input_map& inputs() { return inputs_; }
    submod_map& submods() { return submods_; }
    const result_map& results() const { return base_->results(); }
    bool is_locked() const noexcept { return locked_; }

private:
    bool locked_;
    base_ptr base_;
    cache_ptr cache_;
    input_map inputs_;
    submod_map submods_;
};
} // namespace detail_
} // namespace SDE
