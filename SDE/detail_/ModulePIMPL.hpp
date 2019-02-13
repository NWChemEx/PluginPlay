#pragma once
#include "SDE/ModuleBase.hpp"
#include "SDE/Types.hpp"

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
 *    - Avoids modules having "hidden variables" that live in the derived class
 *
 *
 */
class ModulePIMPL {
public:
    using base_ptr   = std::shared_ptr<const ModuleBase>;
    using cache_type = std::map<std::string, type::result_map>;
    using cache_ptr  = std::shared_ptr<cache_type>;

    ModulePIMPL()                       = default;
    ModulePIMPL(const ModulePIMPL& rhs) = default;
    ModulePIMPL& operator=(const ModulePIMPL& rhs) = default;
    ModulePIMPL(ModulePIMPL&& rhs)                 = default;
    ModulePIMPL& operator=(ModulePIMPL&& rhs) = default;

    explicit ModulePIMPL(base_ptr base, cache_ptr cache = cache_ptr{}) :
      base_(base),
      cache_(cache),
      inputs_(base->inputs()),
      submods_(base->submods()) {}

    ~ModulePIMPL() = default;

    void hash(type::hasher& h) const {
        if(!ready()) throw std::runtime_error("Can't hash non-ready module.");
        base_->memoize(h, inputs_, submods_);
    }

    /** @brief Checks whether the result of a call is cached.
     *
     * @param in_inputs The inputs
     * @return
     */
    bool is_cached(const type::input_map& in_inputs) {
        if(!cache_) return false;
        return cache_->count(get_hash_(in_inputs)) == 1;
    }

    /** @brief Function for determining whether the instance is ready to run.
     *
     * A module is ready if it contains an algorithm (determined by member
     * `base` being non-null), all submodules requests are satisfied, and
     * if all of the submodules are ready. Of important note this definition
     * does not include all inputs being set. This is because callers of the
     * module can pass the not-set inputs to the module when they call `run`.
     * Consequentially, it is `run`'s responsibility to make sure all
     * non-optional inputs are set (for the present module; submodules need to
     * check this during their `run` function for the same reason this module
     * does).
     *
     * @return True if the module is ready and false otherwise
     * @throw none No throw guarantee.
     */
    bool ready() const noexcept {
        if(!base_) return false;
        for(const auto & [k, v] : submods_)
            if(!v.ready()) return false;
        return true;
    }

    bool locked() const noexcept { return locked_; }

    /** @brief Returns a list of module state that is not "ready"
     *
     *  Calling `ready` is an easy way to determine if the module's `run` member
     *  can be called; however, if the module is not ready it can be a bit of a
     *  pain to figure out why. This function will return a map containing the
     *  reasons why the module is not ready. The keys of the map describe the
     *  part of the module that is not ready. Choices are:
     *
     *  - "Algorithm" to indicate that this ModulePIMPL does not contain an
     *    algorithm
     *  - "Inputs" to indicate that one or more required inputs have not been
     *    set
     *  - "Submodules" to indicate that one or more submodules have not been
     *  set.
     *
     *  For inputs and submodules, the value in the map is the set of
     *  input/submodule keys corresponding to the inputs/submodules that are
     *  not set yet.
     *
     *
     * @return A map of module state that is not set yet.
     * @param in_inputs The set of inputs to check for readiness.
     * @throw std::bad_alloc if there is insufficent memory to allocate the
     *        returned object. Strong throw guarantee.
     */
    auto not_set(const type::input_map& in_inputs = type::input_map{}) const {
        auto inps = merge_inputs_(in_inputs);
        Utilities::CaseInsensitiveMap<std::set<type::key>> probs;
        if(!base_) probs["Algorithm"];
        auto in_probs = not_set_guts_(inps);
        if(in_probs.size()) probs.emplace("Inputs", std::move(in_probs));
        auto submod_probs = not_set_guts_(submods_);
        if(submod_probs.size())
            probs.emplace("Submodules", std::move(submod_probs));
        return probs;
    }

    void lock() {
        if(!ready()) throw std::runtime_error("Can't lock a non-ready module");
        for(auto & [k, v] : submods_) v.lock();
        locked_ = true;
    }

    void unlock() noexcept { locked_ = false; }

    type::input_map& inputs() { return inputs_; }
    type::submodule_map& submods() { return submods_; }
    const type::result_map& results() const {
        if(!base_) throw std::runtime_error("Algorithm is not set");
        return base_->results();
    }

    auto run(type::input_map ps) {
        ps = merge_inputs_(std::move(ps));

        // Ensure we're ready to run, including required inputs
        auto arg_probs = not_set(ps);
        if(!arg_probs.empty())
            throw std::runtime_error("Module is not ready to be run");

        lock();

        // Check cache
        auto hv = get_hash_(ps);
        if(cache_ && cache_->count(hv)) return cache_->at(hv);

        // not there so run
        auto rv = base_->run(std::move(ps), submods_);
        if(!cache_) return rv;

        // cache result
        cache_->emplace(hv, std::move(rv));
        return cache_->at(hv);
    }

    ///@{
    /** @name Equivalence comparisons
     *
     * Two modules are equivalent if they contain the same algorithm (determined
     * by comparing the `base` member), they have the same bound inputs, and
     * they have the same set of submodules.
     *
     * @param rhs The instance to compare against
     * @return True if the comparison is true and false otherwise.
     * @throws ??? If the base comparison throws or if any of the input
     *         comparisons throw. Same guarantee.
     */
    bool operator==(const ModulePIMPL& rhs) const {
        return std::tie(locked_, base_, inputs_, submods_) ==
               std::tie(rhs.locked_, rhs.base_, rhs.inputs_, rhs.submods_);
    }

    bool operator!=(const ModulePIMPL& rhs) const { return !((*this) == rhs); }
    ///@}
private:
    type::input_map merge_inputs_(type::input_map in_inputs) const {
        for(const auto & [k, v] : inputs_)
            if(!in_inputs.count(k)) in_inputs[k] = v;
        return in_inputs;
    }
    std::string get_hash_(const type::input_map& in_inputs) {
        type::hasher h(bphash::HashType::Hash128);
        base_->memoize(h, in_inputs, submods_);
        return bphash::hash_to_string(h.finalize());
    }

    template<typename T>
    std::set<type::key> not_set_guts_(T&& map) const {
        std::set<type::key> probs;
        for(const auto & [k, v] : map)
            if(!v.ready()) probs.insert(k);
        return probs;
    }

    bool locked_ = false;
    base_ptr base_;
    cache_ptr cache_;
    type::input_map inputs_;
    type::submodule_map submods_;
};
} // namespace detail_
} // namespace SDE
