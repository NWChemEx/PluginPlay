#include "database_factory.hpp"
#include "key_injector.hpp"
#include "key_proxy_mapper.hpp"
#include "make_any.hpp"
#include "native.hpp"
#include "rocksdb/rocksdb.hpp"
#include "serialized.hpp"
#include "transposer.hpp"
#include "type_eraser.hpp"
#include "value_proxy_mapper.hpp"

namespace pluginplay::cache::database {

// Names are chosen to reflect design diagram
using input_map     = typename DatabaseFactory::input_map_type;
using module_input  = typename DatabaseFactory::module_input_type;
using result_map    = typename DatabaseFactory::result_map_type;
using module_result = typename DatabaseFactory::module_result_type;
using proxy_map     = typename DatabaseFactory::proxy_map_type;
using any_field     = typename DatabaseFactory::any_type;
using uuid          = typename DatabaseFactory::uuid_type;
using binary_type   = typename DatabaseFactory::binary_type;

DatabaseFactory::DatabaseFactory() { set_type_eraser_backend(); }

DatabaseFactory::DatabaseFactory(const std::string& cache_path,
                                 const std::string& uuid_path) {
    set_serialized_pm_to_pm(cache_path);
    set_type_eraser_backend(uuid_path);
}

typename DatabaseFactory::module_db_pointer DatabaseFactory::default_module_db(
  uuid_type module_uuid) const {
    using input_2_any = TypeEraser<module_input, uuid>;
    auto pi2any       = std::make_unique<input_2_any>(m_any2uuid_);

    using input_2_uuid = UUIDMapper<module_input>;
    auto pi2uuid       = std::make_unique<input_2_uuid>(std::move(pi2any));

    using input_2_pm = ProxyMapMaker<input_map>;
    auto pi2pm       = std::make_unique<input_2_pm>(std::move(pi2uuid));

    using key_proxy_mapper = KeyProxyMapper<input_map, result_map>;
    return std::make_unique<key_proxy_mapper>(std::move(pi2pm),
                                              pm2result_db(module_uuid));
}

typename DatabaseFactory::pm_2_result_map_pointer DatabaseFactory::pm2result_db(
  uuid_type module_uuid) const {
    // Short-term storage type
    using pm_2_result = Native<proxy_map, result_map>;

    if(m_serial_pm_) { // This pointer means we have long-term storage
        const std::string key = "__CACHE__ MODULE NAME __CACHE__";

        using injector_type = KeyInjector<proxy_map, proxy_map>;
        auto pinjector      = std::make_unique<injector_type>(
          key, std::move(module_uuid), m_serial_pm_);

        using result_2_any = TypeEraser<module_result, uuid>;
        auto pr2any        = std::make_unique<result_2_any>(m_any2uuid_);

        using result_2_uuid = UUIDMapper<module_result>;
        auto pr2uuid = std::make_unique<result_2_uuid>(std::move(pr2any));

        using result_2_pm = ProxyMapMaker<result_map>;
        auto pr2pm        = std::make_unique<result_2_pm>(std::move(pr2uuid));

        using value_proxy_mapper = ValueProxyMapper<proxy_map, result_map>;
        auto ppm2r = std::make_unique<value_proxy_mapper>(std::move(pr2pm),
                                                          std::move(pinjector));

        return std::make_unique<pm_2_result>(std::move(ppm2r));
    }
    // There's no long-term storage, so we don't actually need the module's uuid
    return std::make_unique<pm_2_result>();
}

void DatabaseFactory::set_serialized_pm_to_pm(const std::string& path) {
    using rocks_db = RocksDB<binary_type, binary_type>;
    auto pRDB_io   = std::make_unique<rocks_db>(path);

    using serial_pm = Serialized<proxy_map, proxy_map>;
    m_serial_pm_    = std::make_shared<serial_pm>(std::move(pRDB_io));
}

void DatabaseFactory::set_type_eraser_backend() {
    using uuid_2_any = Native<uuid, any_field>;
    auto puuid2any   = std::make_unique<uuid_2_any>();

    using transposer = Transposer<any_field, uuid>;
    m_any2uuid_      = std::make_shared<transposer>(std::move(puuid2any));
}

void DatabaseFactory::set_type_eraser_backend(const std::string& path) {
    using rocks_db = RocksDB<binary_type, binary_type>;
    auto pRDB_uuid = std::make_unique<rocks_db>(path);

    using serial_uuid2any = Serialized<uuid, any_field>;
    auto pserial_uuid = std::make_unique<serial_uuid2any>(std::move(pRDB_uuid));

    using uuid_2_any = Native<uuid, any_field>;
    auto puuid2any   = std::make_unique<uuid_2_any>(std::move(pserial_uuid));

    using transposer = Transposer<any_field, uuid>;
    m_any2uuid_      = std::make_shared<transposer>(std::move(puuid2any));
}

} // namespace pluginplay::cache::database
