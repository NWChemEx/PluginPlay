#pragma once

#include <Utilities/SmartEnum.hpp>
#include <bphash/Hasher.hpp>
#include <bphash/types/All.hpp>
#include <functional>

namespace SDE {

using Hasher    = bphash::Hasher;
using HashValue = bphash::HashValue;
using HashType  = bphash::HashType;

} // namespace SDE

namespace Utilities {
template<typename T>
void hash_object(const SmartEnum<T>& ei, bphash::Hasher& h) {
    h(std::string(static_cast<const char*>(ei)));
}
}

namespace std {
template<typename T>
void hash_object(const reference_wrapper<T>& ref, bphash::Hasher& h) {
    h(ref.get());
}

}
