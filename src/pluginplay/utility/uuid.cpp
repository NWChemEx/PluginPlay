#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <pluginplay/utility/uuid.hpp>

namespace pluginplay::utility {

uuid_type generate_uuid() {
    return boost::uuids::to_string(boost::uuids::random_generator()());
}

} // namespace pluginplay::utility
