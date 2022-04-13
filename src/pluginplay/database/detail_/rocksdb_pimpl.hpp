#include "rocksdb.hpp"
#include <rocksdb/db.h>
namespace pluginplay::database::detail_ {

class RocksDBPIMPL {
public:
    using parent_type = RocksDB<std::string, std::string>;

    using const_path_reference = typename parent_type::const_path_reference;

    using key_type = typename parent_type::key_type;

    using const_key_reference = typename parent_type::const_key_reference;

    using mapped_type = typename parent_type::mapped_type;

    using mapped_reference = mapped_type&;

    using const_mapped_reference = typename parent_type::const_mapped_reference;

    RocksDBPIMPL(const_path_reference path);

    bool count(const_key_reference key) const noexcept;

    void insert(key_type key, mapped_type value);

    void free(const_key_reference key);

    const_mapped_reference at(const_key_reference key) const;

private:
    /// Type RocksDB uses for databases
    using db_type = rocksdb::DB;

    /// Type of a raw pointer to a RocksDB database
    using raw_db_pointer = db_type*;

    /// Allows us to use RAII to ensure the RocksDB database is closed correctly
    struct Deleter {
        void operator()(raw_db_pointer db) const {
            db->Close();
            delete db;
        }
    };

    /// Type of the pointer holding a RocksDB database
    using db_pointer = std::unique_ptr<db_type, Deleter>;

    using options_type = rocksdb::Options;

    options_type options_();

    raw_db_pointer allocate_(const_path_reference path, options_type opts);

    void assert_ptr_() const;

    /** @brief Wraps the process of splitting a large value
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

    void large_value_insert_(key_type, mapped_type value);

    const_mapped_reference large_value_at_(const_key_reference key) const;

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

} // namespace pluginplay::database::detail_

#include "rocksdb_pimpl.ipp"
