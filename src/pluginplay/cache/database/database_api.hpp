#pragma once
#include "db_value.hpp"
#include <stdexcept>
#include <vector>

namespace pluginplay::cache::database {

/** @brief Implements a Database object.
 *
 *  To keep things simple for the user we only have one Database class. This
 *  class defines a very simple API that all database backends adhere to.
 *  Different implementations are created by specializing the Database class's
 *  PIMPL. The DatabaseAPI class defines the API that all PIMPLs must have.
 *
 *  @tparam KeyType In PluginPlay databases are viewed as key/value stores,
 *                  @p KeyType is the type for the keys.
 *  @tparam ValueType In PluginPlay databases are viewed as key/value stores,
 *                    @p ValueType is the type for the values.
 */
template<typename KeyType, typename ValueType>
class DatabaseAPI {
public:
    /// Type used to store the keys in the database, same as @p KeyType
    using key_type = KeyType;

    /// Type of a container holding the database's keys
    using key_set_type = std::vector<key_type>;

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
     *       DatabaseAPI uses const_mapped_reference only. Avoiding problems
     *       related to mutating values owned by `Value` instances.
     */
    using mapped_reference = DBValue<mapped_type>;

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
    using const_mapped_reference = ConstDBValue<mapped_type>;

    /// No-op, no-throw default ctor
    DatabaseAPI() noexcept = default;

    /// No-op, no-throw polymorphic default dtor
    virtual ~DatabaseAPI() noexcept = default;

    key_set_type keys() const { return keys_(); }

    /** @brief Public API for determining if a database contains a key.
     *
     *  Databases are viewed as key/value stores. This method is used to
     *  determine the number of times a given key appears in the database. We
     *  require that the database keys be unique, which in turn means that the
     *  result of calling this function is either 0 or 1, i.e., false or true.
     *
     *  For hierarchical databases, and at this point in time, count is NOT
     *  recursive. For example say you have a database which stores things in
     *  memory, but when asked can back the memory database up to a database on
     *  disk. Say @p key is a key in the memory database, after calling dump
     *  calling `count(key)` on the memory database will return false, but
     *  calling `count(key)` on the disk subdatabase will return true.
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

    /** @brief Public API for adding a key/value pair to the database.
     *
     *  Databases are viewed as key/value stores. This method is used to add a
     *  key/value pair to the database. If @p key is not currently being used as
     *  a key, then a new entry will be created using @p key as the key and
     *  @p value as the mapped value. If @p key is currently in use, then
     *  @p value will overwrite the value currently associated with @p key.
     *
     *  For hierarchical databases insert will only add the new element to the
     *  database insert was called on, i.e., the elements will not be added to
     *  any subdatabases until backup and/or dump is called.
     *
     *  N.B. The implementation will own a copy of @p key and @p value. If you
     *       don't want to give up ownership of @p key and @p value let the
     *       API make a copy. Otherwise, make sure you `std::move` @p key and
     *       @p value into this function to avoid (potentially costly) copies.
     *
     *  N.B. This function is implemented by insert_
     *
     *  @param[in] key The label @p value will be stored under.
     *  @param[in] value The object being stored under @p key.
     *
     *  @throw ??? Throws if the backend throws. Same guarantee as the backend.
     */
    void insert(KeyType key, ValueType value);

    /** @brief Public API for releasing a key.
     *
     *  This method will delete the specified key and the value associated with
     *  it. If @p key is not in use this is a no-op. After calling this method
     *  @p key will not be a valid key for this database. In general calling
     *  this method will also invalidate all references obtained via `at` or
     *  `operator[]`, although it may not happen immediately.
     *
     *  For hierarchical databases free only releases the key/value pair from
     *  the database it was called on. If the key/value pair was backed up to a
     *  subdatabase it will still exist there.
     *
     *  N.B. This function is implemented by free_
     *
     *  @param[in] key The database will forget that this key (and its
     *                 associated value) ever existed.
     *
     *  @throw ??? Throws if the backend throws. Backends are encouraged to
     *             implement free in a manner which is no-throw guarantee, but
     *             this may not always be possible.
     */
    void free(const_key_reference key) { free_(key); }

    /** @brief Returns the value associated with a key, throwing if the key is
     *         not valid.
     *
     *  This function is the same as operator[], but will throw if there is no
     *  value stored under @p key.
     *
     *  N.B. This function's implementation relies on count_ and at_.
     *
     *  @param[in] key The label of the value we want.
     *
     *  @return An object whose `.get()` method returns an immutable reference
     *          to the value associated with @p key.
     *
     *  @throw std::out_of_range if @p key is not currently associated with a
     *                           value in the database. Strong throw guarantee.
     */
    const_mapped_reference at(const_key_reference key) const;

