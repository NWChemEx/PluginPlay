#pragma once
#include <Utilities/Containers/CaseInsensitiveMap.hpp>

namespace SDE {
namespace detail_ {

/** @brief Code factorization for the ComputedProperties and Parameters classes.
 *
 *  Generally speaking both the ComputedProperties class and Parameters classes
 *  can be regarded as `std::map<std::string, T>` instances, where the template
 *  type parameter, `T`, is a wrapper class with type-erasure properties. The
 *  result is that we can get a slightly more user-friendly interface by
 *  modifying the `std::map` API so that the undoing of the type-erasure can be
 *  done directly from the map's API. That's what this class does.
 *
 *  Since this class is really only envisioned as being used for the input and
 *  output to a module (that's where the name comes from), it is conceivable
 *  that we could define this class in a source file if we wanted to by using
 *  explicit template specialization. PIMPL-ing this class is also possible,
 *  albeit annoying because one would have to pimpl the iterators also.
 *
 *  @note This map is case-insensitive.
 *
 *  @tparam T The type of the wrapper class. Must be movable and define a
 *          function `const T& value<T>()` for retrieving the type-erased value.
 */
template <typename T>
class IOMap {
private:
    using map_type = Utilities::CaseInsensitiveMap<T>;
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

    /** @brief Releases the memory associated with the keys and values
     *
     *  @throw None No throw guarantee.
     */
    virtual ~IOMap() = default;

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
    auto emplace(key_type key, T value) {
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
protected:
    //@{
    /** @name Ctor and Assignment Operators
     *
     *  All ctors and assignment operators for this class are protected to avoid
     *  the user slicing the derived class.
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
    IOMap() = default;
    IOMap(const IOMap& rhs) = default;
    IOMap& operator=(const IOMap& rhs) = default;
    IOMap(IOMap&& rhs) = default;
    IOMap& operator=(IOMap&& rhs) = default;
    //@}

private:
    ///The object that actually implements this class
    Utilities::CaseInsensitiveMap<T> values_;
}; // class IOMap

} // namespace detail_
} // namespace SDE
