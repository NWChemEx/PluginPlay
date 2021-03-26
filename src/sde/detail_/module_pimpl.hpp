#pragma once
#include "sde/module_base.hpp"
#include "sde/types.hpp"
#include <cereal/access.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/optional.hpp> // std::optional
#include <cereal/types/unordered_map.hpp>
#include <iomanip> // for put_time
#include <utilities/timer.hpp>

namespace sde::detail_ {

// TODO: This function doesn't belong here (move unit test too)
    /// The set of property types that his module satisfies
    std::set<type::rtti> m_property_types_;

    /// Timer used to time runs of this module
    utilities::Timer m_timer_;

    friend class cereal::access;

    template<class Archive>
    void save(Archive& ar) const;

    template<class Archive>
    void load(Archive& ar);

}; // class ModulePIMPL

//-------------------------------Implementations--------------------------------

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
    auto memoizable = m_memoizable_;
    for(const auto& [k, v] : m_submods_) {
        memoizable = v.value().is_memoizable() && memoizable;
    }
    return memoizable;
}

inline void ModulePIMPL::memoize(type::hasher& h,
                                 type::input_map inputs) const {
    inputs = merge_inputs_(std::move(inputs));
    for(const auto& [k, v] : inputs) v.hash(h);
    for(const auto& [k, v] : m_submods_) v.hash(h);
    // This is not a great way of hashing the class name...
    h(m_base_->type().name());
}

inline bool ModulePIMPL::is_cached(const type::input_map& in_inputs) {
    if(!m_cache_) return false;
    auto ps = merge_inputs_(in_inputs);
    return m_cache_->count(get_hash_(ps)) == 1;
}

inline void ModulePIMPL::reset_cache() { m_cache_.reset(); }

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
    // Check the inputs we were just given
    for(const auto& [k, v] : ps)
        if(!v.ready()) throw std::runtime_error("Inputs are not ready");

    // Merge with bound and see if we are ready
    if(!ready(ps)) throw std::runtime_error("Module is not ready to be run");

    lock();

    ps = merge_inputs_(ps);
    // Check cache
    auto hv = get_hash_(ps);

    if(is_memoizable() && m_cache_ && m_cache_->count(hv)) {
        m_timer_.record(time_now);
        return m_cache_->at(hv);
    }

    // not there so run
    auto rv = m_base_->run(std::move(ps), m_submods_);
    if(!m_cache_ || !is_memoizable()) {
        m_timer_.record(time_now);
        return rv;
    }

    // cache result
    m_cache_->emplace(hv, std::move(rv));
    m_timer_.record(time_now);
    return m_cache_->at(hv);
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

inline type::input_map ModulePIMPL::merge_inputs_(
  type::input_map in_inputs) const {
    for(const auto& [k, v] : m_inputs_)
        if(!in_inputs.count(k)) in_inputs[k] = v;
    return in_inputs;
}

inline void ModulePIMPL::lock() {
    for(auto& [k, v] : m_submods_) v.lock();
    m_locked_ = true;
}

/// Code factorization for computing the hash of a module
inline std::string ModulePIMPL::get_hash_(const type::input_map& in_inputs) {
    type::hasher h(bphash::HashType::Hash128);
    memoize(h, in_inputs);
    return bphash::hash_to_string(h.finalize());
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

template<class Archive>
inline void ModulePIMPL::save(Archive& ar) const {
    // ar& cereal::make_nvp("ModulePIMPL has_module", has_module());
    // ar& cereal::make_nvp("ModulePIMPL has_module", has_description());
    ar& cereal::make_nvp("ModulePIMPL cache", m_cache_);
    ar& cereal::make_nvp("ModulePIMPL results", results());
}

template<class Archive>
inline void ModulePIMPL::load(Archive& ar) {
    ar& cereal::make_nvp("ModulePIMPL cache", m_cache_);
}

template void ModulePIMPL::save<cereal::JSONOutputArchive>(
  cereal::JSONOutputArchive&) const;
template void ModulePIMPL::load<cereal::JSONInputArchive>(
  cereal::JSONInputArchive&);
template void ModulePIMPL::save<cereal::BinaryOutputArchive>(
  cereal::BinaryOutputArchive&) const;
template void ModulePIMPL::load<cereal::BinaryInputArchive>(
  cereal::BinaryInputArchive&);

} // namespace sde::detail_
