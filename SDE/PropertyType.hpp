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

    template<typename T, typename... Args>
    static auto wrap_inputs(T&& rv, Args&&... args) {
        return wrap_(std::forward<T>(rv), inputs(),
                     std::forward<Args>(args)...);
    }

    template<typename T, typename... Args>
    static auto& wrap_outputs(T&& output, Args&&... args) {
        return wrap_(std::forward<T>(output), outputs(),
                     std::forward<Args>(args)...);
    }

    template<typename T>
    static auto unwrap_inputs(T&& all_inputs) {
        return unwrap_(inputs(), std::forward<T>(all_inputs));
    }
    template<typename T>
    static auto unwrap_outputs(T&& all_results) {
        return unwrap_(outputs(), std::forward<T>(all_results));
    }

protected:
    static auto declare_input() { return input_builder{}; }

    static auto declare_output() { return output_builder{}; }

private:
    /// Type of this instance
    using my_type = PropertyType<derived_type>;

    template<typename T, typename U, typename... Args>
    static auto& wrap_(T&& rv, U&& builder, Args&&... args) {
        if constexpr(sizeof...(Args) > 0)
            wrap_guts_<0>(std::forward<T>(rv), std::forward<U>(builder),
                          std::forward<Args>(args)...);
        return rv;
    }

    template<std::size_t ArgI, typename T, typename U, typename V,
             typename... Args>
    static void wrap_guts_(T&& rv, U&& builder, V&& value, Args&&... args) {
        using tuple_of_fields = typename std::decay_t<U>::tuple_of_fields;
        using type            = std::tuple_element_t<ArgI, tuple_of_fields>;
        static_assert(std::is_convertible_v<V, type>,
                      "Wrap argument is of incorrect type.");
        auto pvalue = builder.begin() + ArgI;
        rv.at(pvalue->first).change(std::forward<V>(value));
        if constexpr(sizeof...(Args) > 0)
            wrap_guts_<ArgI + 1>(std::forward<T>(rv), std::forward<U>(builder),
                                 std::forward<Args>(args)...);
    }

    template<typename T, typename U>
    static auto unwrap_(T&& builder, U&& rv) {
        auto results =
          unwrap_guts_<0>(std::forward<T>(builder), std::forward<U>(rv));
        using tuple_type            = decltype(results);
        constexpr std::size_t nargs = std::tuple_size_v<tuple_type>;
        if constexpr(nargs == 0) return;
        //        else if constexpr(nargs == 1){
        //            using type = std::tuple_element_t <0, tuple_type>;
        //            return std::forward<type>(std::get<0>(results));
        //        }
        else
            return results;
    }

    template<std::size_t ArgI, typename T, typename U>
    static auto unwrap_guts_(T&& builder, U&& rv) {
        using tuple_of_fields = typename T::tuple_of_fields;
        constexpr auto nargs  = std::tuple_size_v<tuple_of_fields>;
        if constexpr(ArgI == nargs)
            return std::make_tuple();
        else {
            using type = std::tuple_element_t<ArgI, tuple_of_fields>;
            auto key   = (builder.begin() + ArgI)->first;
            auto lhs   = std::tuple<type>(rv.at(key).template value<type>());
            auto rhs   = unwrap_guts_<ArgI + 1>(std::forward<T>(builder),
                                              std::forward<U>(rv));
            return std::tuple_cat(std::move(lhs), std::move(rhs));
        }
    }
}; // End class PropertyType

} // namespace SDE
