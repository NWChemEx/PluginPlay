#pragma once
#include <tuple>
namespace SDE {
namespace detail_ {

/// Function that should never be called, but whose return type is the same as
/// our member function's return type
template<typename R, typename C, typename... Args>
R get_member_fxn_return(R (C::*)(Args...));

/// Function that should never be called, but whose return type is the same as
/// our member function's argument's types (wrapped in a tuple) instance
template<typename R, typename C, typename... Args>
std::tuple<Args...> get_member_fxn_args(R (C::*)(Args...));

/// Struct with member types return_type and args_type set to the returns and
/// arguments of `run_` member function of @p class_name
template<typename class_name>
struct RunDetails {
    using return_type = decltype(get_member_fxn_return(&class_name::run));
    using args_type   = decltype(get_member_fxn_args(&class_name::run));
};

template<typename class_name>
using RunDetails_return_type = typename RunDetails<class_name>::return_type;

template<typename class_name>
using RunDetails_args_type = typename RunDetails<class_name>::args_type;

} // namespace detail_
} // namespace SDE
