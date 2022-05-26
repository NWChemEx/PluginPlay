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

    /** @brief Uncache with optional default value.
     *
     *  This method checks to see if there's a result cached under @p key. If so
     *  that result is returned. If not, then @p default_value is returned.
     *  This is largely intended as code factorization for the very common use
     *  case when you attempt to restart from a cached result, but need to use
     *  a default value if no cached result exists.
     *
     *  @tparam U The type of the result. Results are stored in a type-erased
     *            manner so you must know the result's type to uncache it.
     *  @tparam T The type of the key.
     *  @tparam V The type of the default argument. Must be implicitly
     *            convertible to type U.
     *
     *  @param[in] key The object used to store the result.
     *  @param[in] default_value The value to return if there is no result
     *                           stored under @p key.
     *
     *  @return The result stored under @p key (if there is one) otherwise
     *          @p default_value is returned.
     *
     */
    template<typename U, typename T, typename V>
    U uncache(T&& key, V&& default_value);

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
