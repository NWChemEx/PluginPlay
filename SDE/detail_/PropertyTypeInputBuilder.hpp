#pragma once
#include "SDE/ModuleInput.hpp"
#include "SDE/detail_/ModuleInputBuilder.hpp"
#include <vector>

namespace SDE {
namespace detail_ {

template<typename... Args>
class PropertyTypeInputBuilder {
private:
    template<typename T>
    using new_input = PropertyTypeInputBuilder<Args..., T>;

    using my_type = PropertyTypeInputBuilder<Args...>;

public:
    using key_type         = std::string;
    using input_type       = ModuleInput;
    using value_type       = std::pair<key_type, input_type>;
    using input_list       = std::vector<value_type>;
    using description_type = typename input_type::description_type;
    template<typename T>
    using validity_check = typename input_type::validity_check<T>;

    using types = std::tuple<Args...>;

    PropertyTypeInputBuilder()               = default;
    PropertyTypeInputBuilder(const my_type&) = default;
    my_type& operator=(const my_type&)  = default;
    PropertyTypeInputBuilder(my_type&&) = default;
    my_type& operator=(my_type&& rhs) = default;

    template<typename T>
    auto add_input(std::string key) {
        auto da_pair = std::make_pair(std::move(key), input_type{});
        inputs_.push_back(std::move(da_pair));
        builder_ = ModuleInputBuilder(inputs_.back().second);
        builder_.type<T>();
        new_input<T> temp;
        temp.swap(*this);
        return std::move(temp);
    }

    auto& description(description_type desc) {
        builder_.description(std::move(desc));
        return *this;
    }

    auto& optional() {
        builder_.optional();
        return *this;
    }

    auto& required() {
        builder_.required();
        return *this;
    }

    auto& transparent() {
        builder_.transparent();
        return *this;
    }

    auto& opaque() {
        builder_.opaque();
        return *this;
    }

    template<typename T>
    auto& default_value(T&& value) {
        builder_.default_value(std::forward<T>(value));
        return *this;
    }

    template<typename T>
    auto& check(validity_check<T> check, description_type desc = "") {
        builder_.check(std::move(check), std::move(desc));
        return *this;
    }

    input_list&& finalize() { return std::move(inputs_); }

private:
    template<typename... OtherArgs>
    friend class PropertyTypeInputBuilder;

    template<typename... OtherArgs>
    void swap(PropertyTypeInputBuilder<OtherArgs...>& other) {
        inputs_.swap(other.inputs_);
        std::swap(builder_, other.builder_);
    }

    input_list inputs_;
    ModuleInputBuilder builder_;
};

} // namespace detail_
} // namespace SDE