    /** @brief Returns the value associated with a key.
     *
     *  This method is used to retrieve the value associated with @p key. The
     *  backend is encouraged to do so without performing a bounds check (i.e.,
     *  ensuring @p key is assigned to a value) so that users can prefer this
     *  method over `at` when they know that @p key is valid. That said,
     *  depending on the backend's implementation, looking for the @p key can
     *  itself amount to a bounds check. In these cases the backend may still
     *  throw if the @p key is not found. Even if the backend doesn't throw,
     *  attempting to use the value returned by an invalid key is undefined
     *  behavior.
     *
     *  For hierarchical databases this function only retrieves the value from
     *  the database it was called on. In other words, this function will not
     *  check any subdatabases for @p key. This behavior is consistent with
     *  `count`, which will return false if @p key only exists in the
     *  subdatabase.
     *
     *  @param[in] key The label of the value we want.
     *
     *  @return An object whose `.get()` method returns an immutable reference
     *          to the value associated with @p key.
     *
     *  @throw ??? Throws if the backend throws. Same throw guarantee.
     */
    const_mapped_reference operator[](const_key_reference key) const;

    /** @brief Checkpoints the database.
     *
     *  This function is called when the user wants the database to be
     *  checkpointed, but the user is not done with the data in the database.
     *  Think of this as "save". Exactly what this means is up to the derived
     *  class, but in general after a successful call to this method, the
     *  database instance will still have the same keys and values, but the
     *  data in it will be backed-up as defined by the implementation.
     *
     *  For hierarchical databases this function will usually copy the data into
     *  the subdatabase.
     *
     *  N.B. This function's implementation relies on backup_
     *
     * @throw ??? Throws if the backend throws. The backend is encouraged to try
     *            to implement this method in a no throw manner if at all
     *            possible.
     */
    void backup() { backup_(); }

    /** @brief Checkpoints and erases the database's contents.
     *
     *  This method is intended to be more or less backup(), followed by freeing
     *  up the key/values in the database. Hence it's sort of "save and exit".
     *  This is it's own function (and not implemented in terms of backup_ and
     *  free_) so that the backend can do this in an optimized manner and
     *  guarantee that data isn't loss.
     *
     *  N.B. This function's implementation relies on dump_
     *
     *  @throw ??? Throws if the backend throws. The backend is encouraged to
     *             implement this in a no throw manner if at all possible.
     */
    void dump() { dump_(); }

protected:
    /** @brief Hook for derived class to implement keys.
     *
     *  The derived class is responsible for overriding this method with a
     *  definition consistent with the description of DatabaseAPI::keys.
     *
     *  @return A container with this database's keys
     *
     *  @throw std::bad_alloc if there is a problem creating the return. Strong
     *         throw guarantee.
     */
    virtual key_set_type keys_() const { throw std::runtime_error("NYI"); }

    /** @brief Hook for derived class to implement count.
     *
     *  The derived class is responsible for overriding this method with a
     *  definition consistent with the description of DatabaseAPI::count.
     *
     *  @param[in] key The key the user is looking for.
     *
     *  @return True if the key is found and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    virtual bool count_(const_key_reference key) const noexcept = 0;

    /** @brief Hook for derived class to implement insert
     *
     *  The derived class is responsible for overriding this method with a
     *  definition consistent with the description of DatabaseAPI::insert.
     *
     *  @param[in] key The key that @p value will be stored under.
     *  @param[in] value The object to store under @p key.
     *
     *  @throw ??? Backend may choose to throw for a variety of reasons, many of
     *             which can not be anticipated by this API.
     */
    virtual void insert_(KeyType key, ValueType value) = 0;

    /** @brief Hook for derived class to implement free
     *
     *  The derived class is responsible for overriding this method with a
     *  definition consistent with the description of DatabaseAPI::free.
     *
     *  @param[in] key The key for the key/value pair to delete.
     *
     *  @throw ??? The backend may choose to throw if appropriate.
     */
    virtual void free_(const_key_reference key) = 0;

    /** @brief Hook for derived class to implement operator[]
     *
     *  The derived class is responsible for overriding this method with a
     *  definition consistent with the description of DatabaseAPI::operator[].
     *
     *  @param[in] key The key whose associated value will be returned.
     *
     *  @throw ??? The backend may choose to throw if appropriate.
     */
    virtual const_mapped_reference at_(const_key_reference key) const = 0;

    /** @brief Hook for derived class to implement backup
     *
     *  The derived class is responsible for overriding this method with a
     *  definition consistent with the description of DatabaseAPI::backup.
     *
     *  @throw ??? The backend may choose to throw if appropriate.
     */
    virtual void backup_() = 0;

    /** @brief Hook for derived class to implement dump
     *
     *  The derived class is responsible for overriding this method with a
     *  definition consistent with the description of DatabaseAPI::dump.
     *
     *  @throw ??? The backend may choose to throw if appropriate.
     */
    virtual void dump_() = 0;
};

#define TPARAMS template<typename KeyType, typename ValueType>
#define DB_PIMPL DatabaseAPI<KeyType, ValueType>

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

} // namespace pluginplay::cache::database
