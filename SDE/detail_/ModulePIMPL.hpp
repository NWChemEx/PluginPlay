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
      m_base_(base),
      m_cache_(cache),
      m_inputs_(base->inputs()),
      m_submods_(base->submods()) {}
    ///@}

    /// Standard dtor
    ~ModulePIMPL() = default;

    /// Computes the hash of the current instance using the bound parameters
    void hash(type::hasher& h) const { memoize(h, m_inputs_); }

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
        for(const auto & [k, v] : m_submods_) v.hash(h);
    }

    /** @brief Checks whether the result of a call is cached.
     *
     * @param in_inputs The inputs to use in the memoization
     * @return
     */
    bool is_cached(const type::input_map& in_inputs) {
        if(!m_cache_) return false;
        auto ps = merge_inputs_(in_inputs);
        return m_cache_->count(get_hash_(ps)) == 1;
    }

    /// Returns whether or not the module is locked
    bool locked() const noexcept { return m_locked_; }

    /** @brief Determines if module is ready to be run
     *
     * This function wraps `not_set` and simply asserts that no problems come
     * back.
     *
     * @param inps The inputs to provide to `not_set`
     * @return True if the module is ready and false otherwise
     *
     * @throw std::bad_alloc if `not_set` throws. Strong throw guarantee.
     */
    bool ready(const type::input_map& inps = type::input_map{}) const {
        auto errors = not_set(inps);
        return errors.size() == 0;
    }

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
     *  Note that since some inputs will be provided via the property type it is
     *  possible for the module to be ready even if all the bound inputs are
     *  not. For this reason this function takes an optional list of inputs.
     *  These inputs are assumed to be part of the property type's API.
     *
     * @return A map of module state that is not set yet.
     * @param in_inputs The set of inputs provided by the property type.
     *        Defaults to an empty map. Values are not checked for readyness.
     * @throw std::bad_alloc if there is insufficient memory to allocate the
     *        returned object. Strong throw guarantee.
     */
    not_set_type not_set(
      const type::input_map& in_inputs = type::input_map{}) const {
        not_set_type probs;
        if(!m_base_) probs["Algorithm"];

        // This is all of the not ready inputs
        auto in_probs = not_set_guts_(m_inputs_);
        // Now pull out those set by the property type
        for(const auto & [k, v] : in_inputs)
            if(in_probs.count(k)) in_probs.erase(k);
        if(in_probs.size()) probs.emplace("Inputs", std::move(in_probs));
        auto submod_probs = not_set_guts_(m_submods_);
        if(submod_probs.size())
            probs.emplace("Submodules", std::move(submod_probs));
        return probs;
    }

    /// Locks the module
    void lock() {
        for(auto & [k, v] : m_submods_) v.lock();
        m_locked_ = true;
    }

    /// Unlocks the module
    void unlock() noexcept { m_locked_ = false; }

    ///@{
    /** @name Returns the state of the module
     *
     * @return The requested piece of the module's state
     */
    type::input_map& inputs() { return m_inputs_; }
    type::submodule_map& submods() { return m_submods_; }
    const type::result_map& results() const {
        if(!m_base_) throw std::runtime_error("Algorithm is not set");
        return m_base_->results();
    }
    ///@}

    /** @brief Actually runs the module
     *
     * @param ps The input parameters set by the user, they are not merged with
     *        the bound inputs yet.
     * @return Whatever the module returns.
     */
    auto run(type::input_map ps) {
        for(const auto & [k, v] : ps)
            if(!v.ready()) throw std::runtime_error("Inputs are not ready");
        auto arg_probs = not_set(ps);
        if(!arg_probs.empty())
            throw std::runtime_error("Module is not ready to be run");

        lock();

        ps = merge_inputs_(ps);
        // Check cache
        auto hv = get_hash_(ps);
        if(m_cache_ && m_cache_->count(hv)) return m_cache_->at(hv);

        // not there so run
        auto rv = m_base_->run(std::move(ps), m_submods_);
        if(!m_cache_) return rv;

        // cache result
        m_cache_->emplace(hv, std::move(rv));
        return m_cache_->at(hv);
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
        const bool lptr = static_cast<bool>(m_base_);
        const bool rptr = static_cast<bool>(rhs.m_base_);
        if(lptr != rptr) return false;
        if(std::tie(m_locked_, m_inputs_, m_submods_) !=
           std::tie(rhs.m_locked_, rhs.m_inputs_, rhs.m_submods_))
            return false;
        if(lptr && rptr)
            if(*m_base_ != *rhs.m_base_) return false;
        return true;
    }

    bool operator!=(const ModulePIMPL& rhs) const { return !((*this) == rhs); }
    ///@}
private:
    /// Code factorization for merging an input_map with the inputs in this
    /// class
    type::input_map merge_inputs_(type::input_map in_inputs) const {
        for(const auto & [k, v] : m_inputs_)
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
    bool m_locked_ = false;
    base_ptr m_base_;
    cache_ptr m_cache_;
    type::input_map m_inputs_;
    type::submodule_map m_submods_;
};
} // namespace detail_
} // namespace SDE
