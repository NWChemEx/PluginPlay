/*
 * Copyright 2022 NWChemEx-Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once
#include "pluginplay/property_type/field_tuple.hpp"
#include "pluginplay/type_traits/is_property_type.hpp"
#include <pluginplay/fields/fields.hpp>

namespace pluginplay {
namespace detail_ {
struct BasePropertyType {
    using bases_t = std::tuple<>;
};

} // namespace detail_

/** @brief Base class for defining the API a module uses to compute a particular
 * property.
 *
 * The point of a property type is to standardize the API for obtaining a
 * property. Modules then interact with other modules through these standardized
 * APIs. This couples modules based on the properties they need and not how
 * those properties are obtained. The API is separated into two parts the
 * "input", *i.e.*, the arguments to the module's `run` member, and the
 * "results", *i.e.*, the values returned from the module's `run` member.
 *
 * The PropertyType class works by the curiously recursive template pattern, or
 * CRTP, as it's better known. CRTP enables compile-time polymorphism. What this
 * means to the user is that to implement a new property type you inherit from
 * `PropertyType<X>` where `X` is the type of the class you are implementing
 * (this is the recursive part of CRTP, your class appears as part of the base
 * class's type). Your derived class must then implement two functions:
 *
 * - `inputs_()` which calls the member function `declare_input()`, fills the
 *   object returned by `declare_input()` with the input part of the property
 *   type API, and then returns the object.
 * - `results_()` same as `inputs_()` except one calls `declare_result()` and
 *   fills the object with the details of the results that a property type
 *   computes.
 *
 * For what it's worth, the names of the functions are symbolic of the fact that
 * they should be treated like private/protected functions. The PropertyType
 * class will need access to them so the derived class must make them public or
 * declare `PropertyType<X>` as a friend. The latter is arguably idiomatically
 * more correct as it protects against users accidentally calling the
 * implementation directly.
 *
 * @tparam DerivedType Must be default constructable and define the functions
 *         listed in the above description.
 *
 * @tparam BaseType The type of the property type this property type inherits
 *                 from.
 */
template<typename DerivedType, typename BaseType = detail_::BasePropertyType>
class PropertyType {
private:
    /// Type of this class
    using my_type = PropertyType<DerivedType, BaseType>;

    /// Type of the base class
    using base_type = BaseType;

    /** @brief Determines if this the most basic PropertyType of the hierarchy
     *
     *  N.B. can't use is_base_property_type_t<DerivedType> b/c we're in the
     *  definition of `DerivedType`.
     */
    static constexpr auto am_base_pt =
      std::is_same_v<BaseType, detail_::BasePropertyType>;

    /** @brief Returns a tuple of all the property types this class inherits
     *         from.
     *
     *  This function is used to deduce the type of a tuple containing all of
     *  the property types this property type derives from. It doesn't actually
     *  fill that tuple or do anything useful. The type of the return is then
     *  used to set the `bases_t` member typedef.
     */
    static auto make_base_tuple() {
        if constexpr(am_base_pt) {
            return std::tuple<>{};
        } else {
            std::tuple<BaseType> t;
            typename BaseType::bases_t u{};
            return std::tuple_cat(t, u);
        }
    }

public:
    using bases_t = decltype(my_type::make_base_tuple());

    ///@{
    /** @name Public APIs for obtaining the fields comprising the API
     *
     *  The object that is returned from this function can be used like a map
     *  where the keys are the keywords used to access each field and the
     * values are the actual fields. The object stores the fields in the order
     * that the fields appear in the API. For example if the input API takes
     * an `int`, a `double`, and a `std::string` the first field in the object
     *  will correspond to the `int`, the second to the `double`, and the
     * third to the `std::string`. This works the same for the object that is
     *  returned containing the results.
     *
     * @return An object containing the fields in the input API of the
     * property type or the return part of the API.
     * @throws ??? if the derived class's implementation throws. Same throw
     *         guarantee as the derived class.
     */
    static auto inputs();

    static auto results();
    ///@}

    ///@{
    /** @name Functions automating the wrapping/unwrapping of inputs/results
     *
     * There's a lot of boilerplate associated with taking the inputs/results
     * and placing them into ModuleInput/ModuleResult instances or taking
     * them out of such instances. The present class knows the keywords, types,
     * and position in the API of each input/result and thus is in a position to
     * automate this for you. That is what the functions in this section do.
     *
     * @tparam T The type of the map-like container we are wrapping into or
     *         unwrapping from.
     * @tparam Args The types of the arguments to wrap.
     * @param rv The map-like container instance to hold the wrapped values or
     *        take them from.
     * @param args The values to wrap.
     * @return For wrapping, the container with the wrapped values safely
     *         inside. For unwrapping, an std::tuple of the unwrapped values
     *         ready to be used with structured bindings.
     */
    template<typename T, typename... Args>
    static auto wrap_inputs(T&& rv, Args&&... args);

    template<typename T, typename... Args>
    static auto& wrap_results(T&& rv, Args&&... args);

    template<typename T>
    static auto unwrap_inputs(T&& rv);

    template<typename T>
    static auto unwrap_results(T&& rv);
    ///@}
private:
    ///@{
    /** @name Automatic wrapping/unwrapping implementations.
     *
     * The functions in this section are the implementations for the automatic
     * wrap/unwrap functions.
     *
     * Both the wrapping and unwrapping variants work recursively calling the
     * "guts" variant until all of the arguments are handled, keeping track of
     * the recursion depth via a template non-type parameter. The call at a
     * depth `i` then worries about wrapping/unwrapping the `i`-th element. We
     * get the required information, mainly type, by calling `inputs`/`results`
     * on-the-fly.
     */
    template<typename T, typename U, typename... Args>
    static auto& wrap_(T&& rv, U&& builder, Args&&... args);

    template<std::size_t ArgI, typename T, typename U, typename V,
             typename... Args>
    static void wrap_guts_(T&& rv, U&& builder, V&& value, Args&&... args);

    template<typename T, typename U>
    static auto unwrap_(T&& builder, U&& rv);

    template<std::size_t ArgI, typename T, typename U>
    static auto unwrap_guts_(T&& builder, U&& rv);
    ///@}

}; // End class property_type

///@{
/** @name API generators
 *
 * The class implementing the property type should use these functions to
 * generate the objects for declaring the input/result APIs.
 *
 * @return A factory object that will facilitate the declaration of the
 *         input/result APIs.
 *
 * @relates PropertyType
 */
inline auto declare_input() { return FieldTuple<ModuleInput>{}; }

inline auto declare_result() { return FieldTuple<ModuleResult>{}; }
///@}

} // namespace pluginplay

#include "pluginplay/property_type/macros.hpp"
#include "pluginplay/property_type/property_type_impl.hpp"
