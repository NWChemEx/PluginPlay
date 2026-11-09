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
#include <pluginplay/driver/driver_base.hpp>
#include <pluginplay/module/macros.hpp>

/** @brief Declares a new driver.
 *
 *  The driver counterpart of DECLARE_MODULE. The result of using this macro is
 *  a class @p driver_name which derives from DriverBase. Its constructor is
 *  defined with DRIVER_CTOR and its run member with DRIVER_RUN.
 *
 *  @param[in] driver_name The name of the class which will hold your driver.
 */
#define DECLARE_DRIVER(driver_name)                                      \
    struct driver_name : pluginplay::DriverBase {                        \
        driver_name();                                                   \
                                                                         \
    private:                                                             \
        result_map_type run_(input_map_type inputs,                      \
                             submodule_map_type submods) const override; \
    }

/** @brief Starts the definition of a driver's default constructor.
 *
 *  The driver counterpart of MODULE_CTOR.
 *
 *  @param[in] driver_name The name of the class defining the driver.
 */
#define DRIVER_CTOR(driver_name) \
    driver_name::driver_name() : pluginplay::DriverBase(this)

/** @brief Starts the definition of a templated driver's default constructor.
 *
 *  The driver counterpart of TEMPLATED_MODULE_CTOR.
 *
 *  @param[in] driver_name The name of the class defining the driver.
 */
#define TEMPLATED_DRIVER_CTOR(driver_name, ...) \
    driver_name<__VA_ARGS__>::driver_name() : pluginplay::DriverBase(this)

/** @brief Starts the definition of a driver's run member.
 *
 *  The driver counterpart of MODULE_RUN. run_ has the same signature for
 *  drivers and modules, so this is an alias of MODULE_RUN, provided so that
 *  drivers are defined with driver macros throughout.
 *
 *  @param[in] driver_name The name of the class implementing the driver.
 */
#define DRIVER_RUN(driver_name) MODULE_RUN(driver_name)

/** @brief Starts the definition of a templated driver's run member.
 *
 *  The driver counterpart of TEMPLATED_MODULE_RUN, of which it is an alias.
 *
 *  @param[in] driver_name The name of the class implementing the driver.
 */
#define TEMPLATED_DRIVER_RUN(driver_name, ...) \
    TEMPLATED_MODULE_RUN(driver_name, __VA_ARGS__)
