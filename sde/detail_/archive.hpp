
#include <cereal/archives/portable_binary.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/archives/xml.hpp>
#include <madness/world/binary_fstream_archive.h>
#include <madness/world/buffer_archive.h>
#include <madness/world/text_fstream_archive.h>
#include <madness/world/cereal_archive.h>

namespace sde{
    using PortableBinaryOutputArchive = cereal::PortableBinaryOutputArchive;
    using PortableBinaryInputArchive  = cereal::PortableBinaryInputArchive;
    using BinaryOutputArchive = cereal::BinaryOutputArchive;
    using BinaryInputArchive  = cereal::BinaryInputArchive;
    using JSONOutputArchive = cereal::JSONOutputArchive;
    using JSONInputArchive  = cereal::JSONInputArchive;
    using XMLOutputArchive = cereal::XMLOutputArchive;
    using XMLInputArchive  = cereal::XMLInputArchive;
    using BinaryFstreamOutputArchive = madness::archive::BinaryFstreamOutputArchive;
    using BinaryFstreamInputArchive = madness::archive::BinaryFstreamInputArchive;
    using BufferOutputArchive = madness::archive::BufferOutputArchive;
    using BufferInputArchive = madness::archive::BufferInputArchive;
    using TextFstreamOutputArchive = madness::archive::TextFstreamOutputArchive;   
    using TextFstreamInputArchive = madness::archive::TextFstreamInputArchive;
    using CerealBinaryInputArchive = madness::archive::CerealInputArchive<BinaryInputArchive>;
    using CerealBinaryOutputArchive = madness::archive::CerealOutputArchive<BinaryOutputArchive>;
    using CerealPortableBinaryInputArchive = madness::archive::CerealInputArchive<PortableBinaryInputArchive>;
    using CerealPortableBinaryOutputArchive = madness::archive::CerealOutputArchive<PortableBinaryOutputArchive>;
    using CerealJSONInputArchive = madness::archive::CerealInputArchive<JSONInputArchive>;
    using CerealJSONOutputArchive = madness::archive::CerealOutputArchive<JSONOutputArchive>;
    using CerealXMLInputArchive = madness::archive::CerealInputArchive<XMLInputArchive>;
    using CerealXMLOutputArchive = madness::archive::CerealOutputArchive<XMLOutputArchive>;
}
