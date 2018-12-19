#pragma once
#include "SDE/Cache.hpp"
#include "SDE/Memoization.hpp"
#include "SDE/ModuleHelpers.hpp"
#include "SDE/Parameters.hpp"
#include <Utilities/Containers/CaseInsensitiveMap.hpp>
#include <Utilities/SmartEnum.hpp>
#include <memory> // for shared_ptr and unique_ptr
#include <vector>

namespace SDE {
namespace detail_ {
class PyModuleBase;
class MMImpl;
} // namespace detail_
/**
 *   @brief A list of physical, hardware, and software resources.
 *
 *   The Resource enum is largely intended to be used to select which resource
 *   the ModuleBaseImpl::cost returns the cost of.
 */
DECLARE_SmartEnum(Resource, time, memory, disk, processes, threads

);

/**
 *  @brief Enumerations for the various metadata types associated with a
 *         module.
 *
 *  The MetaProperty enumeration is meant as a way to associate important meta
 *  data with a module.  At the moment, recognized metadata includes:
 *
 *  - name: A descriptive name for the module
 *  - version: Information that can be used to uniquely identify the state of
 *             the module's source code.
 *  - description: An informative discourse of what the module is capable of
 *  - authors: The people who wrote the module
 *  - citations: Things that should be cited if the module is used.
 *
 */
DECLARE_SmartEnum(MetaProperty, name, version, description, authors, citations

);

/**
 * @brief Enumerations pertaining to the characteristics of the module.
 *
 * The moduletraits set of enumerations is envisioned as being one of the ways
 * the SDE, as well as other modules, can obtain information related to the
 * module, without knowing its contents.
 *
 * At the moment the recognized traits include:
 * - nondeterministic: Signals that runs of the same module with the same
 *   algorithmic parameters and the same input may yield different results
 *   (intentionally).  Consequentially memoization should not be performed.
 */
DECLARE_SmartEnum(ModuleTraits, nondeterministic);

/**
 * @brief Enumeration of the various types of state held in the Module
 *
 * This enumeration is primarily meant to be used with the "not_ready" member of
 * the ModuleBase class to signal, which members are not ready.
 */
DECLARE_SmartEnum(ModuleProperty, metadata, parameters, submodules, traits);

/**
 *  @brief This is the class that all modules will be passed around as.
 *
 *  ModuleBase is the opaque handle to a module that is usable with the SDE. As
 *  a handle much of their functionality revolves around exploiting
 *  polymorphism.  In an effort to prevent the user from slicing a module, we
 *  have deleted the copy and move ctors as well as the corresponding assignment
 *  operators.  We expect modules to always be wrapped in shared pointers (I'd
 *  prefer unique pointers, but that's not a viable option for modules that go
 *  round-trip through Python (*i.e.* a C++ module returned into Python, passed
 *  back into C++) because Python (or at least Pybind11) won't give up
 *  ownership.
 *
 *  @nosubgrouping
 *
 */
class ModuleBase {
public:
    using submodule_map = Utilities::CaseInsensitiveMap<Module>;

    Parameters& params();
    const Parameters& params() const {
        return const_cast<Module &>(*this).params();
    }


    /**
     * @brief Can be used to inquire into whether or not the module is currently
     *        locked.
     *
     * @return True if the module is locked and false otherwise.
     *
     * @throws None. No throw guarantee.
     *
     * @par Complexity:
     * Constant.
     *
     * @par Data Races:
     * The state of the current instance will be accessed and concurrent
     * modifications may cause a data race.
     */
    bool locked() const noexcept { return locked_; }

    /**
     * @brief Used to determine if the current module is ready to be run.
     *
     * A module is ready to be run if all required parameters are set and all
     * submodule call back points are assigned a submodule to call (*i.e.*
     * `submodules_["Some call back point"] != nullptr`).  This check will
     * proceed recursively to ensure that all submodules are also ready.
     *
     * @return A list of pairs such that the first element of the pair is a
     * pointer to the module that is not ready, and the second element is one of
     * the ModuleProperty enumerations corresponding to why the module is not
     * ready.
     * @note In order to use enable_shared_from_this we need to ensure that
     * every module is wrapped in shared pointers.  If one is not, we get
     * undefined behavior.  This is difficult to enforce without hiding all the
     * ctors, which in turn makes it hard to write loaders.  Point is if it is
     * the current module that is not ready the key will be a nullptr.
     * @throw std::bad_alloc if there is insufficient memory to add elements to
     *        the return structure.  Strong throw guarantee.
     * @par Complexity:
     * Linear in the total number of submodules.
     */
    not_ready_return not_ready();

