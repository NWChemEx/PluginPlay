#include "pluginplay/detail_/any_wrapper.hpp"

namespace pluginplay::detail_ {

std::map<std::size_t, typename AnyWrapperBase::fxn_type>
  AnyWrapperBase::m_any_maker_;

}