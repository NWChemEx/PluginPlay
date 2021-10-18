#pragma once
#include "pluginplay/detail_/any.hpp"
#include <pluginplay/hasher.hpp>

namespace pluginplay {

enum class CacheTag { Permenant, Temporary };
/** @brief An object for storing data which may be needed after a module has run
 *
 */
class Cache {
public:
    /// Type used for a "key"
    using hash_type = std::string;

private:
    /** @brief Is type @p T the same as `hash_type`?
     *
     *  This is a compile-time constant indicating whether type @p T is the same
     *  type as `hash_type` (it's set to true if @p T is the same type and false
     *  otherwise). It is used primarily for TMP.
     *
     *  @tparam T The type we are comparing to `hash_type`
     */
    template<typename T>
    static constexpr bool is_hash_v = std::is_same_v<T, hash_type>;

    /** @brief Disables an overload if @p T is the same as `hash_type`.
     *
     *  This type exploits SFINAE to disable templated overloads of functions
     *  when the user passes the hash instead of an object that needs hashing.
     */
    template<typename T>
    using disable_if_hash_t = std::enable_if_t<!is_hash_v<T>>;

public:
    /** @brief Determines whether or not the cache stores a value associated
     *         with the provided key.
     *
     *  This function can be used to determine if this Cache instance is
     *  currently storing data associated with a specific key. The key can be
     *  any hashable object. This overload is used whenever the type of @p key,
     *  @p KeyType, is not the same as `hash_type`.
     *
     *  @tparam KeyType The type of the object being used as a key. Must be
     *                  hashable.
     *  @tparam <Anonymous> Template type parameter used to disable overload
     *                      when @p KeyType is the same as `hash_type`.
     *
     *  @param[in] key We are looking for data associated with this value.
     *
     *  @return True if the cache contains data associated with @p key and false
     *          otherwise.
     *
     * @throw ??? If hashing throws. Strong throw guarantee.
     */
    template<typename KeyType, typename = disable_if_hash_t<KeyType>>
    bool count(const KeyType& key) const {
        return count(pluginplay::hash_objects(key));
    }

    /** @brief Determines whether or not the cache stores a value associated
     *         with the provided hash.
     *
     *  This is a convenience function for looking for a key when we already
     *  know the hash of the key (thus allowing you to avoid the cost of
     *  hashing). This overload is used when @p key is of type `hash_type`.
     *
     *  @param[in] key The hash associated with the data we are looking for.
     *
     *  @return True if the cache contains data associated with @p key and false
     *          otherwise.
     *
     * @throw None No throw guarantee.
     */
    bool count(const hash_type& key) const noexcept;

    /** @brief Stores a value under the provided key.
     *
     *  This function is used to add data to the current Cache instance. The
     *  data is associated with the provided object. While the API restricts the
     *  key to being a single object, `std::tuple` can be used to instead
     *
     *  @tparam KeyType The type of the object being used as a key. Must be
     *                  hashable.
     *  @tparam ValueType The type of the data being stored. Must be
     *                    type-erasable via an pluginplayAny.
     *  @tparam <Anonymous> Template type parameter used to disable overload
     *                      when @p KeyType is the same as `hash_type`.
     *
     *  @param[in] key The object the data will be associated with.
     *  @param[in] value The data to store under the key.
     *  @param[in] tag The tag for the key.
     *
     *  @throw std::bad_alloc if there is a memory error allocating the storage.
     *                        Strong throw guarantee.
     *  @throw ??? If there is an error hashing the key.
     */
    template<typename KeyType, typename ValueType,
             typename = disable_if_hash_t<KeyType>>
    void cache(const KeyType& key, ValueType&& value,
               const CacheTag& tag = CacheTag::Permenant);

    /** @brief Stores a value under the provided hash value.
     *
     *  This function is a convenience function for cacheing data when the hash
     *  of the key is already known (thus allowing you to bypass the cost of
     *  hashing again). This overload participates only when @p key is of type
     *  `hash_type`.
     *
     *  @tparam ValueType The type of the data being stored. Must be
     *                    type-erasable via an pluginplayAny.
     *
     *  @param[in] key The hash the data will be associated with.
     *  @param[in] value The data to store under the key.
     *  @param[in] tag The tag for the key.
     *
     *  @throw std::bad_alloc if there is a memory error allocating the storage.
     *                        Strong throw guarantee.
     */
    template<typename ValueType>
    void cache(hash_type key, ValueType&& value,
               const CacheTag& tag = CacheTag::Permenant);

