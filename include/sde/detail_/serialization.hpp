#pragma once
#include <cereal/archives/binary.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <sstream>

// TODO: Just include all of cereal's type headers here (is there a single
//       header that includes all of them?)

namespace sde::detail_ {

class Serializer {
public:
    using binary_archive = cereal::BinaryOutputArchive;

    Serializer(binary_archive& ar) : m_ar_(ar) {}

    template<typename T>
    Serializer& operator<<(T&& obj2serial) {
        m_ar_(std::forward<T>(obj2serial));
        return *this;
    }

private:
    // TODO: Use a variant
    binary_archive& m_ar_;
};

class Deserializer {
public:
    using binary_archive = cereal::BinaryInputArchive;

    Deserializer(binary_archive& ar) : m_ar_(ar) {}

    template<typename T>
    Deserializer& operator>>(T&& serial2obj) {
        m_ar_(std::forward<T>(serial2obj));
        return *this;
    }

private:
    // TODO: Use a variant
    binary_archive& m_ar_;
};

} // namespace sde::detail_