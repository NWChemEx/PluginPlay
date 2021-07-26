#include "sde/detail_/sde_any_wrapper.hpp"

namespace sde::detail_ {

std::map<std::size_t, typename SDEAnyWrapperBase::fxn_type>
  SDEAnyWrapperBase::m_any_maker_;

}