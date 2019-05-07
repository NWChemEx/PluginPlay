#pragma once

#include <bphash/Hasher.hpp>
#include <bphash/types/All.hpp>
#include <functional>
#include <utilities/smart_enum.hpp>

namespace sde {

using Hasher    = bphash::Hasher;
using HashValue = bphash::HashValue;
using HashType  = bphash::HashType;

} // namespace sde

namespace utilities {
template<typename T>
void hash_object(const SmartEnum<T>& ei, bphash::Hasher& h) {
    h(std::string(static_cast<const char*>(ei)));
}
} // namespace utilities

namespace std {
template<typename T>
void hash_object(const reference_wrapper<T>& ref, bphash::Hasher& h) {
    h(ref.get());
}

} // namespace std
