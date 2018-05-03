#pragma once
#include <Utilities/Containers/CaseInsensitiveMap.hpp>
#include <memory> // for shared_ptr and unique_ptr

namespace SDE {
namespace detail_ {

/// This function exists solely so we can determine its return type
template<typename ReturnType,typename ClassType, typename...Args>
auto call_fxn(ReturnType(ClassType::*)(Args...)) {return ReturnType{};}

/**
 * @brief This is a haphazard version of std::result_of that doesn't need the
 * arguments.
 *
 * This class contains a member typedef @p type, which is the type of the value
 * returned from ModuleType::operator().  It assumes that type is copy
 * constructable and that ModuleType does not overload the parenthesis operator.
 *
 * If you want to know how this works:
 * 1. We define a function call_fxn which takes a function pointer and returns a
 *    default constructed version of the return type (which it figures out from
 *    its own signature)
 * 2. We simulate calling call_fxn when it is given a function pointer to
 *    ModuleType::operator()
 * 3. decltype then tells us the return type of this simulated call, which we
 *    know is also the return type of the actual call
 *
 * @tparam ModuleType The module type whose return we wish to know.
 */
template<typename ModuleType>
struct ExtractReturnType{
    /// The type of the result of calling ModuleType::operator().
    using type = decltype(call_fxn(&ModuleType::operator()));
};

} // namespace detail_


class ModuleBase {
public:
    /**
     * @brief A function that allows us to get the RTTI of the module's type
     *        without having to downcast to it.
     * @return The RTTI of the module type's type.
     */
    const std::type_info& type()const noexcept = 0;

protected:
    /// Typedef of a pointer to a module via its base class
    using ModuleBasePtr = std::uniqe_ptr<ModuleBase>;

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
    Utilities:CaseInsensitiveMap<ModuleBasePtr> submodules_;
};

template<typename ModuleType>
class ModuleImpl : public ModuleBase {
public:
    /// The return type of calling the module
    using return_type =
      typename detail_::ExtractReturnType<ModuleType>::return_type;

    const std::type_info& type() const noexcept override {
        return decltype(ModuleType);
    }
};

template<typename ModuleType>
class PropertyBase {
public:
    /// The type of a pointer to a ModuleBase
    using ModuleBasePtr = std::unique_ptr<ModuleBase>;

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
        if(ptr->type() != decltype(ModuleType))
            throw std::bad_cast();
        auto& derived = static_cast<ModuleType&>(std::move(*ptr));
        return std::move(std::make_unique<ModuleType>(derived));
    }

    /// The actual implementation to use
    ModuleImplPtr impl_;
};

} // namespace SDE
