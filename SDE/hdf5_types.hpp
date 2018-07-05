#pragma once

#include "H5Cpp.h"

namespace cereal {

template <class T>
struct CtoHDF5;

#define REGISTER_CXX_TO_HDF(c_type, hdf_type)\
template<>\
struct CtoHDF5<c_type>{\
static inline H5::DataType h5Type = hdf_type;}

REGISTER_CXX_TO_HDF(char, H5::PredType::NATIVE_CHAR);
REGISTER_CXX_TO_HDF(signed char, H5::PredType::NATIVE_SCHAR);
REGISTER_CXX_TO_HDF(unsigned char, H5::PredType::NATIVE_UCHAR);
REGISTER_CXX_TO_HDF(short, H5::PredType::NATIVE_SHORT);
REGISTER_CXX_TO_HDF(unsigned short, H5::PredType::NATIVE_USHORT);

REGISTER_CXX_TO_HDF(int, H5::PredType::NATIVE_INT);
REGISTER_CXX_TO_HDF(unsigned int, H5::PredType::NATIVE_UINT);

REGISTER_CXX_TO_HDF(long, H5::PredType::NATIVE_LONG);
REGISTER_CXX_TO_HDF(unsigned long, H5::PredType::NATIVE_ULONG);

REGISTER_CXX_TO_HDF(long long, H5::PredType::NATIVE_LLONG);
REGISTER_CXX_TO_HDF(unsigned long long, H5::PredType::NATIVE_ULLONG);

REGISTER_CXX_TO_HDF(float, H5::PredType::NATIVE_FLOAT);
REGISTER_CXX_TO_HDF(double, H5::PredType::NATIVE_DOUBLE);
REGISTER_CXX_TO_HDF(long double, H5::PredType::NATIVE_LDOUBLE);

REGISTER_CXX_TO_HDF(bool, H5::PredType::NATIVE_HBOOL);

REGISTER_CXX_TO_HDF(std::string, H5::PredType::C_S1);
}


