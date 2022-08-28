#pragma once
#include "type_eraser.hpp"
#include <pluginplay/fields/fields.hpp>

namespace pluginplay::cache::database {

/// Specialize MakeAny for ModuleInputs
template<>
struct MakeAny<ModuleInput> {
    template<typename U>
    static any::AnyField convert(U&& v) {
        return v.template value<any::AnyField>();
    }
}; // namespace pluginplay::template<>structMakeAny<ModuleInput>

/// Specialize MakeAny for ModuleResults
template<>
struct MakeAny<ModuleResult> {
    template<typename U>
    static any::AnyField convert(U&& v) {
        using shared_any = typename ModuleResult::shared_any;
        return *v.template value<shared_any>();
    }
};

} // namespace pluginplay::cache::database
