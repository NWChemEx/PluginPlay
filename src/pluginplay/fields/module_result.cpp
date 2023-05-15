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
using m_pimpl_t  = detail_::ModuleResultPIMPL;

ModuleResult::ModuleResult() : m_pimpl_(std::make_unique<m_pimpl_t>()) {}

ModuleResult::ModuleResult(const ModuleResult& rhs) :
  m_pimpl_(rhs.m_pimpl_->clone()) {}

ModuleResult::ModuleResult(ModuleResult&& rhs) noexcept = default;

ModuleResult& ModuleResult::operator=(ModuleResult&& rhs) noexcept = default;

ModuleResult::~ModuleResult() noexcept = default;

bool ModuleResult::has_type() const noexcept {
    return has_pimpl_() && m_pimpl_->has_type();
}

bool ModuleResult::has_value() const noexcept { return m_pimpl_->has_value(); }

bool ModuleResult::has_description() const noexcept {
    return m_pimpl_->has_description();
}

ModuleResult& ModuleResult::set_description(type::description desc) noexcept {
    m_pimpl_->set_description(std::move(desc));
    return *this;
}

type::rtti ModuleResult::type() const { return m_pimpl_->type(); }

const type::description& ModuleResult::description() const {
    return m_pimpl_->description();
}

bool ModuleResult::operator==(const ModuleResult& rhs) const {
    return *m_pimpl_ == *rhs.m_pimpl_;
}

// -- Private Methods

bool ModuleResult::has_pimpl_() const noexcept {
    return static_cast<bool>(m_pimpl_);
}

void ModuleResult::assert_type_set_() const {
    if(has_type()) return;
    throw std::runtime_error("Type has not been set");
}

ModuleResult& ModuleResult::set_type_(const std::type_info& rtti) {
    m_pimpl_->set_type(rtti);
    return *this;
}

void ModuleResult::set_type_check_(type_check_function_type fxn) {
    pimpl_().set_type_check(std::move(fxn));
}

const shared_any& ModuleResult::at_() const { return m_pimpl_->value(); }

void ModuleResult::change_(type::any new_value) {
    assert_type_set_();
    m_pimpl_->set_value(std::make_shared<type::any>(std::move(new_value)));
}

void ModuleResult::change_(shared_any new_value) {
    assert_type_set_();
    m_pimpl_->set_value(new_value);
}

ModuleResult::pimpl_reference ModuleResult::pimpl_() {
    if(!m_pimpl_) m_pimpl_ = std::make_unique<pimpl_type>();
    return *m_pimpl_;
}

} // namespace pluginplay
