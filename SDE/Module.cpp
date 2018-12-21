#include "SDE/Module.hpp"

namespace SDE {
namespace detail_ {

class ModulePIMPL {
public:
    using submodule_map = std::map<std::string, Module>;
    using input_type = typename Module::input_type;
    using result_type = typename Module::result_type;

    submodule_map submodules() { return submodules_; }

    result_type run(const input_type& params) {

    }
private:
    std::unique_ptr<ModuleBase> impl_;
    submodule_map submodules_;
    //Cache cache_;
    input_type inputs_;

};


bool Module::can_be_run_as_(inputs, outputs) {}

}}
