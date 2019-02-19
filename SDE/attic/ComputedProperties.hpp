#pragma once
#include "Property.hpp"
#include <Utilities/Containers/CaseInsensitiveMap.hpp>

namespace SDE {

/** @brief Class responsible for holding the objects returned from a module.
 *
 *  This class is a glorified `std::map` except that the elements of the map are
 *  type-erased. As a result, this class extends the typical `std::map` API to
 *  facilitate retrieval of the elements. This is done *via* the `value` member,
 *  which functions similar to `std::map::at`, except that it takes a template
 *  type parameter specifying what the type of the returned quantity is.
 */
class ComputedProperties {
private:
    using map_type = Utilities::CaseInsensitiveMap<Property>;
public:
    /// The type of a natural number returned by this class
    using size_type = typename map_type::size_type;

    /// The type of the keys in the map
    using key_type = typename map_type::key_type;

    /// The type of the elements in the map
    using mapped_type = typename map_type::mapped_type;

    /// The type of a read/write iterator to the map
    using iterator = typename map_type::iterator;

    /// The type of a read-only iterator to the map
    using const_iterator = typename map_type::const_iterator;

    //@{
    /** @name Ctor and Assignment Operators
     *
     *
     *  1) Default ctor
     *  2 and 3) Copy ctor/assignment operators.
     *  4 and 5) Move ctor/assignment operators.
     *
     *  @param rhs The object to copy/move from. For 4 and 5, @p rhs will be in
     *         a valid but otherwise undefined state after the operation.
     *
     *  @throw None 1, 4, and 5 are no throw guarantee.
     *  @throw std::bad_alloc 2 and 3 will throw if there is insufficient
     *         memory to copy the state. Strong throw guarantee.
     */
    ComputedProperties() = default;
    ComputedProperties(const ComputedProperties& rhs) = default;
    ComputedProperties& operator=(const ComputedProperties& rhs) = default;
    ComputedProperties(ComputedProperties&& rhs) = default;
    ComputedProperties& operator=(ComputedProperties&& rhs) = default;
    //@}

    /** @brief Releases the memory associated with the keys and values
     *
     *  @throw None No throw guarantee.
     */
    ~ComputedProperties() = default;

    //@{
    /**
     * @name Element Retrieval
     *
     * @brief Retrieves the actual object associated with the key.
     *
     * Users typically want to access the type-erased value stored in the
     * element. This is the purpose of the `value` function. The other three
     * functions allow the user to retrieve the element of the map (*i.e.*, the
     * things holding the values). These elements hold metadata that may be of
     * interest in select circumstances.
     *
     * @note The `value` function is read-only because we are treating `T` as
     *       an opaque type and thus we do not know how to modify it.
     *
     * 1 and 2) Return an already existing value in read/write or read-only form
     *          respectively.
     *       3) Creates the new element and returns it in read/write form.
     *       4) Retrieves the type-erased value in read-only form.
     *
     * @param key The descriptor for the value of interest.
     * @return The instance of the wrapping class (the literal elements of this
     *         map).
     * @throw std::out_of_range 1, 2, and 3 throw if @p key is not already in
     *        the map.
     * @throw std::bad_alloc 3 throws if there is insufficient memory to
     *        allocate a new element. Strong throw guarantee.
     */
    mapped_type& at(const key_type& key) { return values_.at(key); }
    const mapped_type& at(const key_type& key) const {return values_.at(key); }
    mapped_type& operator[](const key_type& key) { return values_[key]; }
    template <typename U>
    const U& value(const key_type& key) const {
        constexpr bool unqualified = std::is_same_v<std::decay_t<U>, U>;
        static_assert(unqualified, "Template parameter should be unqualified");
        return values_.at(key).template value<U>();
    }
    //@}

    /** @brief Adds a new value to the map.
     *
     * @param key The key the value will be associated with. If @p key already
     *            exists this is a null op.
     * @param value The actual element to store inside this instance.
     * @return An iterator to inserted element and true if the value was
     *         actually inserted or an iterator to the existing element and
     *         false if the value already existed.
     */
    auto emplace(key_type key, mapped_type value) {
        return values_.emplace(std::move(key), std::move(value));
    }


    /** @brief Returns the number of times a key exists in the map.
     *
     * Since this map is not a multimap, a given key can appear 0 or 1 times in
     * the map. Thus the output of this function is suitably regarded as a bool.
     *
     * @param key The descriptor for the element we are looking for.
     * @return The number of times the key appears in the map.
     * @throw None No throw guarantee.
     */
    size_type count(const key_type& key) const noexcept {
        return values_.count(key);
    }

    /** @brief Returns the number of elements currently stored in the map.
     *
     * @return The number of elements stored in the map.
     * @throw None No throw guarantee.
     */
    size_type size() const noexcept { return values_.size(); }

    //@{
    /** @name Iterators
     *
     * 1) A read/write iterator pointing to the first element in the container.
     * 2 and 3) A read-only iterator to the first element in the container.
     * 4) A read/write iterator to just past the last element in the container.
     * 5 and 6) A read-only iterator to just past the last element in the
     *     container.
     *
     * @return 1-3 an iterator to first element of container, 4-6 an iterator
     *         just past the last element of the container.
     * @throw None All functions for creating iterators are no throw guarantee.
     */
    iterator begin() noexcept {return values_.begin(); }
    const_iterator begin() const noexcept {return values_.begin(); }
    const_iterator cbegin() const noexcept {return values_.cbegin(); }
    iterator end() noexcept { return values_.end(); }
    const_iterator end() const noexcept { return values_.end(); }
    const_iterator cend() const noexcept {return values_.cend(); }
    //@}

    //@{
    /** @name Comparison Operators
     *
     * Two ComputedProperties instances are defined as equivalent if they have
     * the same keys, in the same order, and each key maps to exactly the same
     * value. Equality of the values adheres to the definition affored by their
     * respective `operator==` members.
     *
     * 1) Equality operator
     * 2) Inequality operator
     *
     * @param rhs The other ComputedProperties instance to compare against
     * @return 1 returns true if the the current instance is equivalent to
     *         @p rhs and false otherwise. 2 returns false if the current
     *         instance is equivalent to @p rhs and true otherwise.
     *
     * @throw None No throw guarantee.
     */
    bool operator==(const ComputedProperties& rhs) const noexcept {
        return values_ == rhs.values_;
    }

    bool operator!=(const ComputedProperties& rhs) const noexcept {
        return values_ != rhs.values_;
    }
    //@}
private:
    ///The object that actually implements this class
    map_type values_;
}; // class ComputedProperties

} // namespace SDE