    /** @brief Function for accessing cached data by key.
     *
     *  This function allows a module to retrieve data that it has previously
     *  cached by providing the key it is cached under. For API simplicity the
     *  key must be a single object; however, the caller can use an `std::tuple`
     *  of objects to satisfy this requirement. This function only participates
     *  in overload resolution if the type of @p key, @p KeyType, is not
     *  `hash_type`.
     *
     *  @note Whether an object is retrieved by value or reference is determined
     *        by the caller, specifically by the caller's choice of
     *        @p ValueType.
     *
     *  @tparam ValueType The type of the value being retrieved. Must be
     *                    specified by the caller.
     *  @tparam KeyType The type of the object used as a key.
     *
     *  @param[in] key The value that the cached data is stored under.
     *
     *  @return The cached data as an object of type @p ValueType.
     *
     *  @throw std::out_of_range if the cache does not contain a value under the
     *                           provided key. Strong throw guarantee.
     */
    template<typename ValueType, typename KeyType,
             typename = disable_if_hash_t<KeyType>>
    ValueType uncache(const KeyType& key);

    /** @brief Function for accessing cached data by hash.
     *
     *  This function allows a module to retrieve data that it has previously
     *  cached by providing the hash it is cached under. This is a convenience
     *  API for when the caller knows the hash already (thereby avoiding the
     *  need to hash again).
     *
     *  @note Whether an object is retrieved by value or reference is determined
     *        by the caller, specifically by the caller's choice of
     *        @p ValueType.
     *
     *  @tparam ValueType The type of the value being retrieved. Must be
     *                    specified by the caller.
     *
     *  @param[in] key The hash that the cached data is stored under.
     *
     *  @return The cached data as an object of type @p ValueType.
     *
     *  @throw std::out_of_range if the cache does not contain a value under the
     *                           provided key. Strong throw guarantee.
     */
    template<typename ValueType>
    ValueType uncache(const hash_type& key);

    /** @brief Function for accessing read-only cached data by key.
     *
     *  This function allows a module to retrieve data that it has previously
     *  cached by providing the key it is cached under. For API simplicity the
     *  key must be a single object; however, the caller can use an `std::tuple`
     *  of objects to satisfy this requirement. This function only participates
     *  in overload resolution if the type of @p key, @p KeyType, is not
     *  `hash_type`.
     *
     *  @note Whether an object is retrieved by value or reference is determined
     *        by the caller, specifically by the caller's choice of
     *        @p ValueType.
     *
     *  @tparam ValueType The type of the value being retrieved. Must be
     *                    specified by the caller.
     *  @tparam KeyType The type of the object used as a key.
     *
     *  @param[in] key The value that the cached data is stored under.
     *
     *  @return The cached data as an object of type @p ValueType.
     *
     *  @throw std::out_of_range if the cache does not contain a value under the
     *                           provided key. Strong throw guarantee.
     */
    template<typename ValueType, typename KeyType,
             typename = disable_if_hash_t<KeyType>>
    ValueType uncache(const KeyType& key) const;

    /** @brief Function for accessing read-only cached data by hash.
     *
     *  This function allows a module to retrieve data that it has previously
     *  cached by providing the hash it is cached under. This is a convenience
     *  API for when the caller knows the hash already (thereby avoiding the
     *  need to hash again).
     *
     *  @note Whether an object is retrieved by value or reference is determined
     *        by the caller, specifically by the caller's choice of
     *        @p ValueType.
     *
     *  @tparam ValueType The type of the value being retrieved. Must be
     *                    specified by the caller.
     *
     *  @param[in] key The hash that the cached data is stored under.
     *
     *  @return The cached data as an object of type @p ValueType.
     *
     *  @throw std::out_of_range if the cache does not contain a value under the
     *                           provided key. Strong throw guarantee.
     */
    template<typename ValueType>
    ValueType uncache(const hash_type& key) const;

