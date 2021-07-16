#pragma once
#include "sde/serialization.hpp"
#include <variant>

namespace sde::detail_ {
using variant_output =
  std::variant<sde::BinaryOutputArchive*, sde::PortableBinaryOutputArchive*,
               sde::JSONOutputArchive*, sde::XMLOutputArchive*>;
using variant_input =
  std::variant<sde::BinaryInputArchive*, sde::PortableBinaryInputArchive*,
               sde::JSONInputArchive*, sde::XMLInputArchive*>;

/** @brief ArchiveWrapper class enables serialization and deserialization of
 * objects using various types of archives supported by MADNESS and Cereal.
 *
 *  This class wraps MADNESS/Cereal API for different output archive types.
 * Required for serialization of SDEAny.
 */
template<typename VariantArchive>
class ArchiveWrapper {
public:
    template<typename Archive>
    ArchiveWrapper(Archive& ar) : m_ar_(&ar) {}

    template<typename T>
    ArchiveWrapper& operator()(T&& obj) {
        // MADNESS archive doesn't support `()`, but it supports `&` like Boost
        // archive.
        std::visit([&](auto&& ar) { (*ar) & std::forward<T>(obj); }, m_ar_);
        return *this;
    }

    template<typename T>
    ArchiveWrapper& operator<<(T&& obj) {
        std::visit([&](auto&& ar) { (*ar) << std::forward<T>(obj); }, m_ar_);
        return *this;
    }

    template<typename T>
    ArchiveWrapper& operator>>(T&& obj) {
        std::visit([&](auto&& ar) { (*ar) >> std::forward<T>(obj); }, m_ar_);
        return *this;
    }

private:
    VariantArchive m_ar_;
};

using Serializer   = ArchiveWrapper<variant_output>;
using Deserializer = ArchiveWrapper<variant_input>;
} // namespace sde::detail_