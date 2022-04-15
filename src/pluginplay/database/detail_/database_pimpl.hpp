#pragma once
#include "../value.hpp"

namespace pluginplay::database::detail_ {

/** @brief Implements a Database object.
 *
 *  To keep things simple for the user we only have one Database class. This
 *  class defines a very simple API that all database backends adhere to.
 *  Different implementations are created by specializing the Database class's
 *  PIMPL. The DatabasePIMPL class defines the API that all PIMPLs must have.
 *
 *  @tparam KeyType In PluginPlay databases are viewed as key/value stores,
 *                  @p KeyType is the type for the keys.
 *  @tparam ValueType In PluginPlay databases are viewed as key/value stores,
 *                    @p ValueType is the type for the values.
 */
template<typename KeyType, typename ValueType>
class DatabasePIMPL {
public:
    /// Type used to store the keys in the database, same as @p KeyType
    using key_type = KeyType;

    /// Read-only reference to a key. Alias for `const KeyType&`
    using const_key_reference = const key_type&;

    /// Type of the values being stored in the database, same as @p ValueType
    using mapped_type = ValueType;

    /** @brief mutable reference to a value
     *
     *  Because different backends have different memory semantics, insulating
     *  the user from these semantics requires a more complex solution than
     *  simply using `ValueType&`. To this end we have created the `Value`
     *  class. The Value class is capable of holding either a reference to an
     *  object or the value itself. Calling `.get()` on the value returns a
     *  mutable reference, regardless of whether the `Value` instance owns the
     *  mapped value or if the backend does. This type is an alias of
     *  `Value<ValueType>`.
     *
     *  N.B. `mapped_reference` is only used by the backend. The public API of
     *       DatabasePIMPL uses const_mapped_reference only. Avoiding problems
     *       related to mutating values owned by `Value` instances.
     */
    using mapped_reference = Value<mapped_type>;

    /** @brief immutable reference to a value
     *
     *  Because different backends have different memory semantics, insulating
     *  the user from these semantics requires a more complex solution than
     *  simply using `const ValueType&`. To this end we have created the
     *  `ConstValue` class. The ConstValue class is capable of holding either a
     *  const reference to an object or the value itself. Calling `.get()` on
     *  the ConstValue instance returns an immutable reference to the wrapped
     *  value, regardless of whether the `ConstValue` instance owns the
     *  mapped value or if the backend does. This type is an alias of
     *  `ConstValue<ValueType>`.
     *
     */
    using const_mapped_reference = ConstValue<mapped_type>;

    /// No-op, no-throw default ctor
    DatabasePIMPL() noexcept = default;

    /// No-op, no-throw polymorphic default dtor
    virtual ~DatabasePIMPL() noexcept = default;

    /** @brief Public API for determining if a database contains a key.
     *
     *  Databases are viewed as key/value stores. This method is used to
     *  determine the number of times a given key appears in the database. We
     *  require that the database keys be unique, which in turn means that the
     *  result of calling this function is either 0 or 1, i.e., false or true.
     *
     *  N.B. This function is actually implemented by count_
     *
     *  @return The number of times @p key appears in the database. Since keys
     *          can only appear once, the return is true if @p key is in the
     *          database and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool count(const_key_reference key) const noexcept { return count_(key); }

    void insert(KeyType key, ValueType value);

    void free(const_key_reference key) { free_(key); }

    const_mapped_reference at(const_key_reference key) const;

    const_mapped_reference operator[](const_key_reference key) const;

    void backup() { backup_(); }

    void dump() { dump_(); }

protected:
    /** @brief Hook for derived class to implement count.
     *
     *  The derived class is responsible for overriding this method with a
     *  definition consisten with the description of DatabasePIMPL::count.
     *
     *  @param[in] key The key the user is looking for.
     *
     *  @return True if the key is found and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    virtual bool count_(const_key_reference key) const noexcept = 0;

    virtual void insert_(KeyType key, ValueType value) = 0;

    virtual void free_(const_key_reference key) = 0;

    virtual const_mapped_reference at_(const_key_reference key) const = 0;

    virtual void backup_() = 0;

    virtual void dump_() = 0;
};

#define TPARAMS template<typename KeyType, typename ValueType>
#define DB_PIMPL DatabasePIMPL<KeyType, ValueType>

TPARAMS
void DB_PIMPL::insert(KeyType key, ValueType value) {
    insert_(std::move(key), std::move(value));
}

TPARAMS
typename DB_PIMPL::const_mapped_reference DB_PIMPL::at(
  const_key_reference key) const {
    if(count(key)) return at_(key);
    throw std::out_of_range("Key was not found in the database");
}

TPARAMS
typename DB_PIMPL::const_mapped_reference DB_PIMPL::operator[](
  const_key_reference key) const {
    return at_(key);
}

#undef TPARAMS
#undef DB_PIMPL

} // namespace pluginplay::database::detail_
