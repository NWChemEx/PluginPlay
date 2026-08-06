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

// To be included only from module_input_pimpl.hpp
#include <sstream>

namespace pluginplay::detail_ {

//-----------------------------Implementations----------------------------------

inline bool ModuleInputPIMPL::is_valid(const type::any& new_value) const {
    assert_type_set_();
    for(const auto& [k, v] : m_checks_)
        if(!v(new_value)) return false;
    return true;
}

inline void ModuleInputPIMPL::set_type(
  typename ModuleInputPIMPL::rtti_type type) {
    if(has_type() && has_value() && m_type_ != type)
        throw std::runtime_error("Cannot change type after value is set.");
    m_type_ = type;
}

inline void ModuleInputPIMPL::set_value(type::any any) {
    assert_type_set_();
    if(!is_valid(any)) {
        std::string msg("Input value: \"");
        std::stringstream ss;
        any.print(ss);
        msg += ss.str();
        msg += std::string("\" has failed bounds checks: ");
        for(const auto& [x, y] : m_checks_) {
            if(!y(any)) msg += x + " ";
        }
        throw std::invalid_argument(msg);
    }
    m_value_.swap(any);
}

inline void ModuleInputPIMPL::set_description(type::description desc) noexcept {
    m_desc_.emplace(std::move(desc));
}

inline void ModuleInputPIMPL::add_check(any_check check,
                                        type::description desc) {
    if(has_value())
        if(!check(m_value_)) {
            const auto msg = std::string("Value failed provided bounds "
                                         "check: ") +
                             desc;
            throw std::invalid_argument(desc);
        }
    m_checks_.emplace(std::move(desc), std::move(check));
}

inline typename ModuleInputPIMPL::rtti_type ModuleInputPIMPL::type() const {
    assert_type_set_();
    return m_type_.value();
}

inline const type::any& ModuleInputPIMPL::value() const {
    assert_value_set_();
    return m_value_;
}

inline void ModuleInputPIMPL::assert_type_set_() const {
    if(!has_type()) throw std::runtime_error("Must set type first");
}

inline void ModuleInputPIMPL::assert_value_set_() const {
    if(!has_value()) throw std::runtime_error("Value has not been set");
}

inline typename ModuleInputPIMPL::check_description_type
ModuleInputPIMPL::check_descriptions() const {
    check_description_type rv;
    for(const auto& [x, _] : m_checks_) rv.insert(x);
    return rv;
}

inline bool operator==(const ModuleInputPIMPL& lhs,
                       const ModuleInputPIMPL& rhs) noexcept {
    // Check boolean properties first
    if(lhs.has_type() != rhs.has_type()) return false;
    if(lhs.has_value() != rhs.has_value()) return false;
    if(lhs.is_optional() != rhs.is_optional()) return false;
    if(lhs.is_transparent() != rhs.is_transparent()) return false;
    if(lhs.has_description() != rhs.has_description()) return false;

    if(lhs.has_type() && (lhs.type() != rhs.type())) return false;
    if(lhs.has_value() && (lhs.value() != rhs.value())) return false;
    if(lhs.has_description() && (lhs.description() != rhs.description()))
        return false;

    return true;
}

inline bool operator!=(const ModuleInputPIMPL& lhs,
                       const ModuleInputPIMPL& rhs) noexcept {
    return !(lhs == rhs);
}

} // namespace pluginplay::detail_
