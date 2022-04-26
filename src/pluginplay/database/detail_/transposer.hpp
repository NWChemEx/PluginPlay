#pragma once
#include "database_pimpl.hpp"
#include <set>

namespace pluginplay::database::detail_ {

/** @brief Satisfies the API of a DatabasePIMPL<KeyType, ValueType>, but really
 *         is a DatabasePIMPL<ValueType, KeyType>.
 *
 *  This class is used primarily as an optimization. Many DBs are optimized for
 *  the scenario where the size of a key is smaller than the size of the value;
 *  however, there are some mappings that are more naturaly expressed by having
 *  the key be the larger object. This class bridges the gap by allowing the
 *  user to have the API they want, while storing the key/values in an optimized
 *  manner.
 *
 *  N.B. Since the values of this database are really the keys, doing something
 *  like:
 *
 *  ```
 *  db.insert("hello", "world");
 *  db.insert("good-bye", "world");
 *  ```
 *  does not actually result in two database entries being added to the wrapped
 *  database, but rather results in the second call overwriting the first call.
 *  This class does nothing to prevent this from happening (in case that's the
 *  user's desired behvior).
 *
 *  @tparam KeyType The type of the keys. Will actually be the values in the
 *                  wrapped database.
 *  @tparam ValueType The types of the values. Will actually be the keys in the
 *                  wrapped database.
 */
template<typename KeyType, typename ValueType>
class Transposer : public DatabasePIMPL<KeyType, ValueType> {
private:
    /// Type of database being implemented by this class
    using base_type = DatabasePIMPL<KeyType, ValueType>;

public:
    /// Type of the database's keys, typedef of KeyType
    using typename base_type::key_type;

    /// Type of a read-only reference to a key, typedef of const KeyType&
    using typename base_type::const_key_reference;

    /// Type of the values in the database, typedef of ValueType
    using typename base_type::mapped_type;

    /// Type of an object containing a read-only reference to a value
    using typename base_type::const_mapped_reference;

    /// Type of the API the wrapped database satisfies
    using wrapped_db_type = DatabasePIMPL<mapped_type, key_type>;

    /// Type of a smart pointer to a database suitable for wrapping
    using wrapped_db_pointer = std::unique_ptr<wrapped_db_type>;

    /** @brief Creates a new Transposer instance by wrapping the provided
     *         database.
     *
     *  Transposer is a wrapper layer around an existing database that gives the
     *  existing database the appearance of being "transposed", i.e., the
     *  wrapped database will appear to the user as if the keys are the values,
     *  and the values are the keys.
     *
     *  N.B. At the moment there is no general way for the Transposer class to
     *  figure out what key/value pairs are in @p p when it gets passed in. So
     *  for example count will only properly find keys if the key/values are
     *  inserted through the Transposer::insert API>
     *
     *  @param[in] p The database we are wrapping. @p p is expected to have been
     *               allocated by the caller.
     *
     *  @throw std::runtime_error if @p p is a nullptr. Strong throw guarantee.
     */
    explicit Transposer(wrapped_db_pointer p);

protected:
    /// Loops over m_keys_ looking for a "key" whose value is @p key
    bool count_(const_key_reference key) const noexcept override;

    /// Adds @p key to the wrapped database under the "key" @p value
    void insert_(key_type key, mapped_type value) override;

    /// Loops over m_keys_ and if a value maps to @p key that value is freed
    void free_(const_key_reference key) override;

    /// Loops over m_keys_ returning the value that maps to @p key
    const_mapped_reference at_(const_key_reference key) const override;

    /// Calls backup on the wrapped databse
    void backup_() override { m_db_->backup(); }

    /// Calls dump on the wrapped database and clear on m_keys_
    void dump_() override;

private:
    /// The values the user has provided, they are keys in the wrapped database
    std::set<mapped_type> m_keys_;

    /// The wrapped database
    wrapped_db_pointer m_db_;
};

} // namespace pluginplay::database::detail_

#include "transposer.ipp"
