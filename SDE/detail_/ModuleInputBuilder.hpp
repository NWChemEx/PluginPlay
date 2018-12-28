#pragma once
#include "SDE/ModuleInput.hpp"

namespace SDE {
namespace detail_ {

class ModuleInputBuilder{
public:
    using input_type = ModuleInput;
    using description_type = typename input_type::description_type;
    template<typename T>
    using validity_check = typename input_type::validity_check<T>;

    ModuleInputBuilder() = default;
    ModuleInputBuilder(const ModuleInputBuilder& rhs) = default;
    ModuleInputBuilder& operator=(const ModuleInputBuilder&) =default;
    ModuleInputBuilder(ModuleInputBuilder&& rhs) = default;
    ModuleInputBuilder& operator=(ModuleInputBuilder&&) = default;
    ModuleInputBuilder(input_type& input) : input_(&input){}

    auto& description(description_type new_desc){
        input_->desc = std::move(new_desc);
        return *this;
    }

    auto& optional(){ input_->is_optional = true; return *this; }

    auto& required(){ input_->is_optional = false; return *this; }

    auto& transparent(){ input_->is_transparent = true; return *this; }

    auto& opaque(){ input_->is_transparent = false; return *this; }

    template<typename T>
    auto& type(){ input_->set_type<T>(); return *this; }

    template<typename T>
    auto& default_value(T&& value){
        input_->change(std::forward<T>(value));
        return *this;
    }

    template<typename T>
    auto& check(validity_check<T> new_check, description_type desc = ""){
        input_->add_check(std::move(new_check), std::move(desc));
        return *this;
    }
private:
    ModuleInput* input_ = nullptr;
};

}
}
