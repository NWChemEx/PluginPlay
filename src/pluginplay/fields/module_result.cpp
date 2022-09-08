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

#include "detail_/module_result_pimpl.hpp"
#include <pluginplay/fields/module_result.hpp>
#include <typeindex>

namespace pluginplay {

using shared_any = typename ModuleResult::shared_any;
using pimpl_t    = detail_::ModuleResultPIMPL;

ModuleResult::ModuleResult() : pimpl_(std::make_unique<pimpl_t>()) {}

ModuleResult::ModuleResult(const ModuleResult& rhs) :
  pimpl_(rhs.pimpl_->clone()) {}

ModuleResult::ModuleResult(ModuleResult&& rhs) noexcept = default;

ModuleResult& ModuleResult::operator=(ModuleResult&& rhs) noexcept = default;

ModuleResult::~ModuleResult() noexcept = default;

bool ModuleResult::has_type() const noexcept { return pimpl_->has_type(); }

bool ModuleResult::has_value() const noexcept { return pimpl_->has_value(); }

bool ModuleResult::has_description() const noexcept {
    return pimpl_->has_description();
}

ModuleResult& ModuleResult::set_type_(const std::type_info& rtti) {
    pimpl_->set_type(rtti);
    return *this;
}

void ModuleResult::change_(type::any new_value) {
    pimpl_->set_value(std::make_shared<type::any>(std::move(new_value)));
}

void ModuleResult::change_(shared_any new_value) {
    pimpl_->set_value(new_value);
}

ModuleResult& ModuleResult::set_description(type::description desc) noexcept {
    pimpl_->set_description(std::move(desc));
    return *this;
}

type::rtti ModuleResult::type() const { return pimpl_->type(); }

const shared_any& ModuleResult::at_() const { return pimpl_->value(); }

const type::description& ModuleResult::description() const {
    return pimpl_->description();
}

bool ModuleResult::operator==(const ModuleResult& rhs) const {
    return *pimpl_ == *rhs.pimpl_;
}

} // namespace pluginplay
