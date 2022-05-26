#include "database/database_api.hpp"
#include "module_cache_pimpl.hpp"

namespace pluginplay::cache {

ModuleCache::ModuleCache() noexcept = default;

ModuleCache::ModuleCache(pimpl_pointer pimpl) noexcept :
  m_pimpl_(std::move(pimpl)) {}

ModuleCache::ModuleCache(ModuleCache&& other) noexcept = default;

ModuleCache::~ModuleCache() noexcept = default;

bool ModuleCache::count(const_key_reference key) const {
    if(!m_pimpl_) return false;
    return m_pimpl_->m_db->count(key);
}

void ModuleCache::cache(key_type key, mapped_type value) {
    pimpl_().m_db->insert(std::move(key), std::move(value));
}

typename ModuleCache::mapped_type ModuleCache::uncache(
  const_key_reference key) {
    if(!count(key)) throw std::out_of_range("No cached results");
    return m_pimpl_->m_db->at(key).get();
}

void ModuleCache::clear() {
    if(!m_pimpl_) return;
    m_pimpl_->m_db->dump();
}

void ModuleCache::assert_pimpl_() const {
    if(m_pimpl_) return;
    throw std::runtime_error("ModuleCache does not have a PIMPL. Did you move "
                             "from it or initialize it with a null pointer?");
}

typename ModuleCache::pimpl_reference ModuleCache::pimpl_() {
    assert_pimpl_();
    return *m_pimpl_;
}

typename ModuleCache::const_pimpl_reference ModuleCache::pimpl_() const {
    assert_pimpl_();
    return *m_pimpl_;
}

} // namespace pluginplay::cache
