#pragma once
#include <Utilities/Containers/CaseInsensitiveMap.hpp>
#include <memory> // for shared_ptr and unique_ptr

namespace SDE {
namespace detail_ {

/// This function exists solely so we can get the return type of a function
template<typename ReturnType,typename ClassType, typename...Args>
ReturnType call_fxn_return(ReturnType(ClassType::*)(Args...)) {}

/// This function exists solely so we can get the arguments of a function
template<typename ReturnType, typename ClassType, typename...Args>
std::tuple<Args...> call_fxn_args(ReturnType(ClassType::*)(Args...)){}


/**
 * @brief This is a haphazard version of std::result_of that doesn't need the
 * arguments.
 *
 * This class contains a member typedef @p type, which is the type of the value
 * returned from ModuleType::run_(Args...).  It assumes that ModuleType does not
 * overload the run_ function.
 *
 * If you want to know how this works:
 * 1. We define a function call_fxn which takes a function pointer and returns a
 *    return type (the type of which it figures out from its own signature)
 * 2. We simulate calling call_fxn when it is given a function pointer to
 *    ModuleType::run_()
 * 3. decltype then tells us the return type of this simulated call, which we
 *    know is also the return type of the actual call
 *
 * @tparam ModuleType The module type whose return we wish to know.
 */
template<typename ModuleType>
struct ExtractReturnType{
    /// The type of the result of calling ModuleType::run_(Args...).
    using type = decltype(call_fxn_return(&ModuleType::run_));
};

/**
 * @brief This struct allows us to get the types of the arguments to the run_
 * function.
 *
 * This class contains a member typedef @p type, which is a tuple filled with
 * the types of the arguments to ModuleType::run_(Args...).  It assumes that
 * ModuleType does not overload the run_ function.
 *
 * @tparam ModuleType The module type whose arguments we wish to know.
 */
template<typename ModuleType>
struct ExtractArgTypes{
    /// The type of the arguments to ModuleType::run_(Args...) in a tuple
    using type = decltype(call_fxn_args(&ModuleType::run_));
};

} // namespace detail_


class ModuleBase {
public:

    /**
     * @brief A function that allows us to get the RTTI of the module's type
     *        without having to downcast to it.
     *
     * This function is implemented in ModuleImpl<T> so that it returns the RTTI
     * of T.
     *
     * @return The RTTI of the module type's type.
     */
    virtual const std::type_info& type()const noexcept = 0;

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

template<typename ModuleType>
class ModuleImpl : public ModuleBase {
public:
    /// The type of the property computed by the module
    using return_type =
      typename detail_::ExtractReturnType<ModuleType>::return_type;

    template<typename...Args>
    return_type operator()(Args&&...args) {



        auto result = ModuleType{}.run_(std::forward<Args>(args)...);
    }

    const std::type_info& type() const noexcept override {
        return typeid(ModuleType);
    }
};

template<typename ModuleType>
class PropertyBase {
public:
    /// The type of a pointer to a ModuleBase
    using ModuleBasePtr = std::unique_ptr<ModuleBase>;

    /// The type of the computed property
    using return_type = typename ModuleType::return_type;

    PropertyBase(ModuleBasePtr&& base):
      impl_(std::move(downcast(std::move(base)))) {}

    template<typename...Args>
    auto operator()(Args&&...args) {
        return (*impl_)(std::forward<Args>(args)...);
    }

private:
    /// Type of a pointer to the module type's API
    using ModuleImplPtr = std::unique_ptr<ModuleType>;

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
     * is not of type @p ModuleImplType.  Strong throw guarantee.
     * @throws ??? if ModuleImplType's move ctor throws.  Same guarantee as
     * ModuleImplType's move ctor.
     */
    ModuleImplPtr downcast(ModuleBasePtr&& ptr) const {
        if(ptr->type() != typeid(ModuleType))
            throw std::bad_cast();
        auto& derived = static_cast<ModuleType&>(std::move(*ptr));
        return std::move(std::make_unique<ModuleType>(derived));
    }

    /// The actual implementation to use
    ModuleImplPtr impl_;
};

} // namespace SDE
