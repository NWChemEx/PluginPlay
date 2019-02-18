#pragma once
#include "SDE/ModuleBase.hpp"
#include "SDE/Types.hpp"

namespace SDE {
namespace detail_ {

/** @brief The class that actually contains a module's state.
 *
 *  This class contains a module's actual state in the sense that whenever the
 *  module is called the values in this class are taken to be the bound values
 *  for the inputs. Of particular not is it is these values, and not the
 *  values in the developer-provided ModuleBase instance, that are used.
 *
 */
class ModulePIMPL {
public:
    /// Type used to store the developer-provided implementation
    using base_ptr = std::shared_ptr<const ModuleBase>;

    /// Type of the object used to store the results computed by the module
    using cache_type = std::map<std::string, type::result_map>;

    /// How we store the object used for caching
    using cache_ptr = std::shared_ptr<cache_type>;

    /// How we tell you what's preventing your module from running
    using not_set_type = Utilities::CaseInsensitiveMap<std::set<type::key>>;

    ///@{
    /** @name Ctor and assignment operators
     *
     * Of note copies deep copy inputs and submodules, the pointer to the base
     * and the pointer to the cache are aliased. This is because the base is
     * simply an algorithm whose state can't change over the run of a
     * calculation, and the cache is assigned based on the base.
     *
     * @param rhs The instance to copy/move from. For move operations @p rhs
     *        will be in a valid, but otherwise undefined state.
     * @param base The algorithm that this Module is wrapping.
     * @param cache Where computed results should be stored.
     * @throw std::bad_alloc 1, 2, 3, and 6 throw if there is insufficient
     *        memory to copy the state over or make the default state (1 only).
     *        Strong throw guarantee.
     * @throw non 1, 4, and 5 are no throw guarantee.
     */
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
    ///@}

    /// Standard dtor
    ~ModulePIMPL() = default;

    /// Computes the hash of the current instance using the bound parameters
    void hash(type::hasher& h) const { memoize(h, inputs_); }

    /** @brief computes a hash for a particular invocation of the `run` member.
     *
     * For a deterministic module providing the module the same inputs must
     * return the same outputs. We need a way to determine if we have already
     * called the module with a particular set of inputs; that's where this
     * function comes in. This function takes a set of input values, as well as
     * the set of submodules to use, and maps them to a hash value. Barring the
     * universe conspiring against us, that hash value is a concise and unique
     * representation of the input state.
     *
     * @param h The hasher instance to use
     * @param inputs The values of the inputs to hash
     *
     */
    void memoize(type::hasher& h, type::input_map inputs) const {
        inputs = merge_inputs_(std::move(inputs));
        for(const auto & [k, v] : inputs) v.hash(h);
        for(const auto & [k, v] : submods_) v.hash(h);
    }

    /** @brief Checks whether the result of a call is cached.
     *
     * @param in_inputs The inputs to use in the memoization
     * @return
     */
    bool is_cached(const type::input_map& in_inputs) {
        if(!cache_) return false;
        auto ps = merge_inputs_(in_inputs);
        return cache_->count(get_hash_(ps)) == 1;
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

    /// Returns whether or not the module is locked
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
    not_set_type not_set(
      const type::input_map& in_inputs = type::input_map{}) const {
        auto inps = merge_inputs_(in_inputs);
        not_set_type probs;
        if(!base_) probs["Algorithm"];
        auto in_probs = not_set_guts_(inps);
        if(in_probs.size()) probs.emplace("Inputs", std::move(in_probs));
        auto submod_probs = not_set_guts_(submods_);
        if(submod_probs.size())
            probs.emplace("Submodules", std::move(submod_probs));
        return probs;
    }

    /// Locks the module
    void lock() {
        if(!ready()) throw std::runtime_error("Can't lock a non-ready module");
        for(auto & [k, v] : submods_) v.lock();
        locked_ = true;
    }

    /// Unlocks the module
    void unlock() noexcept { locked_ = false; }

    ///@{
    /** @name Returns the state of the module
     *
     * @return The requested piece of the module's state
     */
    type::input_map& inputs() { return inputs_; }
    type::submodule_map& submods() { return submods_; }
    const type::result_map& results() const {
        if(!base_) throw std::runtime_error("Algorithm is not set");
        return base_->results();
    }
    ///@}

    /** @brief Actually runs the module
     *
     * @param ps The input parameters set by the user, they are not merged with
     *        the bound inputs yet.
     * @return Whatever the module returns.
     */
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
        const bool lptr = static_cast<bool>(base_);
        const bool rptr = static_cast<bool>(rhs.base_);
        if(lptr != rptr) return false;
        if(std::tie(locked_, inputs_, submods_) !=
           std::tie(rhs.locked_, rhs.inputs_, rhs.submods_))
            return false;
        if(lptr && rptr)
            if(*base_ != *rhs.base_) return false;
        return true;
    }

    bool operator!=(const ModulePIMPL& rhs) const { return !((*this) == rhs); }
    ///@}
private:
    /// Code factorization for merging an input_map with the inputs in this
    /// class
    type::input_map merge_inputs_(type::input_map in_inputs) const {
        for(const auto & [k, v] : inputs_)
            if(!in_inputs.count(k)) in_inputs[k] = v;
        return in_inputs;
    }

    /// Code factorization for computing the hash of a module
    std::string get_hash_(const type::input_map& in_inputs) {
        type::hasher h(bphash::HashType::Hash128);
        memoize(h, in_inputs);
        return bphash::hash_to_string(h.finalize());
    }

    /// Code factorization for checking if things in a map are ready
    template<typename T>
    std::set<type::key> not_set_guts_(T&& map) const {
        std::set<type::key> probs;
        for(const auto & [k, v] : map)
            if(!v.ready()) probs.insert(k);
        return probs;
    }

    ///@{
    /** @name Module state
     *
     *  The members in this section
     */
    bool locked_ = false;
    base_ptr base_;
    cache_ptr cache_;
    type::input_map inputs_;
    type::submodule_map submods_;
};
} // namespace detail_
} // namespace SDE
