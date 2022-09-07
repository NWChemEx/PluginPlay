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

#include "detail_/submodule_request_pimpl.hpp"

namespace pluginplay {

using module_ptr = typename SubmoduleRequest::module_ptr;

SubmoduleRequest::SubmoduleRequest() :
  m_pimpl_(std::make_unique<detail_::SubmoduleRequestPIMPL>()) {}

SubmoduleRequest::SubmoduleRequest(const SubmoduleRequest& rhs) :
  m_pimpl_(rhs.m_pimpl_->clone()) {}

SubmoduleRequest::SubmoduleRequest(SubmoduleRequest&& rhs) noexcept = default;

SubmoduleRequest& SubmoduleRequest::operator=(SubmoduleRequest&&) noexcept =
  default;

SubmoduleRequest::~SubmoduleRequest() noexcept = default;

bool SubmoduleRequest::has_type() const noexcept {
    return m_pimpl_->has_type();
}

bool SubmoduleRequest::has_module() const noexcept {
    return m_pimpl_->has_module();
}

bool SubmoduleRequest::has_description() const noexcept {
    return m_pimpl_->has_description();
}

bool SubmoduleRequest::ready() const noexcept { return m_pimpl_->ready(); }

void SubmoduleRequest::change(module_ptr new_mod) {
    m_pimpl_->set_module(new_mod);
}

SubmoduleRequest& SubmoduleRequest::set_description(
  type::description desc) noexcept {
    m_pimpl_->set_description(std::move(desc));
    return *this;
}

type::rtti SubmoduleRequest::type() const { return m_pimpl_->type(); }

typename SubmoduleRequest::submod_uuid_map SubmoduleRequest::submod_uuids()
  const {
    return m_pimpl_->submod_uuids();
}

typename SubmoduleRequest::uuid_type SubmoduleRequest::uuid() const {
    return m_pimpl_->uuid();
}

const Module& SubmoduleRequest::value() const { return m_pimpl_->value(); }

Module& SubmoduleRequest::value() { return m_pimpl_->value(); }

const type::description& SubmoduleRequest::description() const {
    return m_pimpl_->description();
}

void SubmoduleRequest::lock() { m_pimpl_->lock(); }

bool SubmoduleRequest::operator==(const SubmoduleRequest& rhs) const {
    return *m_pimpl_ == *rhs.m_pimpl_;
}

void SubmoduleRequest::set_type_(const std::type_info& type,
                                 type::input_map inputs) {
    m_pimpl_->set_type(std::type_index(type), std::move(inputs));
}

} // namespace pluginplay
