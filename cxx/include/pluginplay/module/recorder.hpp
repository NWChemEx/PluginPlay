/*
 * Copyright 2026 NWChemEx-Project
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
#include <pluginplay/module/module_base.hpp>
#include <pluginplay/types.hpp>
#include <typeinfo>
#include <vector>

namespace pluginplay {

/** @brief A module which records the results of the module it wraps.
 *
 *  A Recorder satisfies one property type, which is chosen when it is made,
 *  and has one submodule, "Wrapped", of the same property type. When run, it
 *  forwards its inputs, as they are, to "Wrapped", keeps the results, and
 *  returns them. This is how results which are only used inside a
 *  calculation are gotten out of it: put the Recorder between the module which
 *  computes them and the module which uses them.
 *
 *  From C++, for a property type `PT`:
 *
 *  ```cpp
 *  auto recorder = std::make_shared<pluginplay::Recorder>(PT{});
 *  mm.add_module("Recorder", recorder);
 *  mm.change_submod("Recorder", "Wrapped", "Computes PT");
 *  mm.change_submod("Uses PT", "PT submodule", "Recorder");
 *  ```
 *
 *  and from Python, `recorder = pluginplay.Recorder(PT())`. After the
 *  calculation, `recorder->records()` holds the results of each run.
 *
 *  N.B. A run whose results are memoized does not call the Recorder, so it is
 *  not recorded. To record every run, turn off the memoization of the
 *  Recorder (or of "Wrapped").
 */
class Recorder : public ModuleBase {
public:
    /// Type of the RTTI of a property type
    using rtti_type = type::rtti;

    /// Type of the inputs of a property type
    using input_map = type::input_map;

    /// Type of the results of a property type
    using result_map = type::result_map;

    /// Type of the submodules of a module
    using submodule_map = type::submodule_map;

    /// Type of the container holding the results of each run
    using record_container = std::vector<result_map>;

    /** @brief Makes a Recorder satisfying the property type of @p pt.
     *
     *  @tparam PropertyType The property type the Recorder satisfies.
     *
     *  @param[in] pt An instance of @p PropertyType. Only its type is used.
     *
     *  @throw std::bad_alloc if there is insufficient memory. Strong throw
     *                        guarantee.
     */
    template<typename PropertyType>
    explicit Recorder(const PropertyType& /*pt*/) :
      Recorder(rtti_type(typeid(PropertyType)),
               to_map_<input_map>(PropertyType::inputs()),
               to_map_<result_map>(PropertyType::results())) {}

    /** @brief Makes a Recorder satisfying the property type described by its
     *         RTTI, inputs, and results.
     *
     *  This is how a Recorder is made when the property type is only known at
     *  runtime, e.g., from Python.
     *
     *  @param[in] rtti The RTTI of the property type.
     *  @param[in] inputs The inputs of the property type.
     *  @param[in] results The results of the property type.
     *
     *  @throw std::bad_alloc if there is insufficient memory. Strong throw
     *                        guarantee.
     */
    Recorder(rtti_type rtti, input_map inputs, result_map results);

    /** @brief The results of each run, in the order of the runs.
     *
     *  @return The results recorded so far.
     *
     *  @throw None No throw guarantee.
     */
    const record_container& records() const noexcept { return m_records_; }

    /** @brief Forgets the results recorded so far.
     *
     *  @throw None No throw guarantee.
     */
    void clear() noexcept { m_records_.clear(); }

private:
    /// The fields of a property type, @p fields, as a map of type MapType
    template<typename MapType, typename FieldsType>
    static MapType to_map_(const FieldsType& fields) {
        MapType rv;
        rv.insert(fields.begin(), fields.end());
        return rv;
    }

    /// Runs "Wrapped" with @p inputs and records its results
    result_map run_(input_map inputs, submodule_map submods) const override;

    /// The results of each run. Mutable, since run_ is const.
    mutable record_container m_records_;
};

} // namespace pluginplay
