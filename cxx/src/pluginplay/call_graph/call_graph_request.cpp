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

#include <pluginplay/call_graph/call_graph_request.hpp>
#include <pluginplay/module/module_base.hpp>
#include <sstream>
#include <stdexcept>
#include <utilities/strings/string_tools.hpp>

namespace pluginplay {
namespace {

/// Prints a type-erased value for messages
std::string value_string(const type::any& value) {
    std::stringstream ss;
    value.print(ss);
    return ss.str();
}

/// Quotes @p str for messages
std::string quote(const std::string& str) { return "\"" + str + "\""; }

} // namespace

// -----------------------------------------------------------------------------
// -- WiringRequest
// -----------------------------------------------------------------------------

WiringRequest WiringRequest::make_set_input(qualified_key module_key,
                                            key_type input_key,
                                            value_type value) {
    WiringRequest rv(kind_type::set_input, std::move(module_key),
                     std::move(input_key));
    rv.m_value_ = std::move(value);
    return rv;
}

WiringRequest WiringRequest::make_set_submod(qualified_key module_key,
                                             key_type callback_key,
                                             qualified_key submod_key) {
    WiringRequest rv(kind_type::set_submod, std::move(module_key),
                     std::move(callback_key));
    rv.m_submod_key_ = std::move(submod_key);
    return rv;
}

WiringRequest WiringRequest::make_forward_input(qualified_key module_key,
                                                key_type target_input_key,
                                                key_type input_key) {
    WiringRequest rv(kind_type::forward_input, std::move(module_key),
                     std::move(target_input_key));
    rv.m_source_key_ = std::move(input_key);
    return rv;
}

WiringRequest WiringRequest::make_forward_submod(qualified_key module_key,
                                                 key_type target_callback_key,
                                                 key_type callback_key) {
    WiringRequest rv(kind_type::forward_submod, std::move(module_key),
                     std::move(target_callback_key));
    rv.m_source_key_ = std::move(callback_key);
    return rv;
}

WiringRequest& WiringRequest::required(bool value) {
    if(m_owner_.value != nullptr) m_owner_.value->assert_unlocked_();
    m_required_ = value;
    return *this;
}

const WiringRequest::value_type& WiringRequest::value() const {
    assert_kind_(m_kind_ == kind_type::set_input, "set_input");
    return m_value_;
}

const WiringRequest::qualified_key& WiringRequest::submod_key() const {
    assert_kind_(m_kind_ == kind_type::set_submod, "set_submod");
    return m_submod_key_;
}

const WiringRequest::key_type& WiringRequest::source_key() const {
    assert_kind_(is_forward(), "forward");
    return m_source_key_;
}

std::string WiringRequest::to_string() const {
    const auto target = quote(m_module_key_.to_string()) + "'s " +
                        (sets_input() ? "input " : "submodule ") +
                        quote(m_target_key_);
    switch(m_kind_) {
        case kind_type::set_input:
            return "setting " + target + " to " + value_string(m_value_);
        case kind_type::set_submod:
            return "setting " + target + " to " +
                   quote(m_submod_key_.to_string());
        case kind_type::forward_input:
            return "forwarding input " + quote(m_source_key_) + " to " + target;
        case kind_type::forward_submod:
            return "forwarding submodule " + quote(m_source_key_) + " to " +
                   target;
    }
    return ""; // Unreachable, but some compilers warn without it
}

bool WiringRequest::operator==(const WiringRequest& rhs) const noexcept {
    using utilities::strings::case_insensitive_equal;
    return m_kind_ == rhs.m_kind_ && m_module_key_ == rhs.m_module_key_ &&
           case_insensitive_equal(m_target_key_, rhs.m_target_key_) &&
           m_value_ == rhs.m_value_ && m_submod_key_ == rhs.m_submod_key_ &&
           case_insensitive_equal(m_source_key_, rhs.m_source_key_) &&
           m_required_ == rhs.m_required_;
}

void WiringRequest::assert_kind_(bool is_kind, const char* what) const {
    if(is_kind) return;
    throw std::runtime_error("Wiring request " + to_string() + " is not a " +
                             what + " request");
}

// -----------------------------------------------------------------------------
// -- ConditionalWiring
// -----------------------------------------------------------------------------

const ConditionalWiring::value_type& ConditionalWiring::Branch::value() const {
    if(!has_condition())
        throw std::runtime_error("The final branch of a chain has no value");
    return m_value_;
}

std::string ConditionalWiring::Branch::condition_string() const {
    if(!has_condition()) return "no other condition holds";
    return quote(*m_input_key_) + " is " + value_string(m_value_);
}

bool ConditionalWiring::Branch::operator==(const Branch& rhs) const noexcept {
    using utilities::strings::case_insensitive_equal;
    if(has_condition() != rhs.has_condition()) return false;
    if(has_condition() &&
       !case_insensitive_equal(*m_input_key_, *rhs.m_input_key_))
        return false;
    return m_value_ == rhs.m_value_ && m_requests_ == rhs.m_requests_ &&
           m_required_ == rhs.m_required_;
}

ConditionalWiring::ConditionalWiring(CallGraphRequest& owner,
                                     std::size_t owner_id, key_type input_key,
                                     value_type value) :
  m_owner_(&owner), m_owner_id_(owner_id) {
    add_branch_(std::move(input_key), std::move(value));
}

ConditionalWiring& ConditionalWiring::set_submod(qualified_key module_key,
                                                 key_type callback_key,
                                                 qualified_key submod_key) {
    return add_(WiringRequest::make_set_submod(
      std::move(module_key), std::move(callback_key), std::move(submod_key)));
}

ConditionalWiring& ConditionalWiring::forward_input(qualified_key module_key,
                                                    key_type target_input_key,
                                                    key_type input_key) {
    m_owner_->assert_driver_input_(input_key);
    return add_(WiringRequest::make_forward_input(std::move(module_key),
                                                  std::move(target_input_key),
                                                  std::move(input_key)));
}

ConditionalWiring& ConditionalWiring::forward_submod(
  qualified_key module_key, key_type target_callback_key,
  key_type callback_key) {
    m_owner_->assert_driver_submod_(callback_key);
    return add_(WiringRequest::make_forward_submod(
      std::move(module_key), std::move(target_callback_key),
      std::move(callback_key)));
}

ConditionalWiring& ConditionalWiring::otherwise() {
    if(!m_branches_.back().has_condition())
        throw std::logic_error("The chain already has a final branch");
    m_branches_.emplace_back();
    return *this;
}

ConditionalWiring& ConditionalWiring::required() noexcept {
    auto& branch       = m_branches_.back();
    branch.m_required_ = true;
    for(auto& request : branch.m_requests_) request.required();
    return *this;
}

ConditionalWiring& ConditionalWiring::add_(WiringRequest request) {
    m_owner_->claim_(request, m_owner_id_);
    auto& branch = m_branches_.back();
    if(branch.m_required_) request.required();
    branch.m_requests_.push_back(std::move(request));
    return *this;
}

ConditionalWiring& ConditionalWiring::add_branch_(key_type input_key,
                                                  value_type value) {
    if(!m_branches_.empty() && !m_branches_.back().has_condition())
        throw std::logic_error(
          "A branch can not follow the final branch of a chain");
    m_owner_->assert_driver_input_(input_key);
    Branch branch;
    branch.m_input_key_.emplace(std::move(input_key));
    branch.m_value_ = std::move(value);
    m_branches_.push_back(std::move(branch));
    return *this;
}

// -----------------------------------------------------------------------------
// -- CallGraphRequest
// -----------------------------------------------------------------------------

CallGraphRequest::CallGraphRequest(const CallGraphRequest& rhs) :
  m_driver_(rhs.m_driver_),
  m_requests_(rhs.m_requests_),
  m_chains_(rhs.m_chains_),
  m_submod_defaults_(rhs.m_submod_defaults_),
  m_memoization_(rhs.m_memoization_),
  m_claims_(rhs.m_claims_),
  m_next_owner_(rhs.m_next_owner_),
  m_locked_(rhs.m_locked_) {
    adopt_children_();
}

CallGraphRequest::CallGraphRequest(CallGraphRequest&& rhs) noexcept :
  m_driver_(rhs.m_driver_),
  m_requests_(std::move(rhs.m_requests_)),
  m_chains_(std::move(rhs.m_chains_)),
  m_submod_defaults_(std::move(rhs.m_submod_defaults_)),
  m_memoization_(std::move(rhs.m_memoization_)),
  m_claims_(std::move(rhs.m_claims_)),
  m_next_owner_(rhs.m_next_owner_),
  m_locked_(rhs.m_locked_) {
    adopt_children_();
}

CallGraphRequest& CallGraphRequest::operator=(const CallGraphRequest& rhs) {
    if(this != &rhs) *this = CallGraphRequest(rhs);
    return *this;
}

CallGraphRequest& CallGraphRequest::operator=(CallGraphRequest&& rhs) noexcept {
    if(this == &rhs) return *this;
    m_driver_          = rhs.m_driver_;
    m_requests_        = std::move(rhs.m_requests_);
    m_chains_          = std::move(rhs.m_chains_);
    m_submod_defaults_ = std::move(rhs.m_submod_defaults_);
    m_memoization_     = std::move(rhs.m_memoization_);
    m_claims_          = std::move(rhs.m_claims_);
    m_next_owner_      = rhs.m_next_owner_;
    m_locked_          = rhs.m_locked_;
    adopt_children_();
    return *this;
}

WiringRequest& CallGraphRequest::set_submod(qualified_key module_key,
                                            key_type callback_key,
                                            qualified_key submod_key) {
    assert_unlocked_();
    return add_(WiringRequest::make_set_submod(
      std::move(module_key), std::move(callback_key), std::move(submod_key)));
}

bool CallGraphRequest::operator==(const CallGraphRequest& rhs) const noexcept {
    return m_requests_ == rhs.m_requests_ && m_chains_ == rhs.m_chains_ &&
           m_submod_defaults_ == rhs.m_submod_defaults_ &&
           m_memoization_ == rhs.m_memoization_;
}

WiringRequest& CallGraphRequest::forward_input(qualified_key module_key,
                                               key_type target_input_key,
                                               key_type input_key) {
    assert_driver_input_(input_key);
    auto request = WiringRequest::make_forward_input(
      std::move(module_key), std::move(target_input_key), std::move(input_key));
    claim_(request, m_next_owner_++);
    return add_(std::move(request));
}

WiringRequest& CallGraphRequest::forward_submod(qualified_key module_key,
                                                key_type target_callback_key,
                                                key_type callback_key) {
    assert_driver_submod_(callback_key);
    auto request = WiringRequest::make_forward_submod(
      std::move(module_key), std::move(target_callback_key),
      std::move(callback_key));
    claim_(request, m_next_owner_++);
    return add_(std::move(request));
}

ConditionalWiring& CallGraphRequest::when(key_type input_key, type::any value) {
    m_chains_.push_back(ConditionalWiring(
      *this, m_next_owner_++, std::move(input_key), std::move(value)));
    return m_chains_.back();
}

void CallGraphRequest::wire_memoization(bool on,
                                        std::vector<qualified_key> except) {
    m_memoization_.emplace(MemoizationPolicy{on, std::move(except)});
}

void CallGraphRequest::set_submod_default(key_type callback_key,
                                          qualified_key submod_key) {
    assert_driver_submod_(callback_key);
    m_submod_defaults_.insert_or_assign(std::move(callback_key),
                                        std::move(submod_key));
}

WiringRequest& CallGraphRequest::add_(WiringRequest request) {
    m_requests_.push_back(std::move(request));
    m_requests_.back().m_owner_.value = this;
    return m_requests_.back();
}

void CallGraphRequest::assert_unlocked_() const {
    if(!m_locked_) return;
    throw std::runtime_error(
      "The call graph request can not be changed because its driver is "
      "locked. Change a copy of the driver instead.");
}

void CallGraphRequest::assert_driver_input_(const key_type& key) const {
    if(m_driver_ == nullptr) return;
    if(!m_driver_->inputs().count(key))
        throw std::out_of_range("The driver has no input " + quote(key) +
                                ". Add it with add_input first.");
    if(m_driver_->property_type_inputs().count(key))
        throw std::invalid_argument(
          "The driver's input " + quote(key) +
          " is an input of a property type, so it is passed on each call, "
          "after the driver's requests are resolved. It can not be forwarded "
          "or tested by a condition.");
}

void CallGraphRequest::assert_driver_submod_(const key_type& key) const {
    if(m_driver_ == nullptr || m_driver_->submods().count(key)) return;
    throw std::out_of_range("The driver has no submodule " + quote(key) +
                            ". Add it with add_submodule first.");
}

void CallGraphRequest::claim_(const WiringRequest& request, std::size_t owner) {
    using utilities::strings::case_insensitive_equal;
    for(const auto& claim : m_claims_) {
        if(claim.owner == owner) continue;
        if(claim.is_input != request.sets_input()) continue;
        if(claim.module_key != request.module_key()) continue;
        if(!case_insensitive_equal(claim.target_key, request.target_key()))
            continue;
        throw std::invalid_argument(
          "Can not add " + request.to_string() + ": that " +
          (claim.is_input ? "input" : "submodule") +
          " is already set by another set_target or chain of the driver. "
          "Each target may be set in only one place.");
    }
    m_claims_.push_back(Claim{request.module_key(), request.sets_input(),
                              request.target_key(), owner});
}

void CallGraphRequest::adopt_children_() noexcept {
    for(auto& request : m_requests_) request.m_owner_.value = this;
    for(auto& chain : m_chains_) chain.m_owner_ = this;
}

} // namespace pluginplay
