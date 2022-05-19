#pragma once

namespace pluginplay::cache {
namespace detail_ {
class ModuleManagerCachePIMPL;
}

class ModuleCache;
class UserCache;

class ModuleManagerCache {
public:
    using module_cache_key = std::string;

    using path_type = std::string;

    using module_cache_type = ModuleCache;

    using module_cache_pointer = std::shared_ptr<module_cache_type>;

    using user_cache_type = UserCache;

    using user_cache_pointer = std::shared_ptr<user_cache_type>;

    ModuleManagerCache() noexcept;

    explicit ModuleManagerCache(path_type disk_location);

    ~ModuleManagerCache() noexcept;

    void change_save_location(path_type disk_location);

    module_cache_pointer get_or_make_module_cache(module_cache_key key);

    user_cache_pointer get_or_make_user_cache(module_cache_key key);

private:
    using pimpl_type = detail_::ModuleManagerCachePIMPL;

    using pimpl_reference = pimpl_type&;

    using const_pimpl_reference = const pimpl_type&;

    using pimpl_pointer = std::unique_ptr<pimpl_type>;

    module_cache_type make_module_cache_(module_cache_key key);

    void assert_pimpl_() const;

    pimpl_reference pimpl_();

    const_pimpl_reference pimpl_() const;

    pimpl_pointer m_pimpl_;
};

} // namespace pluginplay::cache
