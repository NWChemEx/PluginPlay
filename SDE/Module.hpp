#pragma once
#include "SDE/Memoization.hpp"
#include <Utilities/Containers/CaseInsensitiveMap.hpp>
#include <memory> // for shared_ptr and unique_ptr

namespace SDE {

/**
 *   @brief A list of physical, hardware, and software resources.
 *
 *   The Resource enum is largely intended to be used to select which resource
 *   the ModuleBaseImpl::cost returns the cost of.
 */
enum class Resource { time, memory, disk, processes, threads };

/**
 *  @brief This is the class that all modules will be passed around as.
 *
 *  To the outside world the ModuleBase class is little more than a type-erased
 *  handle to the actual module.  To the module itself, the ModuleBase class
 *  provides the module its state.  Notably, this state includes the Cache
 *  instance, the algorithmic parameters selected by the user, and the API to
 *  the framework (and notably the parallel resources).
 */
class ModuleBase {
public:
    /**
     * @brief A function that allows us to get the RTTI of the module's type
     *        without having to downcast to it.
     *
     * This function is implemented in ModuleBaseImpl<T> so that it returns the
     * RTTI of T.
     *
     * @return The RTTI of the module type's type.
     */
    virtual const std::type_info& type() const noexcept = 0;

    /**
     * @brief Provides a hash for the current module's state.
     *
     * For memoization we need two things: the input and algorithm that was
     * called.  This function provides a hash of the latter.  In particular this
     * function will hash the parameters to this module, the submodules this
     * module will call, and the module info.  The combination of the above
     * data should uniquely define the algorithm.  More specifically what we
     * are saying with this hash is that if there exists another module, with
     * the same parameters, list of submodules, and module info, then for a
     * given input this module and the other module will generate the same
     * result.
     *
     * @param[in, out] h The hasher to use.  The state of this module will be
     * hashed into the hasher.
     *
     * @par Complexity:
     * Linear in both the number of parameters and the number of submodules.
     * This assumes that all modules contain a ModuleInfo instance of about the
     * same size.
     *
     * @par Data Races:
     * The state of this module's algorithm, as well as the state of all
     * submodules are accessed.  Data races may result if the states of this
     * module or any of its submodules are concurrently modified.  The state of
     * @p h is modified and data races may result if its state is concurrently
     * accessed or modified.
     *
     * @throw ??? if any of the parameters' or submodules' hash functions throw.
     * Same guarantee as the parameters' and/or submodules' hash functions.
     */
    void hash(Hasher& h) const { h(submodules_); }

protected:
    /// Typedef of a pointer to a module via its base class
    using ModuleBasePtr = std::unique_ptr<ModuleBase>;

    // TODO: Add when SDE is written
    /// A handle to the framework
    // std::shared_ptr<SDE> sde_;

    // TODO: Add when Cache is written
    /// A collection of results previously obtained by this module type
    // std::shared_ptr<Cache> benjamins_;

    // TODO: Add when Parameters are written
    /// Values for the algorithmic parameters associated with the module
    // std::shared_ptr<const Parameters> params_;

    /// Submodules to be called by the module
    Utilities::CaseInsensitiveMap<ModuleBasePtr> submodules_;
};

/**
 * @brief The class all module types inherit from.
 *
 * The ModuleBaseImpl class is primarily responsible for registering the APIs
 * of modules with the SDE (and doing so in a uniform manner).  Because it is
 * the most basic class of the module hierarchy with the full signature, it also
 * is responsible for memoization.
 *
 * @tparam ModuleType the type of the module type.
 * @tparam ReturnType the type of the object returned by the module.
 *         Must satisfy copyable.
 * @tparam Args the types of the arguments to the module's run_ function.
 */
template<typename ModuleType, typename ReturnType, typename... Args>
class ModuleBaseImpl : public ModuleBase {
public:
    /// The type of the property computed by this module
    using return_type = ReturnType;

