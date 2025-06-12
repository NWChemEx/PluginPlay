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

#include "detail_/module_pimpl.hpp"
#include <pluginplay/module/module_class.hpp>
#include <utilities/printing/demangler.hpp>

namespace pluginplay {
using pimpl = detail_::ModulePIMPL;

//-----------------------Ctors and Dtor-----------------------------------------

Module::Module() : m_pimpl_(std::make_unique<pimpl>()) {}

Module::Module(const Module& rhs) :
  m_pimpl_(std::make_unique<pimpl>(*rhs.m_pimpl_)), m_name_(rhs.m_name_) {}

Module& Module::operator=(const Module& rhs) {
    std::make_unique<detail_::ModulePIMPL>(*rhs.m_pimpl_).swap(m_pimpl_);
    if(rhs.has_name()) m_name_ = rhs.m_name_;
    return *this;
}

Module::Module(Module&& rhs) noexcept = default;

Module& Module::operator=(Module&& rhs) noexcept = default;

Module::Module(std::unique_ptr<ModuleBase> ptr) :
  Module(std::make_unique<pimpl>(std::move(ptr), nullptr)) {}

Module::Module(pimpl_ptr pimpl) noexcept : m_pimpl_(std::move(pimpl)) {}

Module::~Module() noexcept = default;

//--------------------------Inquirers-------------------------------------------

bool Module::has_module() const noexcept { return m_pimpl_->has_module(); }

bool Module::has_description() const { return m_pimpl_->has_description(); }

bool Module::locked() const noexcept { return m_pimpl_->locked(); }

typename Module::not_ready_type Module::list_not_ready(
  const type::input_map& inputs) const {
    return m_pimpl_->not_set(inputs);
}

bool Module::ready(const type::input_map& inps) const {
    return m_pimpl_->ready(inps);
}

void Module::reset_cache() { m_pimpl_->reset_cache(); }

void Module::reset_internal_cache() { m_pimpl_->reset_internal_cache(); }

bool Module::is_memoizable() const { return m_pimpl_->is_memoizable(); }

//--------------------Setters--------------------------------------------------

void Module::turn_on_memoization() { m_pimpl_->turn_on_memoization(); }

void Module::turn_off_memoization() { m_pimpl_->turn_off_memoization(); }

void Module::lock() { m_pimpl_->lock(); }

void Module::change_submod(type::key key, std::shared_ptr<Module> new_module) {
    assert_not_locked_();
    m_pimpl_->submods().at(key).change(new_module);
}

//-------------------------------Getters----------------------------------------
const type::result_map& Module::results() const { return m_pimpl_->results(); }

const type::input_map& Module::inputs() const { return m_pimpl_->inputs(); }

const type::submodule_map& Module::submods() const {
    return m_pimpl_->submods();
}
const std::set<type::rtti>& Module::property_types() const {
    return m_pimpl_->property_types();
}

type::result_map Module::run(type::input_map ps) {
    return m_pimpl_->run(std::move(ps));
}

bool Module::operator==(const Module& rhs) const {
    return (*m_pimpl_ == *rhs.m_pimpl_) && (m_name_ == rhs.m_name_);
}

const type::description& Module::description() const {
    return m_pimpl_->description();
}

const std::vector<type::description>& Module::citations() const {
    return m_pimpl_->citations();
}

std::string Module::profile_info() const { return m_pimpl_->profile_info(); }

typename Module::uuid_type Module::uuid() const {
    if(!m_pimpl_) return uuid_type{};
    return m_pimpl_->uuid();
}

typename Module::submod_uuid_map Module::submod_uuids() const {
    if(!m_pimpl_) return submod_uuid_map{};
    return m_pimpl_->submod_uuids();
}

//--------------------------- Private Members --------------------------------/

void Module::unlock_() noexcept { m_pimpl_->unlock(); }

ModuleInput& Module::get_input_(const type::key& key) {
    return m_pimpl_->inputs().at(key);
}

void Module::add_property_type_(pluginplay::type::rtti prop_type) {
    m_pimpl_->property_types().insert(prop_type);
}

void Module::check_property_type_(type::rtti prop_type) {
    if(property_types().count(prop_type)) return;

    // Work around for libc++ typeid inconsistency in Python
    for(const auto& m_prop_type : property_types()) {
        if(prop_type.name() == m_prop_type.name() ||
           std::strcmp(prop_type.name(), m_prop_type.name()) == 0)
            return;
    }

    std::string msg = "Module does not satisfy property type ";
    msg += utilities::printing::Demangler::demangle(prop_type);
    throw std::runtime_error(msg);
}

std::string print_not_ready(const Module& mod, const type::input_map& ps,
                            const std::string& indent) {
    std::string rv      = "";
    auto not_readies    = mod.list_not_ready(ps);
    std::string indent2 = "  " + indent;
    for(const auto& [desc, not_ready_items] : not_readies) {
        rv += indent + desc + " that are not ready:\n";
        for(const auto& not_ready_item : not_ready_items) {
            if(desc != "Submodules") {
                rv += indent2 + not_ready_item + "\n";
            } else {
                auto submod = mod.submods().at(not_ready_item);
                rv += indent2 + not_ready_item;
                if(!submod.has_module()) {
                    rv += " is not set.\n";
                } else { // It's set, so recurse to find out why it's not ready
                    rv += ":\n";
                    rv += print_not_ready(submod.value(), type::input_map{},
                                          indent2 + "  ");
                }
            }
        }
    }
    return rv;
}

} // namespace pluginplay
