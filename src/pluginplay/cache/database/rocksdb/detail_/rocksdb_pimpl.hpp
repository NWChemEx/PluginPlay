/*
 * Copyright 2022 NWChemEx-Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "../rocksdb.hpp"
#include <rocksdb/db.h>
namespace pluginplay::cache::database::detail_ {

/** @brief Implements the RocksDB class when RocksDB support is enabled.
 *
 *  When the user has requested RocksDB support, rocksdb.cpp will select this
 *  header file. As defined in this header file (and the corresponding *.ipp
 *  file), the RocksDBPIMPL wraps a RocksDB database with a minimal API so that
 *  it can implement the RocksDB class. All of the actual RocksDB calls, types,
 *  etc. are in this class.
 *
 *  N.B. RocksDB only supports values that are about 4GB in size. Since the
 *  data is stored in the database as a string we can circumvent this problem
 *  by splitting the value into smaller chunks. The splitting and reassembling
 *  of values happens automatically and users of this database should act as if
 *  it doesn't happen.
 */
class RocksDBPIMPL {
public:
    /// Type of the class we're implementing
    using parent_type = RocksDB<std::string, std::string>;

    /// Type of a read-only reference to a file path
    using const_path_reference = typename parent_type::const_path_reference;

    /// Type used for database keys
    using key_type = typename parent_type::key_type;

    /// Type used for read-only references to database keys
    using const_key_reference = typename parent_type::const_key_reference;

    /// Type used to store the values in the database
    using mapped_type = typename parent_type::mapped_type;

    /// Type used for mutable references to values in the database
    using mapped_reference = mapped_type&;

    using const_mapped_reference = typename parent_type::const_mapped_reference;

    /** @brief Creates (or opens) a RocksDB database with the specified path
     *
     *  This Ctor is used to open an existing database (if @p path already
     *  exists and is a RocksDB database) or to create a new database (if
     *  @p path does not point to an existing file). Errors will arise if the
     *  RockSDB backend can not open the database (@p path already exists), or
     *  if the backend can not create the database. If this call is sucessful
     *  then the database is ready for business.
     *
     *  At the moment, construction uses the default RocksDB options for the
     *  database. No tuning has been performed.
     *
     *  @param[in] path For new databases this is where the database should
     *                  live, for existing databases this is where it lives.
     *
     *  @throws None No throw guarantee. At the moment, if an error occurs an
     *               assertion is tripped.
     */
    explicit RocksDBPIMPL(const_path_reference path);

    /** @brief Returns the number of times a key appears in the database.
     *
     *  Keys either appear or don't appear in a RocksDB database. Meaning, this
     *  method will return true if @p key appears in the database and false
     *  otherwise.
     *
     *  @param[in] key The key we are looking for.
     *
     *  @return True if @p key is in the database and false otherwise.
     *
     *  @throw None no throw guarantee.
     */
    bool count(const_key_reference key) const noexcept;

    /** @brief Adds a new entry to the database.
     *
     *  This method is used to add an entry to the wrapped database. In theory,
     *  there is no limit on how large the value can be (values which exceed
     *  the RocksDB size limits will be split up and recombined automatically
     *  under the hood).
     *
     *  @param[in] key The label for the new entry.
     *  @param[in] value The new entry.
     *
     *  @throw None No throw guarantee. If an error occurs while inserting the
     *              key/value pair an assert will be tripped.
     */
    void insert(key_type key, mapped_type value);

    /** @brief Used to delete a value from the database.
     *
     *  This method will release the value stored under the provided key.
     *  Exactly when the memory is released is up to the backend.
     *
     *  @param[in] key The label of the value which should be deleted. After
     *                 this call @p key (and the value associated with it) will
     *                 no longer live in the database.
     *
     *  @throw None No throw guarantee. If an error occurs while deleting the
     *              entry then an assert is tripped.
     */
    void free(const_key_reference key);

