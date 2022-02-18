#pragma once
#include "pluginplay/any/any_input.hpp"
#include "pluginplay/any/any_result.hpp"
#include "pluginplay/any/detail_/any_input_wrapper.hpp"
#include "pluginplay/any/detail_/any_result_wrapper.hpp"

namespace pluginplay {

/** @brief Creates an AnyResult by wrapping an object of type T.
 *
 *  @tparam T The unqualified (i.e. no cv-qualifiers or refernces) type of the
 *            object that the AnyResult will wrap.
 *  @tparam Args The types of the arguments which will be forwarded to T's ctor.
 *
 *  This free function wraps the process of creating an AnyResult around an
 *  object of type @p T. The actual implmentation creates an object of type
 *  @p T inplace by invoking @p T's ctor with @p args. In common usage, @p args
 *  is a @p T instance and will invoke the copy or move ctor of @p T depending
 *  on whether @p args is an lvalue or an rvalue.
 *
 *  This function will raise a static assert if @p T is anything other than an
 *  unqualified type.
 *
 *  @param[in] args The arguments to use to construct an instance of type @p T.
 *
 *  @return An AnyResult instance which wraps the created @p T instance.
 *
 *  @throw ??? If the @p T ctor throws when invoked with @p args. Same throw
 *             guarantee.
 */
template<typename T, typename... Args>
auto make_any_result(Args&&... args) {
    using pimpl_type = detail_::AnyResultWrapper<T>;
    auto pimpl = std::make_unique<pimpl_type>(T(std::forward<Args>(args)...));
    return AnyResult(std::move(pimpl));
}

/** @brief Creates an AnyInput by wrapping an object of type T.
 *
 *  @tparam T The qualified (i.e. including cv-qualifiers and/or refernces) type
 *            of the object that the AnyInput will wrap.
 *  @tparam Args The types of the arguments which will be forwarded to T's ctor.
 *
 *  This free function wraps the process of creating an AnyInput around an
 *  object of type @p T. The actual implmentation creates an object of type
 *  @p T inplace by invoking @p T's ctor with @p args. In common usage, @p args
 *  is a @p T instance and will invoke the copy or move ctor of @p T depending
 *  on whether @p args is an lvalue or an rvalue.
 *
 *  This function will raise a static assert if @p T is a mutable reference.
 *
 *  @param[in] args The arguments to use to construct an instance of type @p T.
 *
 *  @return An AnyInput instance which wraps the created @p T instance.
 *
 *  @throw ??? If the @p T ctor throws when invoked with @p args. Same throw
 *             guarantee.
 */
template<typename T, typename... Args>
auto make_any_input(Args&&... args) {
    using pimpl_type = detail_::AnyInputWrapper<T>;
    using clean_type = std::decay_t<T>;
    static_assert(!std::is_same_v<T, clean_type&>, "Can't wrap mutable ref");
    auto pimpl = std::make_unique<pimpl_type>(T(std::forward<Args>(args)...));
    return AnyInput(std::move(pimpl));
}

/** @brief Returns the object wrapped in either an AnyInput or an AnyField.
 *
 *  @tparam T The qualified type (i.e. including cv-qualifiers and references)
 *            to return.
 *  @tparam AnyType The type of the type-erased object we are unwrapping.
 *                  Expected to be either an AnyInput or AnyField instance.
 *
 *  This free function is used to unwrap an AnyInput or an AnyField. To avoid
 *  unnecessary copies users need to fully specify the type of object to unwrap.
 *  In many cases @p T should be something like `const U&` where `U` is an
 *  unqualified type (this example would return a read-only reference to an
 *  instance of type `U`). The allowed types for unwrapping depend on whether
 *  the any is an AnyInput or an AnyResult as well as the type of the wrapped
 *  object. Assuming a wrapped object of type @p U it is always possible for
 *  @p T to be:
 *
 *  - `std::decay_t<U>` (by mutable copy)
 *  - `const std::decay_t<U>` (by read-only copy)
 *  - `const std::decay_t<U>&` (by read-only reference)
 *
 *  Additionally, for AnyInputs which own mutable values one can retrieve
 *  mutable references, i.e. `T = std::decay_t<u>&` is also allowed.
 *
 *  Conversions which are not allowed will either trip static assertions or
 *  result in this function throwing `std::runtime_error` depending on whether
 *  or not we know at compile time if a conversion is allowed. Attempting to
 *  unwrap anything other than an AnyInput or an AnyResult will also lead to a
 *  static assertion being tripped.
 *
 *  @note It is not possible to move a mutable value out of an AnyInput at this
 *        time.
 *
 *  @param[in] da_any The type-erased input or result that we are unwrapping.
 *
 *  @return The object wrapped in @p da_any as an instance of type @p T.
 *
 *  @throw std::runtime_error if @p da_any does not currently wrap a value.
 *                            Strong throw guarantee.
 *
 *  @throw std::runtime_error if the wrapped object can not be returned as an
 *                            instance of type @p T. Strong throw guarantee.
 */
template<typename T, typename AnyType>
T any_cast(AnyType&& da_any) {
    using clean_type             = std::decay_t<AnyType>;
    constexpr bool is_any_input  = std::is_same_v<clean_type, AnyInput>;
    constexpr bool is_any_result = std::is_same_v<clean_type, AnyResult>;
    static_assert(is_any_input || is_any_result,
                  "da_any must be an AnyInput or an AnyResult.");

    return da_any.pimpl_().template cast<T>();
}

} // namespace pluginplay
