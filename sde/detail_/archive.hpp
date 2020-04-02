#include <cereal/archives/portable_binary.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/archives/xml.hpp>

namespace sde{
    using PortableBinaryOutputArchive = cereal::PortableBinaryOutputArchive;
    using PortableBinaryInputArchive  = cereal::PortableBinaryInputArchive;
    using BinaryOutputArchive = cereal::BinaryOutputArchive;
    using BinaryInputArchive  = cereal::BinaryInputArchive;
    using JSONOutputArchive = cereal::JSONOutputArchive;
    using JSONInputArchive  = cereal::JSONInputArchive;
    using XMLOutputArchive = cereal::XMLOutputArchive;
    using XMLInputArchive  = cereal::XMLInputArchive;
}
