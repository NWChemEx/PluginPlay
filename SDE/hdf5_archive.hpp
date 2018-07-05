#pragma once

#include "H5Cpp.h"
#include "SDE/hdf5_types.hpp"

#include "cereal/cereal.hpp"
#include "cereal/details/util.hpp"

namespace cereal{

  class HDF5OutputArchive : public OutputArchive<HDF5OutputArchive>{};

  class HDF5InputArchive : public InputArchive<HDF5InputArchive>{};

  template <class T>
  using scalar_t = std::enable_if_t<std::is_scalar<T>::value>;

  template <class T>
  using non_scalar_t = std::enable_if_t<std::negation_v<std::is_scalar<T>> >;

  template <class T> inline
  static scalar_t<T> prologue(
  HDF5OutputArchive& ar, NameValuePair<T> const & data ){};

  template <class T> inline
  static non_scalar_t<T> prologue(HDF5OutputArchive& ar, NameValuePair<T> const & data ){};

  template <class T> inline
  void prologue( HDF5InputArchive &, NameValuePair<T> const & nvp){};

  template <class T> inline
  void epilogue( HDF5OutputArchive &, NameValuePair<T> const & nvp){};

  template <class T> inline
  void epilogue( HDF5InputArchive &, NameValuePair<T> const & nvp){};

  template <class T> inline
  void prologue( HDF5OutputArchive & ar, SizeTag<T> const & sz){};

  template <class T> inline
  void prologue( HDF5InputArchive &, SizeTag<T> const & sz){};

  template <class T> inline
  void epilogue( HDF5OutputArchive &, SizeTag<T> const & sz){};

  template <class T> inline
  void epilogue( HDF5InputArchive &, SizeTag<T> const & sz){};

  template <class T>
  static scalar_t<T> prologue(HDF5OutputArchive& ar, T const & data);

  template <class T>
  static non_scalar_t<T> prologue(HDF5OutputArchive& ar, T const & data);

  template <class T, traits::DisableIf<traits::has_minimal_base_class_serialization<T, traits::has_minimal_input_serialization, HDF5InputArchive>::value ||
                                       traits::has_minimal_input_serialization<T, HDF5InputArchive>::value> = traits::sfinae> inline
  void prologue( HDF5InputArchive & ar, T const & data){};

  template <class T, traits::DisableIf<traits::has_minimal_base_class_serialization<T, traits::has_minimal_output_serialization, HDF5OutputArchive>::value ||
                                       traits::has_minimal_output_serialization<T, HDF5OutputArchive>::value> = traits::sfinae> inline
  void epilogue( HDF5OutputArchive & ar, T const & data){};

  template <class T, traits::DisableIf<traits::has_minimal_base_class_serialization<T, traits::has_minimal_input_serialization, HDF5InputArchive>::value ||
                                       traits::has_minimal_input_serialization<T, HDF5InputArchive>::value> = traits::sfinae> inline
  void epilogue( HDF5InputArchive & ar, T const & data){};

  template <class T> inline
  void CEREAL_SAVE_FUNCTION_NAME( HDF5OutputArchive & ar, NameValuePair<T> const & nvp){};
  
  template <class T> inline
  void CEREAL_LOAD_FUNCTION_NAME( HDF5InputArchive & ar, NameValuePair<T> & nvp){};

  template <class T> inline
  void CEREAL_SAVE_FUNCTION_NAME( HDF5OutputArchive &, SizeTag<T> const & sz){};

  template <class T> inline
  void CEREAL_LOAD_FUNCTION_NAME( HDF5InputArchive & ar, SizeTag<T> & sz){};

  template <class T, traits::EnableIf<std::is_arithmetic<T>::value> = traits::sfinae> inline
  void CEREAL_SAVE_FUNCTION_NAME(HDF5OutputArchive & ar, T & data){};

  template <class T, traits::EnableIf<std::is_arithmetic<T>::value> = traits::sfinae> inline
  void CEREAL_LOAD_FUNCTION_NAME(HDF5InputArchive & ar, T & data){};

  template<class CharT, class Traits, class Alloc> inline
  void CEREAL_SAVE_FUNCTION_NAME(HDF5OutputArchive & ar, std::basic_string<CharT, Traits, Alloc> const & str){};

  template<class CharT, class Traits, class Alloc> inline
  void CEREAL_LOAD_FUNCTION_NAME(HDF5InputArchive & ar, std::basic_string<CharT, Traits, Alloc> & str){};
  
} // End namespace cereal

CEREAL_REGISTER_ARCHIVE(cereal::HDF5InputArchive);
CEREAL_REGISTER_ARCHIVE(cereal::HDF5OutputArchive);

CEREAL_SETUP_ARCHIVE_TRAITS(cereal::HDF5InputArchive, cereal::HDF5OutputArchive);
