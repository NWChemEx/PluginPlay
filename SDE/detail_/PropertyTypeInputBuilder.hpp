#pragma once
#include "SDE/ModuleInput.hpp"
#include "SDE/detail_/ModuleInputBuilder.hpp"
#include <Utilities/Containers/CaseInsensitiveMap.hpp>

namespace SDE {
namespace detail_ {

template<typename...Args>
class PropertyTypeInputBuilder {
private:
    template<typename T>
    using new_input = PropertyTypeInputBuilder<Args..., T>;

    using my_type = PropertyTypeInputBuilder<Args...>;
public:
    using input_type = ModuleInput;
    using input_map  = Utilities::CaseInsensitiveMap<input_type>;
    using description_type = typename input_type::description_type;
    template<typename T>
    using validity_check = typename input_type::validity_check<T>;

    PropertyTypeInputBuilder() = default;
    PropertyTypeInputBuilder(const my_type&) = default;
    my_type& operator=(const my_type&) = default;
    PropertyTypeInputBuilder(my_type&&) = default;
    my_type& operator=(my_type&& rhs) = default;

    template<typename T>
    auto add_input(std::string key) {
        builder_ = ModuleInputBuilder(inputs_[key]);
        builder_.type<T>();
        new_input<T> temp;
        temp.swap(*this);
        return std::move(temp);
    }

    auto &description(description_type desc) {
        builder_.description(std::move(desc));
        return *this;
    }

    auto &optional() {
        builder_.optional();
        return *this;
    }

    auto &required() {
        builder_.required();
        return *this;
    }

    auto &transparent() {
        builder_.transparent();
        return *this;
    }

    auto &opaque() {
        builder_.opaque();
        return *this;
    }

    template<typename T>
    auto &default_value(T &&value) {
        builder_.default_value(std::forward<T>(value));
        return *this;
    }

    template<typename T>
    auto &check(validity_check<T> check, description_type desc = "") {
        builder_.check(std::move(check), std::move(desc));
        return *this;
    }


    input_map &&finalize() { return std::move(inputs_); }

private:
    template<typename...OtherArgs> friend class PropertyTypeInputBuilder;

    template<typename...OtherArgs>
    void swap(PropertyTypeInputBuilder<OtherArgs...> &other) {
        inputs_.swap(other.inputs_);
        std::swap(builder_, other.builder_);
    }

    input_map inputs_;
    ModuleInputBuilder builder_;

};

}
} //End namespace
