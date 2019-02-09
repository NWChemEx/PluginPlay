#pragma once
#include "SDE/Types.hpp"
#include <array>
#include <functional>

namespace SDE {
namespace detail_ {

/** @brief Records the property type's API.
 *
 * Ultimately the PropertyType class needs to know the details of the API. Our
 * goal is to give that class the details it needs in a way that is easy on the
 * user and hides much of the metatemplate programming required to pull it off.
 * This class is the result of those efforts. Admittedly perusing the source
 * code for this class makes it seem complicated; however, its use is much
 * simpler than it looks. To that end, it is our opinion that learning how to
 * use this class is best done by example and we recommend the reader consult
 * the test `examples/TestPropertyType.hpp` for such an example.
 *
 * Anyways, this class works by requiring the developer to declare the results
 * or inputs (for generality we'll call these "fields") in the order they are
 * returned or accepted, respectively. For each field, we record the type in
 * this class's signature and the keyword used for the field. This makes this
 * class behave like a map, given a keyword we want to be able to retrieve the
 * details of a particular field. Where we differ from a map is that the storage
 * has to be in the order we added the fields, and not sorted for easy look-up.
 * Internally we accomplish this by using an std::array as the holder class
 * instead of a red-black tree like most std::map implementations.
 *
 * @tparam ElementType This will be either ModuleInput or ModuleResult depending
 *         on whether we are building the input or result API specification
 *         respectively.
 * @tparam ErasedTypes These are the types of the inputs/results in the order
 *         they are accepted/returned.
 */
template<typename ElementType, typename... ErasedTypes>
class PropertyTypeBuilder {
private:
    /// Type resulting from adding a new field of type @p T, onto the signature
    template<typename T>
    using new_field = PropertyTypeBuilder<ElementType, ErasedTypes..., T>;

    /// Type of this class
    using my_type = PropertyTypeBuilder<ElementType, ErasedTypes...>;

public:
    /// The type of the field, name conforms to std::map::mapped_type
    using mapped_type = ElementType;

    /// How we're storing the key-value pair, name from std::map
    using value_type = std::pair<type::key, mapped_type>;

    /// The current number of fields in the API
    static constexpr auto nfields = sizeof...(ErasedTypes);

    /// A tuple where element i is the type of the i-th field
    using tuple_of_fields = std::tuple<ErasedTypes...>;

    /// The type of an array holding @p N fields
    template<type::size N>
    using array_type = std::array<value_type, N>;

    /// The type of a read/write iterator over this class
    using iterator = typename array_type<nfields>::iterator;

    /// The type of a read-only iterator
    using const_iterator = typename array_type<nfields>::const_iterator;

    ///@{
    /** @name Ctor and assignment operators
     *
     * The functions in this section can be used to create a new instance with
     * a specified state or set the current instance to the state of another
     * instance. Since we need to keep track of the type of each field it is
     * only possible to modify the values held in the object and not to add
     * more. Adding another field requires creating a new instance, which is
     * done with the `add_field` member.
     *
     * @param rhs The instance to copy/move the state from. For move operations
     *        @p rhs is in a valid, but otherwise undefined state after the
     *        operation.
     * @param values For 6, this array contains the values of each field.
     * @return assignment operators return the current instance with the state
     *         set to/taken from the other instance.
     * @throw std::bad_alloc copy operations throw if there is insufficient
     *        memory to copy a field instance's state. Strong throw guarantee.
     * @throw none 1, and 4-6 are all no throw guarantee.
     */
    PropertyTypeBuilder()                               = default;
    PropertyTypeBuilder(const PropertyTypeBuilder& rhs) = default;
    my_type& operator=(const my_type& rhs)         = default;
    PropertyTypeBuilder(PropertyTypeBuilder&& rhs) = default;
    my_type& operator=(my_type&& rhs) = default;
    PropertyTypeBuilder(array_type<nfields> values) noexcept :
      values_(std::move(values)) {}
    ///@}

    /** @brief Standard dtor
     *
     * After calling this function all references are invalidated.
     *
     * @throw none No throw guarantee.
     */
    ~PropertyTypeBuilder() = default;

