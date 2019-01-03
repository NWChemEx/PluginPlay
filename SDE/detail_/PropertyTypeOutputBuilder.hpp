#pragma once
#include "SDE/ModuleOutput.hpp"
#include "SDE/detail_/ModuleOutputBuilder.hpp"
#include <vector>

namespace SDE {
namespace detail_ {

template<typename... Args>
class PropertyTypeOutputBuilder {
private:
    template<typename T>
    using new_output = PropertyTypeOutputBuilder<Args..., T>;

    using my_type = PropertyTypeOutputBuilder<Args...>;

public:
    using key_type         = std::string;
    using output_type      = ModuleOutput;
    using value_type       = std::pair<key_type, output_type>;
    using output_list      = std::vector<value_type>;
    using description_type = typename output_type::description_type;
    using types            = std::tuple<Args...>;

    PropertyTypeOutputBuilder()               = default;
    PropertyTypeOutputBuilder(const my_type&) = default;
    my_type& operator=(const my_type&)   = default;
    PropertyTypeOutputBuilder(my_type&&) = default;
    my_type& operator=(my_type&& rhs) = default;

    template<typename T>
    auto add_output(std::string key) {
        outputs_.push_back(std::make_pair(std::move(key), output_type{}));
        builder_ = ModuleOutputBuilder(outputs_.back().second);
        builder_.type<T>();
        new_output<T> temp;
        temp.swap(*this);
        return std::move(temp);
    }

    auto& description(description_type desc) {
        builder_.description(std::move(desc));
        return *this;
    }

    output_list&& finalize() { return std::move(outputs_); }

private:
    template<typename... OtherArgs>
    friend class PropertyTypeOutputBuilder;

    template<typename... OtherArgs>
    void swap(PropertyTypeOutputBuilder<OtherArgs...>& other) {
        outputs_.swap(other.outputs_);
        std::swap(builder_, other.builder_);
    }

    output_list outputs_;
    ModuleOutputBuilder builder_;
};

} // namespace detail_
} // namespace SDE
