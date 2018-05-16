#pragma once
#include "SDE/Memoization.hpp"
#include "SDE/ModuleHelpers.hpp"
#include <Utilities/Containers/CaseInsensitiveMap.hpp>
#include <Utilities/SmartEnum.hpp>
#include <memory> // for shared_ptr and unique_ptr

namespace SDE {
namespace detail_ {
// Forward declarations of pimpl classes
class ModuleBasePIMPL;
} // namespace detail_

/**
 *   @brief A list of physical, hardware, and software resources.
 *
 *   The Resource enum is largely intended to be used to select which resource
 *   the ModuleBaseImpl::cost returns the cost of.
 */
DECLARE_SmartEnum(Resource, time, memory, disk, processes, threads);

/**
 *  @brief Enumerations for the various meta data types associated with a
 *         module.
 *
 *  The MetaProperty enumeration is meant as a way to associate important meta
 *  data with a module.  At the moment, recognized meta data includes:
 *
 *  - name: A descriptive name for the module
 *  - version: Information that can be used to uniquely identify the state of
 *             the module's source code.
 *  - description: An informative discourse of what the module is capable of
 *  - authors: The people who wrote the module
 *  - citations: Things that should be cited if the module is used.
 *
 */
DECLARE_SmartEnum(MetaProperty, name, version, description, authors, citations);

/**
 *  @brief This is the class that all modules will be passed around as.
 *
 *  ModuleBase is the opaque handle to a module that is usable with the SDE.
 *
 *  @nosubgrouping
 */
class ModuleBase {
public:
    /// Type of a pointer to this class
    using module_pointer = std::shared_ptr<ModuleBase>;

    /// Type of meta-data stored in this class
    using meta_data_type = std::map<MetaProperty, std::string>;

    /// Type of the list of submodules stored in this class
    using submodules_list = Utilities::CaseInsensitiveMap<module_pointer>;

    ModuleBase();
    ModuleBase(const ModuleBase& rhs) = delete;
    ModuleBase& operator=(const ModuleBase& rhs) = delete;
    ModuleBase(ModuleBase&& rhs) noexcept        = delete;
    ModuleBase& operator=(ModuleBase&& rhs) noexcept = delete;
    virtual ~ModuleBase() noexcept;

    /**
     * @brief Swaps the contents of the current instance with that of @p rhs.
     *
     * This function only swaps the contents of the ModuleBase class.  Derived
     * classes will need to additionally add their own swaps to the pool.
     *
     * @param rhs The ModuleBase instance to swap with.
     *
     * @throw None. No throw guarantee.
     *
     * @par Complexity:
     * Constant.
     *
     * @par Data Races:
     * The state of both this module and @p rhs are modified.  Data races may
     * occur if either this instance or @p rhs are concurrently modified.
     */
    void swap(ModuleBase& rhs) noexcept;

    /**
     * @name Accessors
     *
     * @brief The functions in this section allow read-only access to a module's
     *        state.
     *
     *
     */
    ///@{
    const submodules_list& submodules() const noexcept;
    ///@}

    /**
     * @brief A function that allows us to get the RTTI of the module's type
     *        without having to downcast to it.
     *
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
    void hash(Hasher& h) const;

private:
    /// The object that actually holds the implementation
    std::unique_ptr<detail_::ModuleBasePIMPL> pimpl_;
};

/**
 * @brief Convenience class for code factorization.
 *
 * Implementing a module involves a lot of boiler-plate.  This class is
 * designed in an effort to circumvent some of it.
 *
 *
 * @tparam ModuleType the type of the derived module.
 */
template<typename ModuleType>
struct ModuleBaseImpl : ModuleBase {
    using module_pointer = typename ModuleBase::module_pointer;

    const std::type_info& type() const noexcept override {
        return typeid(ModuleType);
    }
};

/**
 *  @brief Class responsible for requesting a property be computed.
 *
 *  Generally speaking we expect the PropertyBase to be used in typedefs,
 *  for example to define a class capable of returning property "Property"
 *  by calling a module of type "PropertyImpl" would be something like:
 *
 *  @code
 *  using Property = PropertyBase<PropertyImpl>;
 *  @endcode
 *
 *  Nonethless, it is conceivable that users may want to inherit from this
 *  class.  For this reason we have made the dtor virtual to ensure
 *  polymorphism works correctly.
 *
 *  @tparam ModuleType The module type which is capable of computing a property.
 *          Should satisfy the concept of module type.
 *
 */
template<typename ModuleType>
class PropertyBase {
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

    /// The type of the hash returned by memoization
    using hash_type = HashValue;

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
    PropertyBase(module_pointer mod) : impl_(downcast(mod)) {}

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
    hash_type memoize(Args&&... args) const {
        // TODO: dispatch to ModuleType if it defines a memoize fxn
        Hasher h(HashType::Hash128);
        // h(*this, std::forward<Args>(args)...);
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

    /**
     *  @brief Cleans up the PropertyBase instance.
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
    virtual ~PropertyBase() = default;

private:
    /// The type of the derived class implementing the module
    using derived_ptr = std::shared_ptr<ModuleType>;

    /// Wrapper around the cast to ensure it succeeds
    derived_ptr downcast(module_pointer ptr) {
        if(ptr->type() != typeid(ModuleType)) throw std::bad_cast{};
        return std::static_pointer_cast<ModuleType>(ptr);
    }

    /// The actual implementation to use
    derived_ptr impl_;
};

} // namespace SDE

#define DEFINE_PROPERTY(prop_name, return_value, ...)             \
    struct prop_name##API : SDE::ModuleBaseImpl<prop_name##API> { \
        virtual return_value run(__VA_ARGS__) = 0;                \
    };                                                            \
    using prop_name = SDE::PropertyBase<prop_name##API>
