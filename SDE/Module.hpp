#pragma once
#include "SDE/Memoization.hpp"
#include <Utilities/Containers/CaseInsensitiveMap.hpp>
#include <Utilities/SmartEnum.hpp>
#include <memory> // for shared_ptr and unique_ptr

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

    /// Type of te metadata stored in this class
    using metadata_type = std::map<MetaProperty, std::string>;

    /// Type of the submodule call backs
    using submodule_list = Utilities::CaseInsensitiveMap<module_pointer>;

    /// Type of the set of traits
    using traits_type = std::set<ModuleTraits>;

    /**
     * @brief Creates a new ModuleBase
     *
     * The resulting ModuleBase will be default initialized and thus will
     * contain no parameters, submodules, or meta-data.  Module developers are
     * encouraged to fill said data in via the derived class's ctor.
     *
     * @throws std::bad_alloc if there is insufficient memory to create a
     *         ModuleBasePIMPL instance.  Strong throw guarantee.
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
    const submodule_list& submodules() const noexcept { return submodules_; }
    const metadata_type& metadata() const noexcept { return metadata_; }
    const traits_type& traits() const noexcept { return traits_; }
    // const Parameters& parameters() const noexcept {return parameters_;}

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

    /*template<typename parameter_type>
    void change_parameter(const std::string& key, parameter_type value) {
        if(locked_) throw std::runtime_error("Module is locked!!!");
        parameters_.at(key) = value;
    }*/

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

protected:
    /// Allows Python trampoline to get at data
    friend class detail_::PyModuleBase;

    /// The list of submodules this module may call
    submodule_list submodules_;

    /// The meta-data associated with this module
    metadata_type metadata_;

    /// The traits associated with the module
    std::set<ModuleTraits> traits_;

private:
    friend class detail_::MMImpl;
    /// Used when making a duplicate module
    ModuleBase& operator=(const ModuleBase& rhs) = default;

    // True means parameters and submodules can no longer be changed
    bool locked_ = false;
};

} // namespace SDE

#define DEFINE_MODULE_TYPE(prop_name, return_value, ...) \
    struct prop_name : SDE::ModuleBase {                 \
        virtual return_value run(__VA_ARGS__) = 0;       \
    }
