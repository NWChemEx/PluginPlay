// This file meant only for inclusion from module_pimpl.hpp

namespace pluginplay::detail_ {

inline ModulePIMPL::ModulePIMPL(base_ptr base, cache_ptr cache) :
  m_base_(base),
  m_cache_(cache),
  m_inputs_(base ? base->inputs() : type::input_map{}),
  m_submods_(base ? base->submods() : type::submodule_map{}),
  m_property_types_(base ? base->property_types() : std::set<type::rtti>{}) {}

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

    auto submod_probs = not_set_guts_(m_submods_);
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
    for(const auto& [k, v] : m_submods_) {
        memoizable = v.value().is_memoizable() && memoizable;
    }
    return memoizable;
}

inline bool ModulePIMPL::is_cached(const type::input_map& in_inputs) {
    if(!m_cache_) return false;
    auto ps = merge_inputs_(in_inputs);
    return m_cache_->count(ps);
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
    for(auto [key, submod] : m_submods_) {
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
    //DEBUG
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
    auto rv = m_base_->run(ps, m_submods_);
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

    if(std::tie(inputs(), submods(), property_types()) !=
       std::tie(rhs.inputs(), rhs.submods(), rhs.property_types()))
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
    std::string submod_key = "__PLUGIN_PLAY__ SUBMOD KEYS __PLUGIN_PLAY__";
    ModuleInput temp;
    temp.set_type<submod_uuid_map>();
    temp.change(submod_uuids());
    in_inputs.emplace(submod_key, temp);
    return in_inputs;
}

inline void ModulePIMPL::lock() {
    for(auto& [k, v] : m_submods_) v.lock();
    m_locked_ = true;
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
