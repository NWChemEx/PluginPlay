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

// This file meant only for inclusion from module_pimpl.hpp

namespace pluginplay::detail_ {

inline ModulePIMPL::ModulePIMPL(base_ptr base, cache_ptr cache) :
  m_base_(base),
  m_cache_(cache),
  m_inputs_(base ? base->inputs() : type::input_map{}),
  m_submods_(base ? base->submods() : type::submodule_map{}),
  m_property_types_(base ? base->property_types() : std::set<type::rtti>{}),
  m_python_property_types_(base ? base->python_property_types() :
                                  std::set<std::string>{}) {}

inline ModulePIMPL::ModulePIMPL(const ModulePIMPL& rhs) :
  m_locked_(rhs.m_locked_),
  m_memoizable_(rhs.m_memoizable_),
  m_base_(rhs.m_base_),
  m_cache_(rhs.m_cache_),
  // An overlay's inputs refer to values it does not own, so they are remade
  // by sync_overlay_ below rather than copied
  m_inputs_(rhs.m_overlay_ ? type::input_map{} : rhs.m_inputs_),
  m_submods_(rhs.m_submods_),
  m_property_types_(rhs.m_property_types_),
  m_python_property_types_(rhs.m_python_property_types_),
  m_timer_(rhs.m_timer_),
  m_user_request_(rhs.m_user_request_),
  m_resolved_(rhs.m_resolved_),
  m_overlay_(rhs.m_overlay_),
  m_fingerprints_(rhs.m_fingerprints_),
  m_fingerprint_(rhs.m_fingerprint_) {
    if(m_overlay_) sync_overlay_();
}

inline ModulePIMPL& ModulePIMPL::operator=(const ModulePIMPL& rhs) {
    if(this != &rhs) *this = ModulePIMPL(rhs);
    return *this;
}

inline bool ModulePIMPL::has_description() const {
    assert_mod_();
    return m_base_->has_description();
}

inline auto ModulePIMPL::not_set(const type::input_map& in_inputs) const {
    assert_mod_();
    not_set_type probs;

    // This is all of the not ready inputs
    auto in_probs = not_set_guts_(m_inputs_);

    // Now pull out those set by the property type
    for(const auto& [k, v] : in_inputs)
        if(in_probs.count(k)) in_probs.erase(k);
    if(!in_probs.empty()) probs.emplace("Inputs", std::move(in_probs));

    auto submod_probs = not_set_guts_(run_submods());
    if(!submod_probs.empty())
        probs.emplace("Submodules", std::move(submod_probs));
    return probs;
}

inline bool ModulePIMPL::ready(const type::input_map& inps) const {
    auto errors = not_set(inps);
    return errors.empty();
}

inline const auto& ModulePIMPL::results() const {
    assert_mod_();
    return m_base_->results();
}

inline auto& ModulePIMPL::inputs() {
    assert_mod_();
    return m_inputs_;
}

inline auto& ModulePIMPL::inputs() const {
    assert_mod_();
    return m_inputs_;
}

inline auto& ModulePIMPL::submods() {
    assert_mod_();
    return m_submods_;
}

inline auto& ModulePIMPL::submods() const {
    assert_mod_();
    return m_submods_;
}

inline auto& ModulePIMPL::property_types() {
    assert_mod_();
    return m_property_types_;
}

inline auto& ModulePIMPL::property_types() const {
    assert_mod_();
    return m_property_types_;
}

inline auto& ModulePIMPL::python_property_types() {
    assert_mod_();
    return m_python_property_types_;
}

inline auto& ModulePIMPL::python_property_types() const {
    assert_mod_();
    return m_python_property_types_;
}

inline auto& ModulePIMPL::description() const {
    assert_mod_();
    return m_base_->get_desc();
}

inline auto& ModulePIMPL::citations() const {
    assert_mod_();
    return m_base_->citations();
}

inline bool ModulePIMPL::is_memoizable() const {
    assert_mod_();
    if(!m_base_->has_uuid()) return false;
    auto memoizable = m_memoizable_;
    for(const auto& [k, v] : run_submods()) {
        memoizable = v.value().is_memoizable() && memoizable;
    }
    return memoizable;
}

inline bool ModulePIMPL::is_cached(const type::input_map& in_inputs) {
    if(!m_cache_) return false;
    auto ps = merge_inputs_(in_inputs);
    return m_cache_->count(ps);
}

inline void ModulePIMPL::attach(runtime_ptr runtime, const mm_cache_ptr& caches,
                                const type::key& cache_key,
                                fingerprint_table_ptr fingerprints) {
    assert_mod_();
    if(fingerprints) m_fingerprints_ = std::move(fingerprints);
    m_base_->set_runtime(runtime);
    if(!m_base_->has_uuid()) m_base_->set_uuid(utility::generate_uuid());
    if(!caches) return;
    m_base_->set_cache(caches->get_or_make_user_cache(m_base_->uuid()));
    m_cache_ = caches->get_or_make_module_cache(cache_key);
}

inline void ModulePIMPL::reset_cache() {
    if(m_cache_) m_cache_->clear();
}

inline void ModulePIMPL::reset_internal_cache() {
    assert_mod_();
    m_base_->reset_internal_cache();
}

inline void ModulePIMPL::turn_off_memoization() {
    assert_mod_();
    m_memoizable_ = false;
}

inline void ModulePIMPL::turn_on_memoization() {
    assert_mod_();
    m_memoizable_ = true;
}

inline std::string ModulePIMPL::profile_info() const {
    std::stringstream ss;
    ss << m_timer_;
    std::string tab("  ");
    for(auto [key, submod] : run_submods()) {
        ss << tab << key << std::endl;
        auto submod_prof_info = submod.value().profile_info();
        std::stringstream ss2(submod_prof_info);
        std::string token;
        while(std::getline(ss2, token, '\n'))
            ss << tab << tab << token << std::endl;
    }
    return ss.str();
}

inline auto ModulePIMPL::run(type::input_map ps) {
    auto time_now = time_stamp();
    m_timer_.reset();
    assert_mod_();
    if(is_driver() && !m_resolved_)
        throw std::runtime_error(
          "A driver must be run through a ModuleManager, i.e., with "
          "ModuleManager::run_as or as a submodule of a module that was, so "
          "that its wiring requests can be resolved.");

    // Check the inputs we were just given
    for(const auto& [k, v] : ps)
        if(!v.ready()) throw std::runtime_error("Inputs are not ready");

    // Merge with bound and see if we are ready
    if(!ready(ps)) {
        // Make a dummy module with this PIMPL so we can print out why it's not
        // ready.
        Module dummy(std::make_unique<ModulePIMPL>(*this));
        throw std::runtime_error(print_not_ready(dummy, ps));
    }

    lock();

    ps = merge_inputs_(ps);

    if(is_memoizable() && m_cache_ && m_cache_->count(ps)) {
        m_timer_.record(time_now);
        return m_cache_->uncache(ps);
    }

    // not there so run
    auto rv = m_base_->run(ps, active_submods_());

    if(!m_cache_ || !is_memoizable()) {
        m_timer_.record(time_now);
        return rv;
    }

    // cache result
    m_cache_->cache(ps, std::move(rv));
    m_timer_.record(time_now);
    return m_cache_->uncache(ps);
}

inline bool ModulePIMPL::operator==(const ModulePIMPL& rhs) const {
    if(has_module() != rhs.has_module()) return false;
    if(locked() != rhs.locked()) return false;
    if(!has_module()) return true;
    if(m_user_request_ != rhs.m_user_request_) return false;

    if(std::tie(inputs(), submods(), property_types(),
                python_property_types()) !=
       std::tie(rhs.inputs(), rhs.submods(), rhs.property_types(),
                rhs.python_property_types()))
        return false;
    return (*m_base_ == *rhs.m_base_);
}

inline typename ModulePIMPL::rtti_type ModulePIMPL::type() const {
    assert_mod_();
    return m_base_->type();
}

inline typename ModulePIMPL::uuid_type ModulePIMPL::uuid() const {
    assert_mod_();
    return m_base_->uuid();
}

inline typename ModulePIMPL::submod_uuid_map ModulePIMPL::submod_uuids() const {
    submod_uuid_map rv;
    for(const auto& [k, v] : submods()) {
        // Prepend the submodule key to each of its submodules' keys
        // N.B. This essentially scopes the sub submodule keys
        for(const auto& [sub_k, sub_v] : v.submod_uuids()) {
            rv.emplace(k + ":" + sub_k, sub_v);
        }
        rv.emplace(k, v.uuid());
    }
    return rv;
}

inline type::input_map ModulePIMPL::merge_inputs_(
  type::input_map in_inputs) const {
    for(const auto& [k, v] : m_inputs_)
        if(!in_inputs.count(k)) in_inputs[k] = v;

    // TODO: It probably makes sense to create an Input class which tracks this
    //       and allows using submods as inputs
    // The fingerprints identify the state of everything below each submodule
    submod_uuid_map fingerprints;
    for(const auto& [k, v] : run_submods())
        fingerprints.emplace(
          k, v.has_module() ? v.value().m_pimpl_->fingerprint() : "");

    std::string submod_key = "__PLUGIN_PLAY__ SUBMOD KEYS __PLUGIN_PLAY__";
    ModuleInput temp;
    temp.set_type<submod_uuid_map>();
    temp.change(std::move(fingerprints));
    in_inputs.emplace(submod_key, temp);
    return in_inputs;
}

inline void ModulePIMPL::lock() {
    // An overlay refers to its original's inputs, which must not change
    if(m_overlay_ && !m_locked_) {
        lock_original_(*m_overlay_->original);
        sync_overlay_();
    }
    for(auto& [k, v] : active_submods_()) v.lock();
    m_locked_ = true;
}

inline void ModulePIMPL::unlock() {
    if(m_overlay_) {
        // Copying an input copies the value it refers to
        type::input_map inputs(m_inputs_);
        m_inputs_.swap(inputs);
        m_overlay_.reset();
    }
    m_resolved_.reset();
    m_fingerprint_.reset();
    m_locked_ = false;
}

// -----------------------------------------------------------------------------
// -- Drivers
// -----------------------------------------------------------------------------

inline bool ModulePIMPL::is_driver() const {
    assert_mod_();
    return dynamic_cast<const DriverBase*>(m_base_.get()) != nullptr;
}

inline const CallGraphRequest& ModulePIMPL::developer_request() const {
    assert_mod_();
    return m_base_->call_graph_request();
}

inline void ModulePIMPL::set_resolution(type::submodule_map run_submods,
                                        std::vector<module_ptr> overlays,
                                        inactive_list inactive) noexcept {
    m_resolved_.emplace(DriverState{std::move(run_submods), std::move(overlays),
                                    std::move(inactive)});
    m_fingerprint_.reset();
}

inline const type::submodule_map& ModulePIMPL::run_submods() const {
    if(!m_resolved_) return m_submods_;
    if(!m_locked_) sync_overlays_();
    return m_resolved_->run_submods;
}

inline const ModulePIMPL::inactive_list& ModulePIMPL::inactive_settings()
  const noexcept {
    static const inactive_list none;
    return m_resolved_ ? m_resolved_->inactive : none;
}

inline type::submodule_map& ModulePIMPL::active_submods_() {
    const auto& rv = std::as_const(*this).run_submods();
    return const_cast<type::submodule_map&>(rv);
}

// -----------------------------------------------------------------------------
// -- Overlay modules
// -----------------------------------------------------------------------------

inline ModulePIMPL::module_ptr ModulePIMPL::make_overlay(
  const module_ptr& original) {
    const auto& rhs = *original->m_pimpl_;
    rhs.assert_mod_();

    auto pimpl                      = std::make_unique<ModulePIMPL>();
    pimpl->m_memoizable_            = rhs.m_memoizable_;
    pimpl->m_base_                  = rhs.m_base_;
    pimpl->m_cache_                 = rhs.m_cache_;
    pimpl->m_property_types_        = rhs.m_property_types_;
    pimpl->m_python_property_types_ = rhs.m_python_property_types_;
    // An overlay of a driver is a driver, which resolves the same requests
    pimpl->m_user_request_ = rhs.m_user_request_;
    pimpl->m_fingerprints_ = rhs.m_fingerprints_;
    pimpl->m_overlay_.emplace(OverlayState{original, {}, {}, std::nullopt});
    pimpl->sync_overlay_();

    auto rv = std::make_shared<Module>(std::move(pimpl));
    if(original->has_name()) rv->set_name(original->get_name());
    return rv;
}

inline const Module& ModulePIMPL::original() const {
    if(!m_overlay_) throw std::runtime_error("Module is not an overlay");
    return *m_overlay_->original;
}

inline void ModulePIMPL::override_input(const type::key& key, type::any value) {
    assert_unlocked_overlay_();
    const auto& inputs = m_overlay_->original->m_pimpl_->m_inputs_;
    auto itr           = inputs.find(key);
    if(itr == inputs.end())
        throw std::out_of_range("Module has no input \"" + key + "\"");

    // Keeps the type and bounds checks of the original's input
    auto input = itr->second.cref_clone();
    input.change(std::move(value));
    m_overlay_->input_overrides.insert_or_assign(key, std::move(input));
    sync_overlay_();
}

inline void ModulePIMPL::override_submod(const type::key& key, module_ptr mod) {
    assert_unlocked_overlay_();
    const auto& submods = m_overlay_->original->m_pimpl_->m_submods_;
    auto itr            = submods.find(key);
    if(itr == submods.end())
        throw std::out_of_range("Module has no submodule \"" + key + "\"");

    SubmoduleRequest request(itr->second);
    request.change(std::move(mod));
    m_overlay_->submod_overrides.insert_or_assign(key, std::move(request));
    sync_overlay_();
}

inline void ModulePIMPL::override_memoization(bool on) {
    assert_unlocked_overlay_();
    m_overlay_->memoizes = on;
    sync_overlay_();
}

inline ModulePIMPL::fingerprint_type ModulePIMPL::fingerprint() const {
    bool is_final = false;
    return fingerprint_(is_final);
}

inline void ModulePIMPL::sync_overlay_() {
    const auto& rhs = *m_overlay_->original->m_pimpl_;

    // Inputs the overlay does not change refer to the original's values
    type::input_map inputs;
    for(const auto& [k, v] : rhs.m_inputs_) inputs.emplace(k, v.cref_clone());
    for(const auto& [k, v] : m_overlay_->input_overrides)
        inputs.insert_or_assign(k, v.cref_clone());
    m_inputs_.swap(inputs);

    auto submods = rhs.m_submods_;
    for(const auto& [k, v] : m_overlay_->submod_overrides)
        submods.insert_or_assign(k, v);
    m_submods_.swap(submods);

    m_memoizable_ = m_overlay_->memoizes.value_or(rhs.m_memoizable_);
    m_fingerprint_.reset();
}

inline void ModulePIMPL::sync_overlays_() const {
    // N.B. The originals of an unlocked driver's overlays may have changed
    // since the driver's requests were resolved, e.g., after a failed run.
    // Syncing keeps the overlays from referring to values that are gone.
    if(!m_resolved_) return;
    for(const auto& overlay : m_resolved_->overlays) {
        auto& pimpl = *overlay->m_pimpl_;
        if(pimpl.m_locked_) continue;
        pimpl.sync_overlay_();
        pimpl.sync_overlays_();
    }
}

inline void ModulePIMPL::assert_unlocked_overlay_() const {
    if(!m_overlay_) throw std::runtime_error("Module is not an overlay");
    if(m_locked_)
        throw std::runtime_error("Locked modules can not be modified");
}

inline ModulePIMPL::fingerprint_type ModulePIMPL::fingerprint_(
  bool& is_final) const {
    if(m_fingerprint_) {
        is_final = true;
        return *m_fingerprint_;
    }
    assert_mod_();

    const auto impl = m_base_->has_uuid() ? m_base_->uuid() : type().name();
    FingerprintTable::Key key{impl, m_memoizable_, {}};
    is_final = m_locked_;
    for(const auto& [k, v] : run_submods()) {
        if(!v.has_module()) {
            key.submods.emplace(k, "");
            is_final = false;
            continue;
        }
        bool child_is_final = false;
        key.submods.emplace(k,
                            v.value().m_pimpl_->fingerprint_(child_is_final));
        is_final = is_final && child_is_final;
    }

    if(!m_fingerprints_) m_fingerprints_ = std::make_shared<FingerprintTable>();
    auto rv = m_fingerprints_->intern(std::move(key), m_inputs_);
    if(is_final) m_fingerprint_ = rv;
    return rv;
}

inline void ModulePIMPL::lock_original_(Module& original) {
    // The original may not be ready on its own, e.g., if the overlay binds a
    // submodule the original leaves unset. Its inputs are what the overlay
    // refers to, so it is locked, and its submodules are locked if they can be.
    auto& pimpl = *original.m_pimpl_;
    if(pimpl.m_locked_) return;
    if(pimpl.m_overlay_) {
        lock_original_(*pimpl.m_overlay_->original);
        pimpl.sync_overlay_();
    }
    for(auto& [k, v] : pimpl.active_submods_())
        if(v.ready()) v.lock();
    pimpl.m_locked_ = true;
}

template<typename T>
std::set<type::key> ModulePIMPL::not_set_guts_(T&& map) const {
    std::set<type::key> probs;
    for(const auto& [k, v] : map)
        if(!v.ready()) probs.insert(k);
    return probs;
}

inline void ModulePIMPL::assert_mod_() const {
    if(has_module()) return;
    throw std::runtime_error("Module does not contain an implementation");
}

} // namespace pluginplay::detail_
