#pragma once
#include "pluginplay/any/any_input.hpp"
#include "pluginplay/any/any_result.hpp"
#include "pluginplay/any/detail_/any_input_wrapper.hpp"
#include "pluginplay/any/detail_/any_result_wrapper.hpp"

namespace pluginplay {

template<typename T, typename... Args>
auto make_any_result(Args&&... args) {
    using pimpl_type = detail_::AnyResultWrapper<T>;
    auto pimpl = std::make_unique<pimpl_type>(T(std::forward<Args>(args)...));
    return AnyResult(std::move(pimpl));
}

template<typename T, typename AnyType>
T any_cast(AnyType&& da_any) {
    return da_any.pimpl_().template cast<T>();
}

} // namespace pluginplay