    /** @brief Function for accessing cached data by key, return default value
     *         if not cached.
     *
     *  This function allows a module to retrieve data that it has previously
     *  cached by providing the key it is cached under. For API simplicity the
     *  key must be a single object; however, the caller can use an `std::tuple`
     *  of objects to satisfy this requirement. This function only participates
     *  in overload resolution if the type of @p key, @p KeyType, is not
     *  `hash_type`. If the key is not present in the cache, the default values
     *  provided are returned.
     *
     *  @note Whether an object is retrieved by value or reference is determined
     *        by the caller, specifically by the caller's choice of
     *        @p ValueType.
     *
     *  @tparam ValueType The type of the value being retrieved. Must be
     *                    specified by the caller.
     *  @tparam KeyType The type of the object used as a key.
     *
     *  @param[in] key The value that the cached data is stored under.
     *
     *  @param[in] default_value The value returned if the key is not found.
     *
     *  @return The cached data of default value as an object of type
     *          @p ValueType.
     *
     *  @throw std::out_of_range if the cache does not contain a value under the
     *                           provided key. Strong throw guarantee.
     */
    template<typename ValueType, typename KeyType,
             typename = disable_if_hash_t<KeyType>>
    ValueType uncache(const KeyType& key, const ValueType& default_value);

    /** @brief Function for accessing cached data by hash, return default value
     *         if not cached.
     *
     *  This function allows a module to retrieve data that it has previously
     *  cached by providing the hash it is cached under. This is a convenience
     *  API for when the caller knows the hash already (thereby avoiding the
     *  need to hash again). If the key is not present in the cache, the
     *  default values provided are returned.
     *
     *  @note Whether an object is retrieved by value or reference is determined
     *        by the caller, specifically by the caller's choice of
     *        @p ValueType.
     *
     *  @tparam ValueType The type of the value being retrieved. Must be
     *                    specified by the caller.
     *
     *  @param[in] key The hash that the cached data is stored under.
     *
     *  @param[in] default_value The value returned if the key is not found.
     *
     *  @return The cached data as an object of type @p ValueType.
     *
     *  @throw std::out_of_range if the cache does not contain a value under the
     *                           provided key. Strong throw guarantee.
     */
    template<typename ValueType>
    ValueType uncache(const hash_type& key, const ValueType& default_value);

    /** @brief Function for accessing read-only cached data by key, return
     *         default value if not cached.
     *
     *  This function allows a module to retrieve data that it has previously
     *  cached by providing the key it is cached under. For API simplicity the
     *  key must be a single object; however, the caller can use an `std::tuple`
     *  of objects to satisfy this requirement. This function only participates
     *  in overload resolution if the type of @p key, @p KeyType, is not
     *  `hash_type`. If the key is not present in the cache, the default values
     *  provided are returned.
     *
     *  @note Whether an object is retrieved by value or reference is determined
     *        by the caller, specifically by the caller's choice of
     *        @p ValueType.
     *
     *  @tparam ValueType The type of the value being retrieved. Must be
     *                    specified by the caller.
     *  @tparam KeyType The type of the object used as a key.
     *
     *  @param[in] key The value that the cached data is stored under.
     *
     *  @param[in] default_value The value returned if the key is not found.
     *
     *  @return The cached data as an object of type @p ValueType.
     *
     *  @throw std::out_of_range if the cache does not contain a value under the
     *                           provided key. Strong throw guarantee.
     */
    template<typename ValueType, typename KeyType,
             typename = disable_if_hash_t<KeyType>>
    ValueType uncache(const KeyType& key, const ValueType& default_value) const;

    /** @brief Function for accessing read-only cached data by hash, return
     *         default value if not cached.
     *
     *  This function allows a module to retrieve data that it has previously
     *  cached by providing the hash it is cached under. This is a convenience
     *  API for when the caller knows the hash already (thereby avoiding the
     *  need to hash again). If the key is not present in the cache, the default
     *  values provided are returned.
     *
     *  @note Whether an object is retrieved by value or reference is determined
     *        by the caller, specifically by the caller's choice of
     *        @p ValueType.
     *
     *  @tparam ValueType The type of the value being retrieved. Must be
     *                    specified by the caller.
     *
     *  @param[in] key The hash that the cached data is stored under.
     *
     *  @param[in] default_value The value returned if the key is not found.
     *
     *  @return The cached data as an object of type @p ValueType.
     *
     *  @throw std::out_of_range if the cache does not contain a value under the
     *                           provided key. Strong throw guarantee.
     */
    template<typename ValueType>
    ValueType uncache(const hash_type& key,
                      const ValueType& default_value) const;