    /** @brief Retrieves the value associated with the specified key
     *
     *  This method will retrieve the value associated with a specified key. If
     *  internally the associated value was split up this method will
     *  automatically put it back together.
     *
     *  @param[in] key The label of the value to retrieve.
     *
     *  @return A ConstDBValue instance which holds a read-only reference to
     *          the requested value (if `count(key)`), otherwise the returned
     *          ConstDBValue object has no value.
     *
     *  @throw std::bad_alloc if there is a problem putting a large value back
     *                        together.
     */
    const_mapped_reference at(const_key_reference key) const;

private:
    /// Type RocksDB uses for databases
    using db_type = rocksdb::DB;

    /// Type RocksDB uses for database-wide options.
    using options_type = rocksdb::Options;

    /// Type of a raw pointer to a RocksDB database
    using raw_db_pointer = db_type*;

    /// Allows us to use RAII to ensure the RocksDB database is closed correctly
    struct Deleter {
        /// Called when the unique_ptr wants to delete the database
        void operator()(raw_db_pointer db) const {
            db->Close();
            delete db;
        }
    };

    /// Type of the pointer holding a RocksDB database
    using db_pointer = std::unique_ptr<db_type, Deleter>;

    /// Wraps the process of setting the default RocksDB options
    options_type options_();

    /// Wraps the process of allocating the RocksDB database
    raw_db_pointer allocate_(const_path_reference path, options_type opts);

    /// Asserts that the RocksDB database has been allocated
    void assert_ptr_() const;

    /** @brief Wraps the process of splitting a large value
     *
     *  This function will take a large value (as defined by m_max_value_size_)
     *  and split it up into chunks that are smaller than equal to
     *  m_max_value_size_. This method simply computes pointers to where the
     *  splits should occur and does not actually split value.
     *
     *  @param[in] value The large value we are splitting. It won't be modified
     *                   at this point; however, we take it by mutable reference
     *                   so that the resulting iterators are modifiable
     *
     *  @return A container of iterator pairs. Each pair is such that the first
     *          iterator points to the first element in that slice, and the
     *          second iterator points ot the first element not in that slice.
     */
    auto split_value_(mapped_reference value);

    /** @brief Wraps the process of inserting a large value
     *
     *  This method is responsible for actually inserting a large value. This
     *  entails: splitting the value, creating "subkeys" for the chunks, and
     *  then storing the subkey/chunk pairs in the database.
     *
     *  N.B. Since ownership of the value is being transferred to the database
     *       it's important to move the value into this function in order to
     *       avoid a very large copy. Of course, if you don't want to actually
     *       give up ownership for some reason then a copy is what you want.
     *
     *  @param[in] key The key for the large value. This is the key we pretend
     *                 that the large value is stored under (it's actually
     *                 stored under a series of subkeys).
     *  @param[in] value The large value we are putting into the database.
     */
    void large_value_insert_(key_type key, mapped_type value);

    /** @brief Wraps the process of taking a large value out of the database.
     *
     *  This method undoes large_value_insert_, meaning it retrieves the subkeys
     *  for @p key, retrieves the chunks associated with those subkeys, and then
     *  merges the chunks back together.
     *
     *  @param[in] key The key associated with the large value.
     *
     *  @return The large value associated with @p key. The value will be
     *          restored to the state it was in prior to calling
     *          large_value_insert_.
     */
    const_mapped_reference large_value_at_(const_key_reference key) const;

    void large_value_free_(const_key_reference key);

    void check_status_(rocksdb::Status s) const;

    /** @brief Maximum size a value can be.
     *
     *  RocksDB can't handle values which are larger than 4 GB; they recommend
     *  that values be less than 3 GB. To circumvent this we split large values
     *  into @p m_max_value_size_ chunks (values for RocksDB are always things
     *  that adhere to random-access containers so this is no problem). This
     *  attribute controls the chunk size and could concievably be set as part
     *  of the API for the class if desired. For now it's just hard-coded to
     *  3 GB.
     */
    const std::size_t m_max_value_size_ = 3E9;

    /// The pointer to the RocksDB database
    db_pointer m_db_;

    /// Maps keys for large values to the keys for the pieces
    std::map<key_type, std::vector<key_type>> m_split_values_;
};

} // namespace pluginplay::cache::database::detail_

#include "rocksdb_pimpl.ipp"
