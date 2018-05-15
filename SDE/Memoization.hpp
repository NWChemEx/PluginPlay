#pragma once

#include <bphash/Hasher.hpp>
#include <bphash/types/All.hpp>
#include <pybind11/pybind11.h>

namespace SDE {

using Hasher    = bphash::Hasher;
using HashValue = bphash::HashValue;
using HashType  = bphash::HashType;

} // namespace SDE

inline void hash_object(const pybind11::object& obj, SDE::Hasher& h) {}

inline void hash_object(const pybind11::args& args, SDE::Hasher& h) {}
