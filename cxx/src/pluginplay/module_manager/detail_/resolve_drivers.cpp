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

/* This file implements how the ModuleManager resolves the call graph requests
 * of drivers into overlay modules. See the driver and call graph state design
 * documentation.
 */

#include "module_manager_pimpl.hpp"
#include <map>
#include <optional>
#include <set>
#include <stdexcept>
#include <tuple>
#include <utilities/strings/string_tools.hpp>
#include <utility>
#include <vector>

namespace pluginplay::detail_ {
namespace {

using module_ptr = std::shared_ptr<Module>;
using key_set = std::set<std::string, utilities::detail_::CaseInsensitiveLess_>;

/// Quotes @p str for messages
std::string quote(const std::string& str) { return "\"" + str + "\""; }

/// Returns a pointer which shares ownership of @p mod
module_ptr share(const Module& mod) {
    return std::const_pointer_cast<Module>(mod.shared_from_this());
}

/// One request to apply, and what set it
struct Item {
    /// The request
    const WiringRequest* request;

    /// The developer's set_target or chain which made it, none for the user
    std::optional<std::size_t> owner;
};

/** @brief Resolves the requests of one driver.
 *
 *  The resolver keeps the overlays it makes in a map from the module each
 *  overlay points to. While requests are applied, the submodules of overlays
 *  are bound to the ModuleManager's modules, i.e., the originals, so that the
 *  call graph is always described in terms of originals. Once every request
 *  is applied, finish_ binds each module to the overlays of the modules
 *  below it, making more overlays where a module's submodule must change.
 */
class Resolver {
public:
    Resolver(const ModuleManagerPIMPL& mm, Module& driver) :
      m_mm_(mm),
      m_driver_(driver),
      m_pimpl_(ModuleManagerPIMPL::pimpl_of(driver)),
      m_dev_(m_pimpl_.developer_request()),
      m_user_(m_pimpl_.user_request()) {}

    void run() {
        bind_defaults_();
        select_branches_();

        // Submodule requests first, since they decide which modules are in the
        // call graph, i.e., which inputs can be set
        const auto items = items_();
        for(const auto& item : items)
            if(!item.request->sets_input()) apply_submod_(item);
        for(const auto& item : items)
            if(item.request->sets_input()) apply_input_(item);
        apply_memoization_();
        check_cut_off_();

        auto overlays = finish_();
        report_branch_only_settings_();

        if(m_mm_.m_runtime_)
            for(const auto& msg : m_inactive_)
                m_mm_.m_runtime_->logger().warn(msg);
        m_pimpl_.set_resolution(std::move(m_run_), std::move(overlays),
                                std::move(m_inactive_));
    }

private:
    // -------------------------------------------------------------------------
    // -- Steps
    // -------------------------------------------------------------------------

    /// Binds the unset submodules of the driver to their defaults
    void bind_defaults_() {
        m_run_ = m_pimpl_.submods();
        for(const auto& [callback, key] : m_dev_.submod_defaults()) {
            auto& request = m_run_.at(callback);
            if(request.has_module()) continue;
            const auto what = "binding the default of submodule " +
                              quote(callback) + " to " + quote(key.to_string());
            std::string why;
            // A missing plugin leaves the submodule unset, so the driver is
            // not ready, like any module with an unset submodule
            auto mod = find_module_(key, what, why);
            if(!mod) continue;
            try {
                request.change(mod);
            } catch(const std::exception& e) {
                throw std::runtime_error(fail_(what, e.what()));
            }
        }
    }

    /// Finds the first branch of each chain whose condition holds
    void select_branches_() {
        const auto& chains = m_dev_.conditional_wiring();
        for(std::size_t i = 0; i < chains.size(); ++i) {
            for(const auto& branch : chains[i].branches()) {
                if(!holds_(branch)) continue;
                m_taken_.emplace_back(&branch, chain_owner_(i));
                break;
            }
        }
    }