    template<typename PropertyType, typename... Args>
    auto run_as(Args&&... args) {
        PropertyType* impl = downcast<PropertyType>();
        if(impl->not_ready().size())
            throw std::runtime_error("Module is not ready");
        lock();
        using return_type   = detail_::RunDetails_return_type<PropertyType>;
        using shared_return = std::shared_ptr<return_type>;
        valKey_             = bphash::hash_to_string(memoize(std::forward<Args>(args)...));
        shared_return result;
	    /* Disable memoization until we have a differentiate hashes based on the module instance
        if(cache_ptr_ && cache_ptr_->count(valKey_))
            result = cache_ptr_->at<return_type>(valKey_);
        else
        {
	*/
            return_type rv = impl->run(std::forward<Args>(args)...);
            //If cache exists, add results
            if(cache_ptr_)
            {
                cache_ptr_->insert(valKey_, rv);
                result = cache_ptr_->at<return_type>(valKey_);
            }
            else
            result         = std::make_shared<return_type>(std::move(rv));
    //    }
        return result;
    };

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
        hash(h);
        h(std::forward<Args>(args)...);
        return h.finalize();
    }

    /**
     * @brief Determines if the result of calling the module with the provided
     * arguments is cached
     *
     * @tparam Args The types of the arguments to memoize.
     * @param[in] args the arguments to memoize.
     * @return True if the result is cached and false otherwise.
     * @throws None. No throw guarantee.
     */
    template<typename... Args>
    bool is_cached(Args&&... args) const noexcept {
        auto key = bphash::hash_to_string(memoize(std::forward<Args>(args)...));
        return(cache_ptr_ && cache_ptr_->count(key));
    }

    /**
     * @brief Sets the Cache that will be used by the module for storage and/or
     * retrieval of computed results
     *
     * @param[in] args Shared pointer to the Cache
     * @throws None. No throw guarantee.
     */
    void set_cache(std::shared_ptr<Cache> cp ) {cache_ptr_=cp;}

    /**
     * @brief Retrieves a shared_ptr to the Cache that is used by the module for
     * storage and/or retrieval of computed results
     *
     * @param[out] args Shared pointer to the Cache
     * @throws None. No throw guarantee.
     */
    std::shared_ptr<Cache> get_cache() {return cache_ptr_;}

    template<typename... Args>
    std::pair<std::string, std::string> make_node(Args&&... args) const {
        auto valKey = bphash::hash_to_string(memoize(std::forward<Args>(args)...));
        auto modKey = bphash::hash_to_string(memoize());
        return std::make_pair(valKey, modKey);
    }



protected:
    /// Allows Python helper class to get at data
    friend class detail_::PyModuleBase;

    /// The meta-data associated with this module
    metadata_type metadata_;

    /// The list of submodules this module may call
    submodule_list submodules_;

    /// The traits associated with the module
    std::set<ModuleTraits> traits_;

    /// The parameters associated with this module
    Parameters parameters_;

    /**
     * @brief Convenience function for calling a submodule.
     *
     * This function wraps the retrieval of the module and the forwarding of
     * the arguments to that module.
     *
     */
    template<typename PropertyType, typename... Args>
    auto call_submodule(const std::string& key, Args&&... args) {
	    if(cache_ptr_)
	    {
        // Record adjacency in module invocation graph
        auto hashes = submodules_.at(key)->make_node(std::forward<Args>(args)...);
        cache_ptr_->add_node(valKey_, hashes);
	// Forward our Cache to the submodule. Should probably be handled by ModuleManager.
	submodules_.at(key)->set_cache(cache_ptr_);
	    }
        return submodules_.at(key)->run_as<PropertyType>(
          std::forward<Args>(args)...);
    }

private:
    /// Allows ModuleManager to copy the module
    friend class detail_::MMImpl;

    /// Used when making a duplicate module
    ModuleBase& operator=(const ModuleBase& rhs) = default;

    // True means parameters and submodules can no longer be changed
    bool locked_ = false;

    std::shared_ptr<Cache> cache_ptr_;

    // Holds hash of Module state and input. Used to create a module graph
    // node if submodules are called.
    std::string valKey_;

    template<typename PropertyType>
    PropertyType* downcast() {
        PropertyType* new_ptr = dynamic_cast<PropertyType*>(this);
        if(!new_ptr) throw std::bad_cast();
        return new_ptr;
    }
};

} // namespace SDE

/** @brief Macro to factor out boilerplate for defining a property type
 *
 *  @param[in] prop_name Name to be used for resulting class
 *  @param[in] return_value Type of the return
 *  @param[in] __VA_ARGS__ The types of the inputs to the module.
 */
#define DEFINE_PROPERTY_TYPE(prop_name, return_value, ...) \
    struct prop_name : SDE::ModuleBase {                   \
        virtual return_value run(__VA_ARGS__) = 0;         \
    }
