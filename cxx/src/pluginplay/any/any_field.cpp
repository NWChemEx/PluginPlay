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

#include "pluginplay/any/any_field.hpp"
#include "pluginplay/any/detail_/any_field_base.hpp"

namespace pluginplay::any {

// -----------------------------------------------------------------------------
// -- CTors and Assignment
// -----------------------------------------------------------------------------

AnyField::AnyField(pimpl_pointer pimpl) noexcept : m_pimpl_(std::move(pimpl)) {}

AnyField::AnyField(const AnyField& other) :
  m_pimpl_(other.has_value() ? other.m_pimpl_->clone() : nullptr) {}

AnyField::AnyField(AnyField&& other) noexcept = default;

AnyField& AnyField::operator=(const AnyField& rhs) {
    if(this != &rhs) AnyField(rhs).swap(*this);
    return *this;
}

AnyField& AnyField::operator=(AnyField&& rhs) noexcept = default;

AnyField::~AnyField() noexcept = default;

void AnyField::swap(AnyField& other) noexcept { m_pimpl_.swap(other.m_pimpl_); }

void AnyField::reset() noexcept { m_pimpl_.reset(); }

typename AnyField::rtti_type AnyField::type() const noexcept {
    if(!has_value()) return rtti_type{typeid(nullptr)};
    return m_pimpl_->type();
}

bool AnyField::operator==(const AnyField& rhs) const noexcept {
    // xor condition, checks if one has PIMPL and the other doesn't
    if(has_value() != rhs.has_value()) return false;

    // Either we both do or both don't have PIMPLs, check for latter
    if(!has_value()) return true;

    // Both have PIMPLs compare them
    return m_pimpl_->value_equal(*rhs.m_pimpl_);
}

bool AnyField::are_equal(const AnyField& rhs) const noexcept {
    // xor condition, checks if one has PIMPL and the other doesn't
    if(has_value() != rhs.has_value()) return false;

    // Either we both do or both don't have PIMPLs, check for latter
    if(!has_value()) return true;

    // Both have PIMPLs compare them
    return m_pimpl_->are_equal(*rhs.m_pimpl_);
}

std::ostream& AnyField::print(std::ostream& os) const {
    if(!has_value()) return os;
    return m_pimpl_->print(os);
}

bool AnyField::has_value() const noexcept {
    return static_cast<bool>(m_pimpl_);
}

bool AnyField::owns_value() const noexcept {
    if(!has_value()) return false;

    // Only way we can alias it is by const reference
    return !m_pimpl_->storing_const_reference();
}

} // namespace pluginplay::any