    /** @brief Set a given cache key as temporary
     *
     *  This function sets a given cache key as temporary
     *  @param[in] key The hash that the cached data is stored under.
     */
    void set_temporary(const hash_type& key);

    /** @brief Set a given cache key as permanent
     *
     *  This function sets a given cache key as permanent
     *  @param[in] key The hash that the cached data is stored under.
     */
    void set_permanent(const hash_type& key);

    /** @brief Deletes temporary values in the cache
     *
     *  This function deletes the cache values tagged as temporary
     */
    void prune_cache();

    /** @brief Reset the module cache to an empty map
     *
     *  This function allows the cache to be emptied of previously stored
     *  results
     */
    void reset_cache();

private:
    /// The object actually storing all of the cached data
    std::map<hash_type, pluginplay::detail_::Any> m_data_;
    /// The set storing the hash keys of temporary values in the cache
    std::set<hash_type> m_temp_;
}; // class Cache

// ------------------------------ Implementations ------------------------------

inline bool Cache::count(const hash_type& key) const noexcept {
    return m_data_.count(key);
}

template<typename KeyType, typename ValueType, typename>
void Cache::cache(const KeyType& key, ValueType&& value, const CacheTag& tag) {
    cache(pluginplay::hash_objects(key), std::forward<ValueType>(value), tag);
}

template<typename ValueType>
void Cache::cache(hash_type key, ValueType&& value, const CacheTag& tag) {
    using clean_type = std::decay_t<ValueType>;
    using pluginplay::detail_::make_Any;
    auto da_any  = make_Any<clean_type>(std::forward<ValueType>(value));
    m_data_[key] = std::move(da_any);
    if(tag == CacheTag::Temporary && !m_temp_.count(key)) m_temp_.insert(key);
}

template<typename ValueType, typename KeyType, typename>
ValueType Cache::uncache(const KeyType& key) {
    return uncache<ValueType>(pluginplay::hash_objects(key));
}

template<typename ValueType>
ValueType Cache::uncache(const hash_type& key) {
    if(count(key)) return m_data_.at(key).template cast<ValueType>();
    throw std::out_of_range("Internal cache does contain specified value");
}

template<typename ValueType, typename KeyType, typename>
ValueType Cache::uncache(const KeyType& key) const {
    return uncache<ValueType>(pluginplay::hash_objects(key));
}

template<typename ValueType>
ValueType Cache::uncache(const hash_type& key) const {
    if(count(key)) return m_data_.at(key).template cast<ValueType>();
    throw std::out_of_range("Internal cache does contain specified value");
}

template<typename ValueType, typename KeyType, typename>
ValueType Cache::uncache(const KeyType& key, const ValueType& default_value) {
    return uncache<ValueType>(pluginplay::hash_objects(key), default_value);
}

template<typename ValueType>
ValueType Cache::uncache(const hash_type& key, const ValueType& default_value) {
    if(count(key))
        return m_data_.at(key).template cast<ValueType>();
    else
        return default_value;
}

template<typename ValueType, typename KeyType, typename>
ValueType Cache::uncache(const KeyType& key,
                         const ValueType& default_value) const {
    return uncache<ValueType>(pluginplay::hash_objects(key), default_value);
}

template<typename ValueType>
ValueType Cache::uncache(const hash_type& key,
                         const ValueType& default_value) const {
    if(count(key))
        return m_data_.at(key).template cast<ValueType>();
    else
        return default_value;
}

inline void Cache::set_temporary(const hash_type& key) { m_temp_.insert(key); }

inline void Cache::set_permanent(const hash_type& key) { m_temp_.erase(key); }

inline void Cache::prune_cache() {
    for(auto const& key : m_temp_) m_data_.erase(key);
}

inline void Cache::reset_cache() { m_data_.clear(); }

} // namespace pluginplay
