#pragma once
#include "sde/detail_/field_tuple_traits.hpp"
#include <functional>

namespace sde {
class ModuleInput;

/** @brief Records the property type's input or result API.
 *
 * This class is responsible for holding the details of a property type's input
 * or result fields. In particular this means that the class must retain:
 * - the order the fields were declared in,
 * - the key for each field,
 * - the type of each field's value, and
 * - an input field's default value (only inputs may have default values)
 *
 * The FieldTuple class works like a map from a field's name to either
 * a ModuleInput or ModuleResult instance (depending respectively on whether the
 * instance is storing the inputs to or the results from calling a module).
 *
 * @tparam ElementType This will be either ModuleInput or ModuleResult depending
 *         on whether we are building the input or result API specification
 *         respectively.
 * @tparam FieldTypes These are the types of the inputs/results in the order
 *         they are accepted/returned.
 */
template<typename ElementType, typename... FieldTypes>
class FieldTuple {
public:
    /// The type of the traits class
    using traits_type = detail_::FieldTupleTraits<ElementType, FieldTypes...>;

    /// The type of a key used to retrieve a field
    using key_type = typename traits_type::key_type;

    /// The type of the field
    using mapped_type = typename traits_type::mapped_type;

    /// How we're storing the key-value pair, name from std::map
    using value_type = typename traits_type::value_type;

    /// The type of a read/write iterator over this class
    using iterator = typename traits_type::iterator;

    /// The type of a read-only iterator
    using const_iterator = typename traits_type::const_iterator;

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
    FieldTuple() = default;

    FieldTuple(const FieldTuple& rhs) = default;

    FieldTuple& operator=(const FieldTuple& rhs) = default;

    FieldTuple(FieldTuple&& rhs) = default;

    FieldTuple& operator=(FieldTuple&& rhs) = default;

    FieldTuple(std::array<value_type, traits_type::nfields> values) noexcept;
    ///@}

    /** @brief Standard dtor
     *
     * After calling this function all references are invalidated.
     *
     * @throw none No throw guarantee.
     */
    ~FieldTuple() = default;

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
     * @return A new FieldTuple instance which in
     * @throw std::bad_alloc if there is not enough memory to create the new
     *        field. Strong throw guarantee.
     * @throw std::invalid_argument if the provided key is already in use by
     *        another field. Strong throw guarantee.
     *
     */
    template<typename T>
    auto add_field(type::key key);

    template<typename T, typename U>
    auto add_field(type::key key, U&& value);

    template<typename T>
    void set_default(T&& value);

    /** @brief Returns the number of fields.
     *
     * This is really a convenience function/prettier API since the number of
     * fields can be read off the type.
     *
     * @return The number of fields
     * @throw none no throw guarantee.
     */
    static constexpr type::size size() noexcept { return traits_type::nfields; }

    /** @brief Determines the number of fields with a particular key
     *
     * We require that each field have a unique keyword associated with it,
     * hence this function can only return 0 or 1.
     *
     * @param key The key we are looking for
     * @return 1 if the instance contains a field with a keyword @p key and 0
     *         otherwise.
     */
    type::size count(const type::key& key) const noexcept;

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

    auto& at(const type::key& key);

    const auto& at(const type::key& key) const;
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
    iterator begin() noexcept { return m_values_.begin(); }

    const_iterator begin() const noexcept { return m_values_.begin(); }

    const_iterator cbegin() const noexcept { return begin(); }

    iterator end() noexcept { return m_values_.end(); }

    const_iterator end() const noexcept { return m_values_.end(); }

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
    auto add_field_(type::key key, mapped_type value) noexcept;

    /** @brief Computes the index for a given key
     *
     * @param key The key we are looking for.
     * @return The position in the API the key is associated with. Returns are
     *         in the range [0, nfields).
     * @throw std::out_of_range if the provided key is not associated with a
     *        field. Strong throw guarantee.
     */
    type::size position_(const type::key& key) const;

    /// The fields that have been added so far
    typename traits_type::template field_array<traits_type::nfields> m_values_;
};

/*******************************************************************************
 *                                 Implementations                             *
 ******************************************************************************/

#define BUILDER_PARAMS template<typename ElementType, typename... FieldTypes>
#define BUILDER_TYPE FieldTuple<ElementType, FieldTypes...>

BUILDER_PARAMS
BUILDER_TYPE::FieldTuple(
  std::array<value_type, traits_type::nfields> values) noexcept :
  m_values_(std::move(values)) {}

BUILDER_PARAMS
template<typename T>
auto BUILDER_TYPE::add_field(type::key key) {
    if(count(key))
        throw std::invalid_argument("Key: " + key + " already exists.");
    ElementType elem;
    elem.template set_type<T>();
    auto temp_values  = add_field_(std::move(key), std::move(elem));
    using return_type = typename traits_type::template new_field_tuple<T>;
    return return_type(std::move(temp_values));
}

BUILDER_PARAMS
template<typename T, typename U>
auto BUILDER_TYPE::add_field(type::key key, U&& value) {
    auto rv = add_field<T>(std::move(key));
    rv.set_default(std::forward<U>(value));
    return rv;
}

BUILDER_PARAMS
template<typename T>
void BUILDER_TYPE::set_default(T&& value) {
    constexpr auto nfields = traits_type::nfields;
    static_assert(nfields > 0, "Must have a field to set a default");
    constexpr bool is_input = std::is_same_v<ElementType, ModuleInput>;
    static_assert(is_input, "Returns cannot have a default value");
    if constexpr(is_input) {
        constexpr auto nm1 = nfields - 1;
        std::get<nm1>(m_values_).second.set_default(std::forward<T>(value));
    }
}

BUILDER_PARAMS
sde::type::size BUILDER_TYPE::count(const type::key& key) const noexcept {
    for(type::size i = 0; i < size(); ++i)
        if(m_values_[i].first == key) return 1;
    return 0;
}

BUILDER_PARAMS
auto& BUILDER_TYPE::at(const type::key& key) {
    const auto& temp = const_cast<const BUILDER_TYPE&>(*this).at(key);
    return const_cast<mapped_type&>(temp.second);
}

BUILDER_PARAMS
const auto& BUILDER_TYPE::at(const type::key& key) const {
    return m_values_.at(position_(key));
}

BUILDER_PARAMS
auto BUILDER_TYPE::add_field_(type::key key, mapped_type value) noexcept {
    constexpr auto nfields = traits_type::nfields;
    typename traits_type::template field_array<nfields + 1> temp;
    for(type::size i = 0; i < nfields; ++i) temp[i] = std::move(m_values_[i]);
    temp[nfields] = std::make_pair(std::move(key), std::move(value));
    return temp;
}

BUILDER_PARAMS
type::size BUILDER_TYPE::position_(const type::key& key) const {
    for(type::size i = 0; i < size(); ++i)
        if(m_values_[i].first == key) return i;
    throw std::out_of_range(std::string("Key ") + key + " not found.");
}

#undef BUILDER_TYPE
#undef BUILDER_PARAMS

} // namespace sde
