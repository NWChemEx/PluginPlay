#pragma once
#include "sde/module.hpp"
#include "sde/module_base.hpp"
#include "sde/types.hpp"
#include <utilities/type_traits/is_tuple.hpp>

namespace sde {
namespace detail_ {

/** @brief Implements a facade module
 *
 */
template<typename PropertyType>
class FacadeModule : public ModuleBase {
private:
    /// The types of the results returned by the property type
    using result_type = decltype(PropertyType::results());
    /// The type of the traits object associated with the results
    using traits_type = typename result_type::traits_type;
    /// A tuple filled with the results
    using result_tuple = typename traits_type::tuple_of_fields;
    /// The number of results
    static constexpr auto n_results = traits_type::nfields;

public:
    /// The type used to return results from the module
    using result_map = type::result_map;

    /// The type used pass inputs to the module
    using input_map = type::input_map;

    /// The type used to pass submodules into the module
    using submodule_map = type::submodule_map;

    /** @brief Creates a FacadeModule by wrapping the provided arguments.

     */
    template<typename... Args>
    explicit FacadeModule(Args&&... args);

    FacadeModule(const FacadeModule&) = delete;
    FacadeModule(FacadeModule&&)      = delete;
    FacadeModule& operator=(const FacadeModule&) = delete;
    FacadeModule& operator=(FacadeModule&&) = delete;

    /** @brief Implements the `run_as` function for the `Module` class.
     *
     *  This function is what is called by the `Module` class's `run_as` and
     *  `run` functions when the Facade module is actually run. This function
     *  simply returns the arguments that the user provied at construction as
     *  results.
     *
     *  @param[in] inputs The type-erased inputs that will be forwarded to the
     *                    user-provided callback. This value will be ignored.
     *  @param[in] submods Facade modules can not have submodules so this is
     *                     expected to be an empty map. This argument will
     *                     ultimately be ignored, but is included to satisfy the
     *                     API. This value will be ignored.
     *
     *  @return A result map which contains the results used to initialize this
     *          .
     */
    result_map run_(input_map, submodule_map) const override;

private:
    /// The inputs to this class, wrapped and ready to be returned.
    result_map m_results_;
}; // class FacadeModule

} // namespace detail_

/** @brief Creates a Facade module by wrapping the provided callback in a
 *         FacadeModule instance.
 *  @return The Facade module.
 */
template<typename PropertyType, typename... Args>
auto make_facade(Args&&... args);

// ---------------------------Implementations -------------------------------
namespace detail_ {
#define FACADE_MOD_TYPE FacadeModule<PropertyType>

template<typename PropertyType>
template<typename... Args>
FACADE_MOD_TYPE::FacadeModule(Args&&... args) : ModuleBase(this) {
    satisfies_property_type<PropertyType>();
    auto rv    = results();
    m_results_ = PropertyType::wrap_results(rv, std::forward<Args>(args)...);
}

template<typename PropertyType>
typename FACADE_MOD_TYPE::result_map FACADE_MOD_TYPE::run_(
  input_map, submodule_map) const {
    return m_results_;
}

#undef Facade_MOD_TYPE

} // namespace detail_

template<typename PropertyType, typename... Args>
auto make_facade(Args&&... args) {
    using Facade_type = detail_::FacadeModule<PropertyType>;
    auto l  = std::make_unique<Facade_type>(std::forward<Args>(args)...);
    auto lm = Module(std::move(l));
    lm.turn_off_memoization();
    return lm;
}

} // namespace sde
