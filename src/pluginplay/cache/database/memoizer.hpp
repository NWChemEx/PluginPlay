#pragma once
#include "../uuid_mapper.hpp"
#include "database_api.hpp"
#include <pluginplay/types.hpp>

namespace pluginplay::cache::database {

/** @brief Implements a database which can be used for memoizing calls to a
 *         module.
 *
 *  Implementation notes:
 *  - Compared to other DatabaseAPI implementations this one does quite a few
 *    coupled things, this can probably be avoided with a better design.
 *  - Going from one map to another map can be thought of as DatabaseAPI
 *    - Could use this to factor out conversion to proxy maps
 *  - Could have one DatabaseAPI which maps values and another which maps keys
 *
 */
class Memoizer : public DatabaseAPI<type::input_map, type::result_map> {
private:
    using base_type = DatabaseAPI<type::input_map, type::result_map>;

public:
    using typename base_type::key_type;

    using typename base_type::mapped_type;

    /// Type used to map an input/result to a proxy
    using proxy_mapper = UUIDMapper<type::any>;

    /// Type of a pointer to the proxy mapper
    using proxy_mapper_ptr = std::shared_ptr<proxy_mapper>;

    /// Type of the objects used as proxies for the input_map/result_map
    using proxy_type = typename proxy_mapper::key_type;

    /// Type of the object used as a proxy for input_map/result_map
    using proxy_map_type = utilities::CaseInsensitiveMap<proxy_type>;

    /// Type of the object used to hold the results in their native form
    using quick_db_type = DatabaseAPI<proxy_map_type, mapped_type>;

    /// Type of the pointer used to store the quick_db_type
    using quick_db_pointer = std::unique_ptr<quick_db_type>;

    /// Type of the database holding proxy-to-proxy relationships
    using sub_db_type = DatabaseAPI<proxy_map_type, proxy_map_type>;

    /// Type of a pointer to the proxy-to-proxy database
    using sub_db_pointer = std::unique_ptr<sub_db_type>;
};

} // namespace pluginplay::cache::database
