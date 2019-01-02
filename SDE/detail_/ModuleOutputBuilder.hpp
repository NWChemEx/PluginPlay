#pragma once
#include "SDE/ModuleOutput.hpp"

namespace SDE {
namespace detail_ {

class ModuleOutputBuilder {
public:
    ModuleOutputBuilder() = default;
    ModuleOutputBuilder(ModuleOutput& mo) : output_(&mo) {}
    ModuleOutputBuilder(const ModuleOutputBuilder& rhs) = default;
    ModuleOutputBuilder& operator=(const ModuleOutputBuilder& rhs) = default;
    ModuleOutputBuilder(ModuleOutputBuilder&& rhs)                 = default;
    ModuleOutputBuilder& operator=(ModuleOutputBuilder&&) = default;
    ~ModuleOutputBuilder()                                = default;

    ModuleOutputBuilder& description(std::string desc) {
        output_->desc = std::move(desc);
        return *this;
    }

    template<typename T>
    ModuleOutputBuilder& type() {
        output_->set_type<T>();
        return *this;
    }

private:
    ModuleOutput* output_ = nullptr;
};

} // namespace detail_
} // namespace SDE
