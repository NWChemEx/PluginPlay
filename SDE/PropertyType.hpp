#pragma once
#include "SDE/ModuleInput.hpp"
#include "SDE/ModuleOutput.hpp"
#include "SDE/detail_/PropertyTypeBuilder.hpp"

namespace SDE {

/**
 *
 * @tparam derived_type Must be default constructible and define the following
 *         functions:
 *
 *         - inputs_() must return object returned by declare_input(), but
 *           filled in.
 *         - outputs_() must return object returned by declare_output(), but
 *           filled in.
 */
template<typename derived_type>
class PropertyType {
public:
    using input_type     = ModuleInput;
    using input_builder  = detail_::PropertyTypeBuilder<input_type>;
    using output_type    = ModuleOutput;
    using output_builder = detail_::PropertyTypeBuilder<output_type>;

    static auto inputs() {
        derived_type parent;
        return parent.inputs_();
    }

    static auto outputs() {
        derived_type parent;
        return parent.outputs_();
    }

    template<typename... Args>
    static auto wrap_inputs(Args&&... args) {
        auto params = inputs();
        if constexpr(sizeof...(Args) > 0)
            wrap_inputs_<0>(params, std::forward<Args>(args)...);
        return params;
    }

    template<typename T>
    static auto unwrap_outputs(T&& all_results) {
        auto results =
          unwrap_outputs_<0>(outputs(), std::forward<T>(all_results));
        if constexpr(std::tuple_size_v<decltype(results)> == 0)
            return;
        else if constexpr(std::tuple_size_v<decltype(results)> == 1)
            return std::get<0>(results);
        else
            return results;
    }

protected:
    static auto declare_input() { return input_builder{}; }

    static auto declare_output() { return output_builder{}; }

private:
    /// Type of this instance
    using my_type = PropertyType<derived_type>;

    template<std::size_t ArgI, typename T, typename U, typename... Args>
    static void wrap_inputs_(T&& params, U&& value, Args&&... args) {
        using input_tuple =
          typename decltype(my_type::inputs())::tuple_of_fields;
        using type = std::tuple_element_t<ArgI, input_tuple>;
        static_assert(std::is_convertible_v<U, type>,
                      "Argument is of incorrect type.");
        auto pvalue = params.begin() + ArgI;
        pvalue->second.change(std::forward<U>(value));
        if constexpr(sizeof...(Args) > 0)
            wrap_inputs_<ArgI + 1>(std::forward<T>(params),
                                   std::forward<Args>(args)...);
    }

    template<std::size_t ArgI, typename T, typename U>
    static auto unwrap_outputs_(T&& builder, U&& rv) {
        using output_tuple =
          typename decltype(my_type::outputs())::tuple_of_fields;
        constexpr auto nargs = std::tuple_size_v<output_tuple>;
        if constexpr(ArgI == nargs)
            return std::make_tuple();
        else {
            using type = std::tuple_element_t<ArgI, output_tuple>;
            auto key   = (builder.begin() + ArgI)->first;
            auto lhs   = std::make_tuple(rv.at(key).template value<type>());
            auto rhs   = unwrap_outputs_<ArgI + 1>(std::forward<T>(builder),
                                                 std::forward<U>(rv));
            return std::tuple_cat(std::move(lhs), std::move(rhs));
        }
    }
}; // End class PropertyType

} // namespace SDE
