#include "rocksdb.hpp"
#include <rocksdb/db.h>
namespace pluginplay::database::detail_ {

class RocksDBPIMPL {
public:
    using db_type    = rocksdb::DB;
    using db_pointer = db_type*;

    using options_type = rocksdb::Options;

    using parent_type = RocksDB<std::string, std::string>;

    using key_type = typename parent_type::key_type;

    using mapped_type = typename parent_type::mapped_type;

    using mapped_reference = mapped_type&;

    using const_path_reference = typename parent_type::const_path_reference;

    RocksDBPIMPL(const_path_reference path);

    auto options() {
        options_type options;
        options.create_if_missing = true; // Make DB if it DNE
        return options;
    }

    auto allocate(const_path_reference path, options_type opts) {
        db_pointer db;
        auto status = rocksdb::DB::Open(std::move(opts), path, &db);
        assert(status.ok());
        return db;
    }

private:
    struct Deleter {
        void operator()(db_pointer db) const {
            db->Close();
            delete db;
        }
    };

    std::unique_ptr<db_type, Deleter> m_db_;

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

    std::unique_ptr<RocksDBPIMPL> m_pimpl_;

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

    std::map<key_type, std::vector<key_type>> m_split_values_;
};

inline RocksDBPIMPL::RocksDBPIMPL(const_path_reference path) :
  m_db_(allocate(path, options())) {}

inline bool RocksDBPIMPL::count_(const_key_reference key) const noexcept {
    assert_ptr_();
    auto opts = rocksdb::ReadOptions();

    // Rule out that it definitely doesn't exist
    if(!m_db_->KeyMayExist(opts, key, nullptr)) return false;

    ValueType buffer;
    auto status = m_db_->Get(opts, key, &buffer);
    return ValueType{} != buffer;
}

} // namespace pluginplay::database::detail_
