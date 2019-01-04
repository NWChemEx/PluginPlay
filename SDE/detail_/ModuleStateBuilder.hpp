#pragma once
#include "SDE/detail_/ModuleInputBuilder.hpp"
#include "SDE/detail_/ModuleOutputBuilder.hpp"
#include "SDE/detail_/ModuleState.hpp"

namespace SDE {
namespace detail_ {

class ModuleStateBuilder {
public:
    using key_type = typename ModuleState::key_type;

    ModuleStateBuilder(ModuleState& state) : state_(&state) {}

    template<typename T>
    auto add_input(key_type key) {
        auto& new_input = state_->inputs[std::move(key)];
        return detail_::ModuleInputBuilder(new_input).type<T>();
    }

    template<typename T>
    auto add_output(key_type key) {
        auto& new_output = state_->outputs[std::move(key)];
        return detail_::ModuleOutputBuilder(new_output).type<T>();
    }

    template<typename T>
    auto add_submodule(key_type key) {
        auto& new_submod = state_->
    }

private:
    ModuleState* state_;
};

} // namespace detail_
} // namespace SDE
