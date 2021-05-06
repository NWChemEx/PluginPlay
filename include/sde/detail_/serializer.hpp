#pragma once
#include "sde/serialization.hpp"
#include <sstream>
#include <variant>

namespace sde::detail_ {
using binary_output  = cereal::BinaryOutputArchive;
using json_output    = cereal::JSONOutputArchive;
using xml_output     = cereal::XMLOutputArchive;
using variant_output = std::variant<binary_output*, json_output*, xml_output*>;

using binary_input  = cereal::BinaryInputArchive;
using json_input    = cereal::JSONInputArchive;
using xml_input     = cereal::XMLInputArchive;
using variant_input = std::variant<binary_input*, json_input*, xml_input*>;

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
    ArchiveWrapper& operator<<(T&& obj2serial) {
        if(m_ar_.index() == 0) {
            if constexpr(is_serializable<T, binary_output>::value) {
                auto bar = std::get<binary_output*>(m_ar_);
                (*bar)(std::forward<T>(obj2serial));
            } else
                throw std::runtime_error("Nonserializable type!");
        } else if(m_ar_.index() == 1) {
            if constexpr(is_serializable<T, json_output>::value) {
                auto bar = std::get<json_output*>(m_ar_);
                (*bar)(std::forward<T>(obj2serial));
            } else
                throw std::runtime_error("Nonserializable type!");
        } else if(m_ar_.index() == 2) {
            if constexpr(is_serializable<T, xml_output>::value) {
                auto bar = std::get<xml_output*>(m_ar_);
                (*bar)(std::forward<T>(obj2serial));
            } else
                throw std::runtime_error("Nonserializable type!");
        }
        return *this;
    }

    template<typename T>
    ArchiveWrapper& operator>>(T&& serial2obj) {
        if(m_ar_.index() == 0) {
            if constexpr(is_deserializable<T, binary_input>::value) {
                auto bar = std::get<binary_input*>(m_ar_);
                (*bar)(std::forward<T>(serial2obj));
            } else
                throw std::runtime_error("Nonserializable type!");
        } else if(m_ar_.index() == 1) {
            if constexpr(is_deserializable<T, json_input>::value) {
                auto bar = std::get<json_input*>(m_ar_);
                (*bar)(std::forward<T>(serial2obj));
            } else
                throw std::runtime_error("Nonserializable type!");
        } else if(m_ar_.index() == 2) {
            if constexpr(is_deserializable<T, xml_input>::value) {
                auto bar = std::get<xml_input*>(m_ar_);
                (*bar)(std::forward<T>(serial2obj));
            } else
                throw std::runtime_error("Nonserializable type!");
        }
        return *this;
    }

private:
    VariantArchive m_ar_;
};

using Serializer   = ArchiveWrapper<variant_output>;
using Deserializer = ArchiveWrapper<variant_input>;
} // namespace sde::detail_