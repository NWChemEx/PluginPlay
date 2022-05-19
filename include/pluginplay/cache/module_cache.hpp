#pragma once
#include <memory>
#include <pluginplay/cache/module_manager_cache.hpp>
#include <pluginplay/fields/fields.hpp>
#include <pluginplay/types.hpp>

namespace pluginplay::cache {
namespace detail_ {
class ModuleCachePIMPL;
}

class ModuleCache {
public:
    /// Type of the object used to uniquely identify a module
    using module_uuid_type = typename ModuleManagerCache::module_cache_key;

    using key_type = type::input_map;

    using const_key_reference = const key_type&;

    using mapped_type = type::result_map;

    using pimpl_type = detail_::ModuleCachePIMPL;

    using pimpl_pointer = std::unique_ptr<pimpl_type>;

    ModuleCache() noexcept;

    explicit ModuleCache(pimpl_pointer pimpl) noexcept;

    ModuleCache(ModuleCache&& other) noexcept;

    ~ModuleCache() noexcept;

    bool count(const_key_reference key) const;

    void cache(key_type key, mapped_type value);

    mapped_type uncache(const_key_reference key);

    void clear();

private:
    /// Type of a modifiable PIMPL
    using pimpl_reference = pimpl_type&;

    /// Type of a reference to a read-only PIMPL
    using const_pimpl_reference = const pimpl_type&;

    /// Raises std::runtime_error if m_pimpl_ is null
    void assert_pimpl_() const;

    /// Asserts we have a PIMPL then returns it
    pimpl_reference pimpl_();

    /// Asserts we have a PIMPL then returns it (read-only)
    const_pimpl_reference pimpl_() const;

    /// The object holding the state of the object
    pimpl_pointer m_pimpl_;
};

} // namespace pluginplay::cache