    /// The requests to apply, in the order they are applied
    std::vector<Item> items_() const {
        std::vector<Item> rv;
        const auto& dev = m_dev_.requests();
        for(std::size_t i = 0; i < dev.size(); ++i)
            rv.push_back(Item{&dev[i], i});
        for(const auto& [branch, owner] : m_taken_)
            for(const auto& request : branch->requests())
                rv.push_back(Item{&request, owner});
        for(const auto& request : m_user_.requests())
            rv.push_back(Item{&request, std::nullopt});
        return rv;
    }

    /// Applies a request which sets a submodule
    void apply_submod_(const Item& item) {
        const auto& request = *item.request;
        const auto what     = describe_(request);
        std::string why;
        auto target = find_target_(request.module_key(), what, why);
        if(!target) return inactive_(what, why, request.is_required());

        const auto& key     = request.target_key();
        const auto& submods = current_(*target).submods();
        if(!submods.count(key))
            throw std::out_of_range(
              fail_(what, "the module has no submodule " + quote(key)));

        module_ptr mod;
        if(request.kind() == WiringRequest::kind_type::forward_submod) {
            const auto& source = m_run_.at(request.source_key());
            // Nothing to forward, so the driver is not ready
            if(!source.has_module()) return;
            mod = share(source.value());
        } else {
            mod = find_module_(request.submod_key(), what, why);
            if(!mod) return inactive_(what, why, request.is_required());
        }

        claim_(item, *target, false, key, what);
        m_applied_.emplace_back(target.get(), what, request.is_required());
        const auto& current = submods.at(key);
        if(current.has_module() && &current.value() == mod.get()) return;
        try {
            overlay_(target).override_submod(key, mod);
        } catch(const std::exception& e) {
            throw std::runtime_error(fail_(what, e.what()));
        }
    }

    /// Applies a request which sets an input
    void apply_input_(const Item& item) {
        const auto& request = *item.request;
        const auto what     = describe_(request);
        std::string why;
        auto target = find_target_(request.module_key(), what, why);
        if(!target) return inactive_(what, why, request.is_required());

        const auto& key    = request.target_key();
        const auto& inputs = current_(*target).inputs();
        if(!inputs.count(key))
            throw std::out_of_range(
              fail_(what, "the module has no input " + quote(key)));

        type::any value;
        if(request.kind() == WiringRequest::kind_type::forward_input) {
            const auto& source = m_pimpl_.inputs().at(request.source_key());
            // Nothing to forward, so the driver is not ready
            if(!source.has_value()) return;
            value = source.value<type::any>();
        } else {
            value = request.value();
        }

        claim_(item, *target, true, key, what);
        m_applied_.emplace_back(target.get(), what, request.is_required());
        const auto& current = inputs.at(key);
        if(current.has_value() && current.value<type::any>() == value) return;
        try {
            overlay_(target).override_input(key, std::move(value));
        } catch(const std::exception& e) {
            throw std::invalid_argument(fail_(what, e.what()));
        }
    }

    /// Applies the developer's memoization policy, if there is one
    void apply_memoization_() {
        const auto& policy = m_dev_.memoization_policy();
        if(!policy) return;

        std::set<const Module*> except;
        for(const auto& key : policy->except) {
            const auto what = std::string("turning memoization ") +
                              (policy->on ? "off" : "on") + " for " +
                              quote(key.to_string());
            std::string why;
            auto target = find_target_(key, what, why);
            if(!target) {
                inactive_(what, why, false);
                continue;
            }
            except.insert(target.get());
        }

        for(const auto* mod : region_(true)) {
            const bool on     = except.count(mod) ? !policy->on : policy->on;
            const auto& pimpl = ModuleManagerPIMPL::pimpl_of(current_(*mod));
            if(pimpl.memoizes() == on) continue;
            overlay_(share(*mod)).override_memoization(on);
        }
    }

