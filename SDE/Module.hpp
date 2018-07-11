#pragma once
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
    /// Type of a pointer to this class
    using module_pointer = std::shared_ptr<ModuleBase>;

    /// Type of the metadata stored in this class
    using metadata_type = std::map<MetaProperty, std::string>;

    /// Type of the submodule call backs
    using submodule_list = Utilities::CaseInsensitiveMap<module_pointer>;

    /// Type of the set of traits
    using traits_type = std::set<ModuleTraits>;

    /// Type returned by not_ready (no support for multimap in Python)
    using not_ready_return =
      std::vector<std::pair<module_pointer, ModuleProperty>>;

    /**
     * @brief Creates a new ModuleBase
     *
     * The resulting ModuleBase will be default initialized and thus will
     * contain no parameters, submodules, or meta-data.  Module developers are
     * encouraged to fill said data in via the derived class's ctor.
     *
     * @throws None. No throw guarantee.
     *
     * @par Complexity:
     * Constant.
     *
     * @par Data Races:
     * None.
     */
    ModuleBase() = default;

    /**
     * @brief Frees up memory associated with the current module.
     *
     * @par Complexity:
     * Linear in the number of submodules, and parameters.
     *
     * @par Data Races:
     * The contents of the current instance are modified and data races may
     * result if the instance is concurrently accessed.
     *
     * @throws None No throw guarantee.
     */
    virtual ~ModuleBase() noexcept = default;

    /// Deleted to avoid slicing, ModuleBase should always be passed as pointers
    ///@{
    ModuleBase(const ModuleBase& rhs) = delete;

    ModuleBase(ModuleBase&& rhs) noexcept = delete;

    ModuleBase& operator=(ModuleBase&& rhs) noexcept = delete;
    ///@}

    /**
     * @name Accessors
     * @brief Functions in this section provide read-only access to the
     *        module's state.
     *
     * @return The requested submodules, metadata, or parameters objects in a
     *         read-only state.
     *
     * @throw None. All functions are no-throw guarantee.
     *
     * @par Complexity:
     * Constant.
     *
     */
    ///@{
    const metadata_type& metadata() const noexcept { return metadata_; }
    const submodule_list& submodules() const noexcept { return submodules_; }
    const traits_type& traits() const noexcept { return traits_; }
    const Parameters& parameters() const noexcept { return parameters_; }

    /**
     * @name Modifiers
     *
     * @brief The functions in this section allow users to modify the parameters
     *        and submodules so long as the module has not been locked yet.
     *
     * @param[in] key The name of the parameter/submodule callback point you
     *            want to change the value of.
     * @param[in] value the new parameter/submodule to use.
     *
     * @throw std::runtime_error if the module is locked.  Strong throw
     *        guarantee.
     * @throw std::range_error if @p key is not a valid key.
     *
     * @par Complexity:
     * Constant.
     *
     * @par Data Races:
     * The content of the module is modified and data races may ensue if the
     * module is concurrently accessed.
     */
    ///@{
    void change_submodule(const std::string& key, module_pointer value) {
        if(locked_) throw std::runtime_error("Module is locked!!!");
        submodules_.at(key) = value;
    }

    template<typename T>
    void change_parameter(const std::string& key, T value) {
        if(locked_) throw std::runtime_error("Module is locked!!!");
        parameters_.change(key, value);
    }

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
     * @note It is not straightforward to expose the hasher to Python;
     *       consequentially, we instead internally perform the hash and return
     *       the value as a string.
     *
     * @return The hash value of the module, as a string.
     *
     * @par Complexity:
     * Linear in both the number of parameters and the number of submodules.
     * This assumes that all modules contain a ModuleInfo instance of about the
     * same size.
     *
     * @par Data Races:
     * The state of this module's algorithm, as well as the state of all
     * submodules are accessed.  Data races may result if the states of this
     * module or any of its submodules are concurrently modified.
     *
     * @throw ??? if any of the parameters' or submodules' hash functions throw.
     * Same guarantee as the parameters' and/or submodules' hash functions.
     */
    void hash(Hasher& h) const { h(submodules_, metadata_); }

    /**
     * @brief Locks the module and all submodules.
     *
     * A module that is locked can no longer have its parameters or submodules
     * changed.
     *
     * @throw None. No throw guarantee.
     *
     * @par Complexity:
     * Linear in the number of submodules. Constant on all subsequent calls.
     *
     * @par Data Races:
     * The state of the current module is modified and data races may occur if
     * the module is concurrently accessed.
     */
    void lock() noexcept {
        if(locked_) return;
        for(auto& x : submodules_)
            if(x.second) x.second->lock();
        locked_ = true;
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
        auto hv             = memoize(std::forward<Args>(args)...);
        using return_type   = detail_::RunDetails_return_type<PropertyType>;
        using shared_return = std::shared_ptr<return_type>;
        // check cache for hv
        shared_return result;
        if(true) {
            return_type rv = impl->run(std::forward<Args>(args)...);
            result         = std::make_shared<return_type>(std::move(rv));
            // put in cache
        }
        // Get result from cache and return
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

     * @tparam Args The types of the arguments to memoize.
     * @param[in] args the arguments to memoize.
     * @return True if the result is cached and false otherwise.
     * @throws None. No throw guarantee.
     */
    template<typename... Args>
    bool is_cached(Args&&...) const noexcept {
        return false;
    }

protected:
    /// Allows Python trampoline to get at data
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
     * This function wraps the retreival of the module and the forwarding of
     * the arguments to that module.
     *
     */
    template<typename PropertyType, typename... Args>
    auto call_submodule(const std::string& key, Args&&... args) {
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
