#pragma once
#include "sde/serialization.hpp"
#include <sstream>
#include <variant>

namespace sde::detail_ {
using binary_output   = cereal::BinaryOutputArchive;
using portable_output = cereal::PortableBinaryOutputArchive;
using json_output     = cereal::JSONOutputArchive;
using xml_output      = cereal::XMLOutputArchive;
using variant_output =
  std::variant<binary_output*, portable_output*, json_output*, xml_output*>;

using binary_input   = cereal::BinaryInputArchive;
using portable_input = cereal::PortableBinaryInputArchive;
using json_input     = cereal::JSONInputArchive;
using xml_input      = cereal::XMLInputArchive;
using variant_input =
  std::variant<binary_input*, portable_input*, json_input*, xml_input*>;

/** @brief ArchiveWrapper class enables serialization and deserialization of
 * objects using various types of archives supported by Cereal.
 *
 *  This class wraps Cereal's API for different output
 * archive types. Required for serialization of SDEAny.
 */
template<typename VariantArchive>
class ArchiveWrapper {
public:
    template<typename Archive>
    ArchiveWrapper(Archive& ar) : m_ar_(&ar) {}

    template<typename T>
    ArchiveWrapper& operator&(T&& obj) {
        if(m_ar_.index() == 0) {
            auto bar = std::get<0>(m_ar_);
            (*bar)(std::forward<T>(obj));
        } else if(m_ar_.index() == 1) {
            auto bar = std::get<1>(m_ar_);
            (*bar)(std::forward<T>(obj));
        } else if(m_ar_.index() == 2) {
            auto bar = std::get<2>(m_ar_);
            (*bar)(std::forward<T>(obj));
        } else if(m_ar_.index() == 3) {
            auto bar = std::get<3>(m_ar_);
            (*bar)(std::forward<T>(obj));
        }
        return *this;
    }

private:
    VariantArchive m_ar_;
};

using Serializer   = ArchiveWrapper<variant_output>;
using Deserializer = ArchiveWrapper<variant_input>;
} // namespace sde::detail_