    /// Reports requests whose targets a later request removed from the graph
    void check_cut_off_() {
        const auto region = region_(true);
        for(const auto& [mod, what, required] : m_applied_) {
            if(region.count(mod)) continue;
            const auto name = mod->has_name() ? quote(mod->get_name()) :
                                                std::string("The module");
            inactive_(what,
                      name + " was removed from the call graph by another "
                             "setting",
                      required);
        }
    }

    /** @brief Binds each module to the overlays of the modules below it.
     *
     *  @return The overlays reachable from the driver.
     */
    std::vector<module_ptr> finish_() {
        std::map<const Module*, module_ptr> bound;
        for(auto& [key, request] : m_run_) {
            if(!request.has_module()) continue;
            auto mod = bind_(request.value(), bound);
            if(mod.get() != &request.value()) request.change(mod);
        }

        std::vector<module_ptr> rv;
        for(const auto& [original, mod] : bound)
            if(mod.get() != original) rv.push_back(mod);
        return rv;
    }

    /// Reports inputs and submodules only forwarded in branches not taken
    void report_branch_only_settings_() {
        key_set always_inputs, always_submods, taken_inputs, taken_submods,
          branch_inputs, branch_submods;
        const auto add = [](const WiringRequest& request, key_set& inputs,
                            key_set& submods) {
            if(!request.is_forward()) return;
            auto& keys = request.sets_input() ? inputs : submods;
            keys.insert(request.source_key());
        };
        for(const auto& request : m_dev_.requests())
            add(request, always_inputs, always_submods);
        for(const auto& chain : m_dev_.conditional_wiring())
            for(const auto& branch : chain.branches())
                for(const auto& request : branch.requests())
                    add(request, branch_inputs, branch_submods);
        for(const auto& [branch, owner] : m_taken_)
            for(const auto& request : branch->requests())
                add(request, taken_inputs, taken_submods);

        const auto& defaults = m_pimpl_.default_inputs();
        for(const auto& key : branch_inputs) {
            if(always_inputs.count(key) || taken_inputs.count(key)) continue;
            // Only report what the user changed
            if(m_pimpl_.inputs().at(key) == defaults.at(key)) continue;
            m_inactive_.push_back(not_taken_(key));
        }
        for(const auto& key : branch_submods) {
            if(always_submods.count(key) || taken_submods.count(key)) continue;
            if(!m_pimpl_.submods().at(key).has_module()) continue;
            m_inactive_.push_back(not_taken_(key));
        }
    }

    // -------------------------------------------------------------------------
    // -- Helpers
    // -------------------------------------------------------------------------

    /// Does the condition of @p branch hold?
    bool holds_(const ConditionalWiring::Branch& branch) const {
        if(!branch.has_condition()) return true;
        const auto& input = m_pimpl_.inputs().at(branch.input_key());
        if(!input.has_value()) return false;
        return input.value<type::any>() == branch.value();
    }

    /// The ID chain @p i claims its targets with
    std::size_t chain_owner_(std::size_t i) const {
        return m_dev_.requests().size() + i;
    }

    /// The overlay of @p mod, if it has one, else @p mod
    const Module& current_(const Module& mod) const {
        auto itr = m_overlays_.find(&mod);
        return itr == m_overlays_.end() ? mod : *itr->second;
    }

    /// The PIMPL of the overlay of @p mod, which is made if needed
    ModulePIMPL& overlay_(const module_ptr& mod) {
        auto& overlay = m_overlays_[mod.get()];
        if(!overlay) overlay = ModulePIMPL::make_overlay(mod);
        return ModuleManagerPIMPL::pimpl_of(*overlay);
    }

