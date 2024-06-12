/*
 * Copyright 2024 NWChemEx-Project
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

/** @brief Declares a new module.
 *
 *  Modules are implemented by inheriting from ModuleBase and by implementing
 *  a default constructor and a run_ member. This macro will take care of
 *  declaring the derived class for you. The result of using this macro is a
 *  class @p module_name which properly inherits from ModuleBase.
 *
 *  @param[in] module_name The name of the class which will hold your module.
 */
#define DECLARE_MODULE(module_name)                                      \
    struct module_name : pluginplay::ModuleBase {                        \
        module_name();                                                   \
                                                                         \
    private:                                                             \
        result_map_type run_(input_map_type inputs,                      \
                             submodule_map_type submods) const override; \
    }

/** @brief Starts the definition of a Module's default constructor.
 *
 *  The module's default constructor establishes what property types it
 *  satisfies and associates meta-data with the module. This macro takes care of
 *  declaring the hook's signature so that all you need to do is declare the
 *  constructor's body.
 *
 *  @param[in] module_name The name of the class defining the module.
 */
#define MODULE_CTOR(module_name) \
    module_name::module_name() : pluginplay::ModuleBase(this)

#define TEMPLATED_MODULE_CTOR(module_name, ...) \
    module_name<__VA_ARGS__>::module_name() : pluginplay::ModuleBase(this)

/** @brief Starts the definition of a module's run hook.
 *
 *  The run hook of a class derived from ModuleBase defines what a module does
 *  when it is executed. The hook is implemented as a member function of the
 *  @p module_name class with the macro taking care of declaring the hook's
 *  signature so that all you need to do is declare the function's body.
 *
 *  @param[in] module_name The name of the class implementing the module.
 *  @param[in] inputs Name given to the first input parameter of the run_
 *                    function. In your function's body use @p inputs to access
 *                    the inputs the user provided to your module.
 *  @param[in] submods Name given to the second input parameter of the run_
 *                     function. In your function's body use @p submods to
 *                     access the submodules provided to your module.
 */
#define MODULE_RUN(module_name)                     \
    pluginplay::type::result_map module_name::run_( \
      pluginplay::type::input_map inputs,           \
      pluginplay::type::submodule_map submods) const

#define TEMPLATED_MODULE_RUN(module_name, ...)                   \
    pluginplay::type::result_map module_name<__VA_ARGS__>::run_( \
      pluginplay::type::input_map inputs,                        \
      pluginplay::type::submodule_map submods) const
