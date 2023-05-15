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
#include <pluginplay/fields/bounds_checking/compare_to.hpp>
#include <pluginplay/fields/bounds_checking/in_range.hpp>
#include <pluginplay/fields/bounds_checking/print_bounds.hpp>
#include <pluginplay/fields/bounds_checking/type_check.hpp>

/** @brief Namespace for functors used in bounds checking inputs to a module
 *
 *  Many of the bounds checks we provide for the user are likely to interfere
 *  name-wise with other classes. Hence we namespace protect them with the
 *  bounds_checking namespace.
 */
namespace pluginplay::bounds_checking {}

/** @brief Contains template meta-programming classes for printing bounds checks
 *
 *  In order to print a bounds check we need to know if it is one of the ones
 *  defined by the pluginplay. The pluginplay::bounds_check::detail_ namespace
 * includes the machinery required for these checks.
 */
namespace pluginplay::bounds_checking::detail_ {}
