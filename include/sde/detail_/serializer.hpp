#pragma once
#include "sde/detail_/serialization.hpp"
#include <sstream>
#include <variant>

namespace sde::detail_ {

/** @brief Serializer class enables serialization of objects into various types
 * of archives supported by Cereal.
 *
 *  This class wraps Cereal's serialization API into different output archive
 * types. Required for serialization of SDEAny.
 */
class Serializer {
public:
    using binary_archive = cereal::BinaryOutputArchive;
    using json_archive   = cereal::JSONOutputArchive;
    using xml_archive    = cereal::XMLOutputArchive;
    template<typename T, typename Archive>
    using is_serializable = cereal::traits::is_output_serializable<T, Archive>;
    template<typename Archive>
    using get_input_from_output =
      cereal::traits::detail::get_input_from_output<Archive>;
    template<typename Archive>
    Serializer(Archive& ar) : m_ar_(&ar) {}

    template<typename T>
    Serializer& operator<<(T&& obj2serial) {
        if(m_ar_.index() == 0) {
            if constexpr(is_serializable<T, binary_archive>::value) {
                auto bar = std::get<binary_archive*>(m_ar_);
                (*bar)(std::forward<T>(obj2serial));
            }
        } else if(m_ar_.index() == 1) {
            if constexpr(is_serializable<T, json_archive>::value) {
                auto bar = std::get<json_archive*>(m_ar_);
                (*bar)(std::forward<T>(obj2serial));
            }
        } else if(m_ar_.index() == 2) {
            if constexpr(is_serializable<T, xml_archive>::value) {
                auto bar = std::get<xml_archive*>(m_ar_);
                (*bar)(std::forward<T>(obj2serial));
            }
        }
        return *this;
    }

private:
    std::variant<binary_archive*, json_archive*, xml_archive*> m_ar_;
};

/** @brief Deserializer class enables deserialization of objects from various
 * types of archives supported by Cereal.
 *
 *  This class wraps Cereal's deserialization API for different archive
 * types. Required for deserialization of SDEAny.
 */
class Deserializer {
public:
    using binary_archive = cereal::BinaryInputArchive;
    using json_archive   = cereal::JSONInputArchive;
    using xml_archive    = cereal::XMLInputArchive;
    template<typename T, typename Archive>
    using is_deserializable = cereal::traits::is_input_serializable<T, Archive>;
    template<typename Archive>
    using get_output_from_input =
      cereal::traits::detail::get_output_from_input<Archive>;

    template<typename Archive>
    Deserializer(Archive& ar) : m_ar_(&ar) {}

    template<typename T>
    Deserializer& operator>>(T&& serial2obj) {
        if(m_ar_.index() == 0) {
            if constexpr(is_deserializable<T, binary_archive>::value) {
                auto bar = std::get<binary_archive*>(m_ar_);
                (*bar)(std::forward<T>(serial2obj));
            }
        } else if(m_ar_.index() == 1) {
            if constexpr(is_deserializable<T, json_archive>::value) {
                auto bar = std::get<json_archive*>(m_ar_);
                (*bar)(std::forward<T>(serial2obj));
            }
        } else if(m_ar_.index() == 2) {
            if constexpr(is_deserializable<T, xml_archive>::value) {
                auto bar = std::get<xml_archive*>(m_ar_);
                (*bar)(std::forward<T>(serial2obj));
            }
        }
        return *this;
    }

private:
    std::variant<binary_archive*, json_archive*, xml_archive*> m_ar_;
};

} // namespace sde::detail_