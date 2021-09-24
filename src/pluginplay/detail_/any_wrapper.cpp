#include "pluginplay/detail_/any_wrapper.hpp"

namespace pluginplay::detail_ {
template<>
std::map<std::size_t, typename AnyWrapperBase<>::fxn_type>
  AnyWrapperBase<>::m_any_maker_;

}