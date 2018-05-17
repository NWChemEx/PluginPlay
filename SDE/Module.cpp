#include "SDE/Module.hpp"

using meta_data_key     = typename ModuleBase::meta_data_key;
using submodule_key     = typename ModuleBase::submodule_key;
using parameter_key     = typename ModuleBase::parameter_key;
using meta_data_key_set = typename ModuleBase::meta_data_key_set;
using submodule_key_set = typename ModuleBase::std::string > ;

/// Type of a set of parameter keys
using parameter_key_set = std::set<std::string>;

/// Type of meta-data
using meta_data_value = std::string;

/// Type of submodule value
using submodule_value = std::string;

namespace SDE {

submodule_key_set submodule_keys() const { return get_keys(submodules_); }
meta_data_key_set meta_data_keys() const { return get_keys(meta_data_); }
// parameter_key_set parameter_keys() const { return get_keys(parameters_);}

} // namespace SDE
