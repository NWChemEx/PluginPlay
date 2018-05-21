#pragma once
#include "SDE/Module.hpp"
#include "SDE/ModuleHelpers.hpp"

namespace SDE {
/**
 *  @brief Class responsible for requesting a property be computed.
 *
 *
 *  @tparam ModuleType The module type which is capable of computing a property.
 *          Should satisfy the concept of module type.
 *
 */
template<typename ModuleType>
class Property {
public:
    /// The type of the module's API
    using module_type = ModuleType;

    /// The type of the computed property
    using return_type = detail_::RunDetails_return_type<ModuleType>;

    /// The type of a shared_pointer to the computed property
    using shared_return = std::shared_ptr<const return_type>;

    /// The type of the arguments to run_
    using args_type = detail_::RunDetails_args_type<ModuleType>;

    /// The type of a pointer to a ModuleBase
    using module_pointer = typename ModuleBase::module_pointer;

    /// The type of a cost as computed by the cost function
    using cost_type = std::size_t;

    /**
     * @brief Creates a class capable of computing a given property.
     *
     * @param mod A module that implements an algorithm capable of computing a
     *            given property.
     *
     * @throws std::bad_cast if @p mod is not derived from @p ModuleType.
     *         Strong throw guarantee.
     *
     * @par Complexity:
     * Constant.
     */
    Property(module_pointer mod) : impl_(downcast(mod)) { impl_->lock(); }

    /**
     *  @brief Cleans up the Property instance.
     *
     *  Note the dtor is virtual to allow for polymorphism.
     *
     *  @par Complexity:
     *  Constant.  The state of the held module implementation is stored in
     *  shared_ptr's retained by the SDE.  This deallocation simply decrements
     *  the reference counts.
     *
     *  @par Data Races:
     *  The state of the current instance is modified and data races may occur
     *  if it is concurrently modified or accessed.
     *
     *  @throw None. No throw guarantee.
     */
    virtual ~Property() = default;

    /**
     * @brief The API used to actually call the module.
     *
     * This is the API to the module as seen by users of the SDE.  It wraps the
     * actual call to the module ensuring that memoization occurs.
     *
     *
     * @param[in] args the arguments that will be forwarded to the module.
     *
     * @par Complexity:
     * The greater of the cost: to hash this module or to run this module.
     *
     * @par Data Races:
     * The state of this module will be accessed and modified, thus data races
     * may ensue if this module is concurrently modified or accessed.
     *
     * @return a shared pointer to the result.
     *
     * @throws ??? If the module throws. Strong throw guarantee so long as
     *         @p args is read-only.  Otherwise guarantee depends on the called
     *         module.
     *
     */
    template<typename... Args>
    shared_return operator()(Args&&... args) {
        auto hv = memoize(std::forward<Args>(args)...);
        // check cache for hv
        shared_return result;
        if(true) {
            auto rv = impl_->run(std::forward<Args>(args)...);
            result  = std::make_shared<return_type>(std::move(rv));
            // put in cache
        }
        return result;
    }

    /**
     * @brief This function provides the hash value associated with a specific
     *        input for memoization purposes.
     *
     * This is a default memoization implementation.  It hashes the algorithm
     * and the input values.  Module developers are free to override this
     * behavior to, say, additionally hash the SDE instance or anything else
     * that may influence the value of the results.
     *
     * @note We do not want to put the && after @p Args because the user gave
     *       us the arguments fully decorated and the && would trigger perfect
     *       forwarding, which (somewhat ironically) will put additional
     *       references on the arguments.
     *
     * @param[in] args The input to the module.
     * @return  A hash value that depends on both the current state of the
     *          algorithm as well as the input.
     * @throws ??? If hashing any of the input arguments fails.  Strong throw
     *         guarantee.
     * @par Complexity:
     *      The greater of the complexity to hash the algorithm's state or
     *      linear in the number of arguments.
     * @par Data Races:
     *      The state of the arguments as well as the module are accessed. Thus
     *      data races may result if either state is concurrently modified.
     */
    template<typename... Args>
    HashValue memoize(Args&&... args) const {
        Hasher h(HashType::Hash128);
        impl_->hash(h);
        h(std::forward<Args>(args)...);
        return h.finalize();
    }

    /**
     * @brief Allow introspection
     *
     * The default implementation simply returns the maximum of the type used
     * for the cost.  Modules are encouraged to override this function so that
     * the SDE can better manage resources.
     *
     * @param[in] r The resource for which we want the cost.
     * @param[in] args The arguments to pass to the module.
     * @return The cost of calling the module with the provided arguments.  The
     *         units of the returned cost depend on the resource selected.
     */
    template<typename... Args>
    cost_type cost(Resource r, Args&&...) {
        // TODO: dispatch to ModuleType if it defines a cost fxn
        return std::numeric_limits<cost_type>::max();
    }

private:
    /// The type of the derived class implementing the module
    using derived_ptr = std::shared_ptr<ModuleType>;

    /// Wrapper around the cast to ensure it succeeds
    derived_ptr downcast(module_pointer ptr) {
        auto new_ptr = std::dynamic_pointer_cast<ModuleType>(ptr);
        if(!new_ptr) throw std::bad_cast{};
        return new_ptr;
    }

    /// The actual implementation to use
    derived_ptr impl_;
};

} // namespace SDE