    /** @brief Adds another field to the API.
     *
     * This function adds another field to the property type's API. In order to
     * store the type of the new field the type of this class must change, which
     * can only be done by making a new instance (with a new type). This
     * function will make that new instance, move this instance's state to the
     * new instance, and then return the new instance. After calling this
     * function the current instance will be in a valid, but otherwise undefined
     * state. References to the state of each field are still valid, but
     * references to a field are not.
     *
     * @tparam T The type of the field you are adding.
     * @param key The keyword to associate with this field.
     * @return A new PropertyTypeBuilder instance which in
     * @throw std::bad_alloc if there is not enough memory to create the new
     *        field. Strong throw guarantee.
     * @throw std::invalid_argument if the provided key is already in use by
     *        another field. Strong throw guarantee.
     *
     */
    template<typename T>
    auto add_field(type::key key) {
        if(count(key))
            throw std::invalid_argument("Key: " + key + " already exists.");
        ElementType elem;
        elem.template set_type<T>();
        auto temp_values = add_field_(std::move(key), std::move(elem));
        return new_field<T>(std::move(temp_values));
    }

    /** @brief Returns the number of fields.
     *
     * This is really a convenience function/prettier API since the number of
     * fields can be read off the type.
     *
     * @return The number of fields
     * @throw none no throw guarantee.
     */
    static constexpr type::size size() noexcept { return nfields; }

    /** @brief Determines the number of fields with a particular key
     *
     * We require that each field have a unique keyword associated with it,
     * hence this function can only return 0 or 1.
     *
     * @param key The key we are looking for
     * @return 1 if the instance contains a field with a keyword @p key and 0
     *         otherwise.
     */
    type::size count(const type::key& key) const noexcept {
        for(type::size i = 0; i < size(); ++i)
            if(values_[i].first == key) return 1;
        return 0;
    }

    ///@{
    /** @name Field accessors
     *
     * Functions in this section can be used to retrieve a field by providing
     * the key for that field. Depending on the member called the resulting
     * field is in a read/write state (1 and 3) or is in a read-only state (2
     * and 4). Note that all of these functions behave like `std::map::at` and
     * require @p key to be valid. Adding fields can be accomplished with the
     * add_field member.
     *
     * @param key The keyword associated with the field you want.
     * @return The requested field in a read/write (1 and 3) or read-only state
     *         (2 and 4).
     * @throw std::out_of_range if @p key is not a keyword associated with an
     *        already existing field. Strong throw guarantee.
     */
    auto& operator[](const type::key& key) { return at(key); }
    const auto& operator[](const type::key& key) const { return at(key); }
    auto& at(const type::key& key) {
        const auto& temp = const_cast<const my_type&>(*this).at(key);
        return const_cast<mapped_type&>(temp.second);
    }
    const auto& at(const type::key& key) const {
        return values_.at(position_(key));
    }
    ///@}

    ///@{
    /** @name Iterators
     *
     * The functions in this section can be used to retrieve iterators that run
     * over the instance. Iterators are bidirectional. Variants 1 and 4 return
     * iterators that can be used to read or modify the fields, wheras all other
     * variants return read-only iterators. The first three variants return an
     * iterator pointing at the first field in the API and the last three return
     * an iterator just past the last field in the API.
     *
     * @return the requested iterator.
     * @throw none No throw guarantee.
     */
    iterator begin() noexcept { return values_.begin(); }
    const_iterator begin() const noexcept { return values_.begin(); }
    const_iterator cbegin() const noexcept { return begin(); }
    iterator end() noexcept { return values_.end(); }
    const_iterator end() const noexcept { return values_.end(); }
    const_iterator cend() const noexcept { return end(); }
    ///@}

private:
    /** @brief Function which moves the fields over to a new std::array
     *
     * This function is basically the guts of a `std::map::emplace`-like
     * function for our current class.
     *
     * @param key The key associated with the new field.
     * @param value The new field to tack onto the end of the array.
     * @return An one element-larger array containing the original fields as
     *         well as the new provided field.
     * @throw none No throw guarantee.
     */
    auto add_field_(type::key key, mapped_type value) noexcept {
        array_type<nfields + 1> temp;
        for(type::size i = 0; i < nfields; ++i) temp[i] = std::move(values_[i]);
        temp[nfields] = std::make_pair(std::move(key), std::move(value));
        return temp;
    }

    /** @brief Computes the index for a given key
     *
     * @param key The key we are looking for.
     * @return The position in the API the key is associated with. Returns are
     *         in the range [0, nfields).
     * @throw std::out_of_range if the provided key is not associated with a
     *        field. Strong throw guarantee.
     */
    type::size position_(const type::key& key) const {
        for(type::size i = 0; i < size(); ++i)
            if(values_[i].first == key) return i;
        throw std::out_of_range(std::string("Key ") + key + " not found.");
    }

    /// The fields that have been added so far
    array_type<nfields> values_;
};

} // namespace detail_
} // namespace SDE
