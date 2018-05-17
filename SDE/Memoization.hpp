#pragma once

#include <Utilities/SmartEnum.hpp>
#include <bphash/Hasher.hpp>
#include <bphash/types/All.hpp>

namespace SDE {

using Hasher    = bphash::Hasher;
using HashValue = bphash::HashValue;
using HashType  = bphash::HashType;

template<typename T>
void hash_object(const Utilities::SmartEnum<T>& ei, bphash::Hasher& h) {
    h(std::string(static_cast<const char*>(ei)));
}

} // namespace SDE
