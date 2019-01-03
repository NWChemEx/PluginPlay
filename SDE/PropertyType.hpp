#pragma once
#include "SDE/detail_/PropertyTypeInputBuilder.hpp"
#include "SDE/detail_/PropertyTypeOutputBuilder.hpp"

namespace SDE {

/**
 *
 * @note Since the order of the inputs/outputs maps to the order of the run
 *       function we can't use the actual user-facing classes,
 */
template<typename derived_type>
class PropertyType {
public:
    using input_type = typename detail_::PropertyTypeInputBuilder<>::input_type;
    using input_list = typename detail_::PropertyTypeInputBuilder<>::input_list;
    using output_type =
      typename detail_::PropertyTypeOutputBuilder<>::output_type;
    using output_list =
      typename detail_::PropertyTypeOutputBuilder<>::output_list;
    using key_type = std::string;

    static input_list inputs() { return input_builder_().finalize(); }

    static output_list outputs() { return output_builder_().finalize(); }

    template<typename... Args>
    static auto wrap_inputs(Args&&... args) {
        auto params = inputs();
        if constexpr(sizeof...(Args) > 0)
            wrap_inputs_<0>(params, std::forward<Args>(args)...);
        return params;
    }

    template<typename output_map>
    static auto unwrap_outputs(output_map&& all_results) {
        auto results =
          unwrap_outputs_<0>(outputs(), std::forward<output_map>(all_results));
        if constexpr(std::tuple_size_v<decltype(results)> == 1)
            return std::get<0>(results);
        else
            return results;
    }

protected:
    template<typename T>
    auto add_input(key_type key) {
        detail_::PropertyTypeInputBuilder temp;
        return temp.add_input<T>(std::move(key));
    }

    template<typename T>
    auto add_output(key_type key) {
        detail_::PropertyTypeOutputBuilder temp;
        return temp.add_output<T>(std::move(key));
    }

private:
    using my_type = PropertyType<derived_type>;

    static auto input_builder_() {
        derived_type parent;
        return parent.inputs_();
    }

    static auto output_builder_() {
        derived_type parent;
        return parent.outputs_();
    }

    template<std::size_t ArgI, typename T, typename... Args>
    static void wrap_inputs_(input_list& params, T&& value, Args&&... args) {
        using input_tuple = typename decltype(my_type::input_builder_())::types;
        using type        = std::tuple_element_t<ArgI, input_tuple>;
        static_assert(std::is_convertible_v<T, type>,
                      "Argument is of incorrect type.");
        params[ArgI].second.change(std::forward<T>(value));
        if constexpr(sizeof...(Args) > 0)
            wrap_inputs_<ArgI + 1>(params, std::forward<Args>(args)...);
    }

    template<std::size_t ArgI, typename output_map>
    static auto unwrap_outputs_(const output_list& results,
                                output_map&& all_results) {
        using builder_type   = decltype(my_type::output_builder_());
        using output_tuple   = typename builder_type::types;
        constexpr auto nargs = std::tuple_size_v<output_tuple>;

        if constexpr(ArgI == nargs)
            return std::make_tuple();
        else {
            using type          = std::tuple_element_t<ArgI, output_tuple>;
            const key_type& key = results[ArgI].first;
            auto lhs =
              std::make_tuple(all_results.at(key).template value<type>());
            auto rhs = unwrap_outputs_<ArgI + 1>(
              results, std::forward<output_map>(all_results));
            return std::tuple_cat(std::move(lhs), std::move(rhs));
        }
    }
    //
    //    auto set_output_(output_map& results, const output_list& output) {
    //         return std::make_tuple(
    //             results.at(output[ArgI])
    //                    .value<std::tuple_element_t<ArgI, output_types>()...
    //         );
    //    }
}; // End class PropertyType

} // namespace SDE