    /** @brief The modules reachable from the driver.
     *
     *  @param[in] stop_at_drivers If true, the modules below nested drivers
     *                             are left out. Nested drivers are not.
     */
    std::set<const Module*> region_(bool stop_at_drivers) const {
        std::set<const Module*> rv;
        std::vector<const Module*> stack;
        for(const auto& [key, request] : m_run_)
            if(request.has_module()) stack.push_back(&request.value());
        while(!stack.empty()) {
            const auto* mod = stack.back();
            stack.pop_back();
            if(!rv.insert(mod).second) continue;
            const bool is_driver = mod->is_driver();
            if(stop_at_drivers && is_driver) continue;

            const auto& current = current_(*mod);
            for(const auto& [key, request] : current.submods()) {
                if(request.has_module()) {
                    stack.push_back(&request.value());
                    continue;
                }
                // A nested driver binds its unset submodules to their defaults
                // when its requests are resolved
                if(!is_driver) continue;
                const auto& defaults = ModuleManagerPIMPL::pimpl_of(current)
                                         .developer_request()
                                         .submod_defaults();
                auto itr = defaults.find(key);
                if(itr == defaults.end() || m_mm_.count(itr->second) != 1)
                    continue;
                stack.push_back(m_mm_.at(itr->second).get());
            }
        }
        return rv;
    }

    /** @brief Finds the module with key @p key.
     *
     *  @return The module, or null if its plugin is not loaded, in which case
     *          @p why says so.
     *
     *  @throw std::out_of_range if no module has the key.
     *  @throw std::invalid_argument if the key is ambiguous.
     */
    module_ptr find_module_(const QualifiedKey& key, const std::string& what,
                            std::string& why) const {
        try {
            return m_mm_.at(key);
        } catch(const std::out_of_range& e) {
            if(key.is_qualified() && !key.plugin().empty() &&
               !m_mm_.has_plugin(key.plugin())) {
                why = "plugin " + quote(key.plugin()) + " is not loaded";
                return nullptr;
            }
            throw std::out_of_range(fail_(what, e.what()));
        } catch(const std::invalid_argument& e) {
            throw std::invalid_argument(fail_(what, e.what()));
        }
    }

    /** @brief Finds the target module with key @p key.
     *
     *  @return The module, or null if the setting is inactive, in which case
     *          @p why says why.
     *
     *  @throw std::runtime_error if the module is only reachable below a
     *                            nested driver.
     */
    module_ptr find_target_(const QualifiedKey& key, const std::string& what,
                            std::string& why) const {
        auto mod = find_module_(key, what, why);
        if(!mod) return nullptr;
        if(mod.get() == &m_driver_ ||
           (m_pimpl_.is_overlay() && mod.get() == &m_pimpl_.original()))
            throw std::invalid_argument(
              fail_(what, "a driver's own inputs and submodules are set with "
                          "change_input and change_submod"));
        if(region_(true).count(mod.get())) return mod;
        if(region_(false).count(mod.get()))
            throw std::runtime_error(fail_(
              what, quote(key.to_string()) +
                      " is below a nested driver. Configure it through that "
                      "driver's inputs and submodules instead."));
        why = quote(key.to_string()) + " is not in the call graph";
        return nullptr;
    }

    /// Records that @p item sets the target, or throws if another does
    void claim_(const Item& item, const Module& target, bool is_input,
                const type::key& key, const std::string& what) {
        // The user's requests override the developer's on purpose
        if(!item.owner) return;
        auto id = std::make_tuple(&target, is_input,
                                  utilities::strings::tolower_string(key));
        auto [itr, is_new] =
          m_owners_.emplace(id, std::pair{*item.owner, what});
        if(is_new || itr->second.first == *item.owner) return;
        throw std::invalid_argument(fail_(
          what, "the same target is set by " + itr->second.second +
                  ". Each target may be set by only one set_target or chain."));
    }

