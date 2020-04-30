
#include <cereal/archives/portable_binary.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/archives/xml.hpp>
#include <madness/world/binary_fstream_archive.h>
#include <madness/world/buffer_archive.h>
#include <madness/world/text_fstream_archive.h>
#include <madness/world/cereal_archive.h>

namespace sde{
    using BinaryFstreamOutputArchive = madness::archive::BinaryFstreamOutputArchive;
    using BinaryFstreamInputArchive = madness::archive::BinaryFstreamInputArchive;
    using BufferOutputArchive = madness::archive::BufferOutputArchive;
    using BufferInputArchive = madness::archive::BufferInputArchive;
    using TextFstreamOutputArchive = madness::archive::TextFstreamOutputArchive;   
    using TextFstreamInputArchive = madness::archive::TextFstreamInputArchive;
    using CerealBinaryInputArchive = madness::archive::CerealInputArchive<cereal::BinaryInputArchive>;
    using CerealBinaryOutputArchive = madness::archive::CerealOutputArchive<cereal::BinaryOutputArchive>;
    using CerealPortableBinaryInputArchive = madness::archive::CerealInputArchive<cereal::PortableBinaryInputArchive>;
    using CerealPortableBinaryOutputArchive = madness::archive::CerealOutputArchive<cereal::PortableBinaryOutputArchive>;
    using CerealJSONInputArchive = madness::archive::CerealInputArchive<cereal::JSONInputArchive>;
    using CerealJSONOutputArchive = madness::archive::CerealOutputArchive<cereal::JSONOutputArchive>;
    using CerealXMLInputArchive = madness::archive::CerealInputArchive<cereal::XMLInputArchive>;
    using CerealXMLOutputArchive = madness::archive::CerealOutputArchive<cereal::XMLOutputArchive>;
}
