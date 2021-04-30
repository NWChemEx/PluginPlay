#pragma once
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/archives/xml.hpp>
#include <cereal/details/traits.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <sstream>
#include <variant>

// TODO: Just include all of cereal's type headers here (is there a single
//       header that includes all of them?)

namespace sde::detail_ {

/** @brief Serializer class enables serialization into various types of
 * archives.
 *
 *  This class wraps Cereal's input archive types API.
 */
class Serializer {
public:
    using binary_archive = cereal::BinaryOutputArchive;
    using json_archive   = cereal::JSONOutputArchive;
    using xml_archive    = cereal::XMLOutputArchive;
    using variant_archive =
      std::variant<binary_archive*, json_archive*, xml_archive*>;
    Serializer(variant_archive& ar) : m_ar_(ar) {}

    template<typename T>
    Serializer& operator<<(T&& obj2serial) {
        if(m_ar_.index() == 0) {
            auto bar = std::get<binary_archive*>(m_ar_);
            if constexpr(cereal::traits::is_output_serializable<
                           T, binary_archive>::value)
                (*bar)(std::forward<T>(obj2serial));
        } else if(m_ar_.index() == 1) {
            auto bar = std::get<json_archive*>(m_ar_);
            if constexpr(cereal::traits::is_output_serializable<
                           T, json_archive>::value)
                (*bar)(std::forward<T>(obj2serial));
        } else if(m_ar_.index() == 1) {
            auto bar = std::get<xml_archive*>(m_ar_);
            if constexpr(cereal::traits::is_output_serializable<
                           T, xml_archive>::value)
                (*bar)(std::forward<T>(obj2serial));
        }
        return *this;
    }

private:
    variant_archive& m_ar_;
};

class Deserializer {
public:
    using binary_archive = cereal::BinaryInputArchive;
    using json_archive   = cereal::JSONInputArchive;
    using xml_archive    = cereal::XMLInputArchive;
    using variant_archive =
      std::variant<binary_archive*, json_archive*, xml_archive*>;

    Deserializer(variant_archive& ar) : m_ar_(ar) {}

    template<typename T>
    Deserializer& operator>>(T&& serial2obj) {
        if(m_ar_.index() == 0 &&
           cereal::traits::is_input_serializable<T, binary_archive>::value) {
            auto bar = std::get<binary_archive*>(m_ar_);
            if constexpr(cereal::traits::is_input_serializable<
                           T, binary_archive>::value)
                (*bar)(std::forward<T>(serial2obj));
        } else if(m_ar_.index() == 1 &&
                  cereal::traits::is_input_serializable<T,
                                                        json_archive>::value) {
            auto bar = std::get<json_archive*>(m_ar_);
            if constexpr(cereal::traits::is_input_serializable<
                           T, json_archive>::value)
                (*bar)(std::forward<T>(serial2obj));
        } else if(m_ar_.index() == 2) {
            auto bar = std::get<xml_archive*>(m_ar_);
            if constexpr(cereal::traits::is_input_serializable<
                           T, xml_archive>::value)
                (*bar)(std::forward<T>(serial2obj));
        }
        return *this;
    }

private:
    variant_archive& m_ar_;
};

} // namespace sde::detail_