    /// Binds @p mod, and the modules below it, to their overlays
    module_ptr bind_(const Module& mod,
                     std::map<const Module*, module_ptr>& bound) {
        if(auto itr = bound.find(&mod); itr != bound.end()) return itr->second;

        auto itr      = m_overlays_.find(&mod);
        module_ptr rv = itr == m_overlays_.end() ? share(mod) : itr->second;
        // Nested drivers configure the modules below them themselves
        if(!mod.is_driver()) {
            std::vector<type::key> keys;
            for(const auto& [key, request] : current_(mod).submods())
                keys.push_back(key);
            for(const auto& key : keys) {
                const auto& request = current_(mod).submods().at(key);
                if(!request.has_module()) continue;
                const auto& child = request.value();
                auto new_child    = bind_(child, bound);
                if(new_child.get() == &child) continue;
                ModulePIMPL& pimpl = overlay_(share(mod));
                rv                 = m_overlays_.at(&mod);
                pimpl.override_submod(key, new_child);
            }
        }
        bound.emplace(&mod, rv);
        return rv;
    }

    /// Describes @p request for messages
    static std::string describe_(const WiringRequest& request) {
        return request.to_string();
    }

    /// The name of the driver for messages
    std::string driver_name_() const {
        return m_driver_.has_name() ? quote(m_driver_.get_name()) :
                                      std::string("Unnamed driver");
    }

    /// The message for a failed request
    std::string fail_(const std::string& what,
                      const std::string& reason) const {
        return driver_name_() + ": " + what + " failed: " + reason;
    }

    /// The message for a driver input forwarded only in branches not taken
    std::string not_taken_(const type::key& key) const {
        return driver_name_() + ": " + quote(key) +
               " has no effect because no branch that forwards it was taken";
    }

    /// Records, or throws for, a setting which has no effect
    void inactive_(const std::string& what, const std::string& why,
                   bool required) {
        auto msg =
          driver_name_() + ": " + what + " has no effect because " + why;
        if(required) throw std::runtime_error(msg + ", but it is required");
        m_inactive_.push_back(std::move(msg));
    }

    /// The ModuleManager resolving the driver
    const ModuleManagerPIMPL& m_mm_;

    /// The driver
    Module& m_driver_;

    /// The PIMPL of the driver
    ModulePIMPL& m_pimpl_;

    /// The developer's request
    const CallGraphRequest& m_dev_;

    /// The user's request
    const CallGraphRequest& m_user_;

    /// The submodules the driver will run with
    type::submodule_map m_run_;

    /// The taken branches, with the IDs of their chains
    std::vector<std::pair<const ConditionalWiring::Branch*, std::size_t>>
      m_taken_;

    /// The overlays, keyed by the module they point to
    std::map<const Module*, module_ptr> m_overlays_;

    /// The targets set by the developer's requests, with what set them
    std::map<std::tuple<const Module*, bool, std::string>,
             std::pair<std::size_t, std::string>>
      m_owners_;

    /// The targets of the applied requests, with descriptions of the requests
    std::vector<std::tuple<const Module*, std::string, bool>> m_applied_;

    /// The settings with no effect
    std::vector<std::string> m_inactive_;
};

} // namespace

void ModuleManagerPIMPL::resolve_drivers(Module& root) const {
    std::set<const Module*> visited;
    std::vector<Module*> stack{&root};
    while(!stack.empty()) {
        auto* mod = stack.back();
        stack.pop_back();
        if(!visited.insert(mod).second) continue;

        auto& pimpl = pimpl_of(*mod);
        if(!pimpl.has_module()) continue;
        // The requests of an unlocked driver are resolved again, since the
        // modules they name may have changed since they were last resolved
        if(pimpl.is_driver() && !(pimpl.locked() && pimpl.is_resolved()))
            Resolver(*this, *mod).run();

        // Below a driver, this visits its overlays, so drivers are resolved
        // from the outermost inward
        for(const auto& [key, request] : pimpl.run_submods())
            if(request.has_module())
                stack.push_back(&const_cast<Module&>(request.value()));
    }
}

} // namespace pluginplay::detail_
