#pragma once
#include "SDE/ModuleInput.hpp"
#include "SDE/ModuleOutput.hpp"
#include "SDE/detail_/PropertyTypeBuilder.hpp"

namespace SDE {

template<typename derived_type>
class PropertyType {
public:
    using input_type     = ModuleInput;
    using input_builder  = detail_::PropertyTypeBuilder<input_type>;
    using output_type    = ModuleOutput;
    using output_builder = detail_::PropertyTypeBuilder<output_type>;

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
    static auto declare_input() { return input_builder{}; }

    static auto declare_output() { return output_builder_{}; }

private:
    using my_type = PropertyType<derived_type>;

    //@{
    /** @name compile-time polymorphism hooks
     *
     * The derived class is responsible for implementing our API. Ideally we
     * want this API to be as simple as possible. The trick to this is to use a
     * series of wrappers around the actual hooks instead of calling the hooks
     * directly. By doing this we can more or less insulate our code from the
     * qualifiers/lack of qualifiers the derived class applies to each of the
     * hooks. For example, in a perfect world, the derived class would declare
     * each hook as static, but I anticipate most developers will not do so.
     */
    static auto inputs_wrapper_() {
        derived_type parent;
        return parent.inputs_();
    }

    static auto outputs_wrapper_() {
        derived_type parent;
        return parent.outputs_();
    }
    //@}
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
}; // End class PropertyType

} // namespace SDE