    /// The type of the property computed by the module, as a shared pointer
    using shared_return = std::shared_ptr<const return_type>;

    /// The type of a cost as computed by the cost function
    using cost_type = std::size_t;

    /**
     * @brief The API used to actually call the module.
     *
     * This is the API to the module as seen by users of the SDE.  It wraps the
     * actual call to the module ensuring that memoization occurs.
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
    shared_return operator()(Args... args) {
        auto hv = memoize(std::forward<Args>(args)...);
        // check cache for hv
        shared_return result;
        if(true) {
            result = std::make_shared<return_type>(
              std::move(run_(std::forward<Args>(args)...)));
            // put in cache
        }
        return result;
    }

    /**
     * @brief Returns the RTTI of the module.
     *
     * This function returns the RTTI of the module type **NOT** the type of
     * the implementation.  For example assume you implement `MyGreatAlgorithm`
     * by deriving from `GreatAlgorithm` (which in turn inherited from
     * `ModuleBaseImpl<GreatAlgorithm>`) this function would return the RTTI of
     * `GreatAlgorithm` **NOT* the RTTI of `MyGreatAlgorithm`.
     *
     * @return The RTTI of the module type's type.
     *
     * @par Complexity:
     * Constant.
     *
     * @par Data Races:
     * None.
     *
     * @throw None. No throw guarantee.
     */
    const std::type_info& type() const noexcept override {
        return typeid(ModuleType);
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
    virtual cost_type cost(Resource r, Args... args) {
        return std::numeric_limits<cost_type>::max();
    }

protected:
    /**
     * @brief This is the function that should be implemented by the derived
     *        class to provide functionality.
     *
     * @note We do not want to put the && after @p Args because the user gave
     *       us the arguments fully decorated and the && would trigger perfect
     *       forwarding, which (somewhat ironically) will put additional
     *       references on the arguments.
     *
     * @param[in] args The input to the module.
     * @return  The result of running this module
     */
    virtual return_type run_(Args... args) = 0;

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
    virtual HashValue memoize(Args... args) const {
        Hasher h(HashType::Hash128);
        h(*this, std::forward<Args>(args)...);
        return h.finalize();
    }
};

template<typename ModuleType>
class PropertyBase {
public:
    /// The type of a pointer to a ModuleBase
    using ModuleBasePtr = std::unique_ptr<ModuleBase>;

    /// The type of the computed property
    using return_type = typename ModuleType::return_type;

    PropertyBase(ModuleBasePtr&& base) :
      impl_(std::move(downcast(std::move(base)))) {}

    template<typename... Args>
    auto operator()(Args&&... args) {
        return (*impl_)(std::forward<Args>(args)...);
    }

private:
    /// Type of a pointer to the module type's API
    using ModuleBaseImplPtr = std::unique_ptr<ModuleType>;

    /**
     * @brief Code factorization for downcasting from the ModuleBase to the
     *        actual implementation.
     *
     * @par Complexity:
     * Constant.
     *
     * @par Data Race:
     * This call modifies both the current instance and @p ptr.  Data
     * races may ensure if either the current instance or @p ptr are
     * concurrently accessed.
     *
     * @param[in] ptr The instance which implements the property algorithm
     *            passed via the base class.
     * @return  The same instance as @p ptr, downcasted to
     * @throws std::bad_cast if the module implementation pointed to by @p ptr
     * is not of type @p ModuleBaseImplType.  Strong throw guarantee.
     * @throws ??? if ModuleBaseImplType's move ctor throws.  Same guarantee as
     * ModuleBaseImplType's move ctor.
     */
    ModuleBaseImplPtr downcast(ModuleBasePtr&& ptr) const {
        if(ptr->type() != typeid(ModuleType)) throw std::bad_cast();
        auto& derived = static_cast<ModuleType&>(std::move(*ptr));
        return std::move(std::make_unique<ModuleType>(derived));
    }

    /// The actual implementation to use
    ModuleBaseImplPtr impl_;
};

} // namespace SDE
