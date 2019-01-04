#pragma once
#include <functional>
#include <type_traits>

namespace SDE {
namespace detail_ {

template<typename T>
struct ModuleIOTraits {
    static constexpr bool is_ref = std::is_reference_v<T>;

    using no_ref = std::remove_reference_t<T>;

    using clean_T = std::decay_t<T>;

    using fxn_arg = const clean_T&;

    using ref_wrapper = std::reference_wrapper<T>;

    using wrapped_t = std::conditional_t<is_ref, ref_wrapper, fxn_arg>;

    using fxn_t = std::function<bool(fxn_arg)>;
};

} // namespace detail_
} // namespace SDE
