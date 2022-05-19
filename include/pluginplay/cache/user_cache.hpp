#pragma once
#include <pluginplay/cache/module_cache.hpp>

namespace pluginplay::cache {

class UserCache {
public:
    using sub_cache_type = ModuleCache;

    UserCache() = default;

    explicit UserCache(sub_cache_type cache) : m_cache_(std::move(cache)) {}

    template<typename T>
    bool count(T&& key) const;

    template<typename T, typename U>
    void cache(T&& key, U&& value);

    template<typename U, typename T>
    U uncache(T&& key);

    void reset_cache() { m_cache_.clear(); }

private:
    using input_map_type = typename sub_cache_type::key_type;

    using result_map_type = typename sub_cache_type::mapped_type;

    template<typename T>
    input_map_type wrap_inputs_(T&& key) const;

    template<typename T>
    result_map_type wrap_results_(T&& value) const;

    template<typename T>
    T unwrap_results_(result_map_type value) const;

    sub_cache_type m_cache_;
};

} // namespace pluginplay::cache

#include "user_cache.ipp"
