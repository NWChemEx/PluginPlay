#pragma once
#include "SDE/ModuleInput.hpp"
#include "SDE/ModuleOutput.hpp"
#include "SDE/SubmoduleRequest.hpp"
#include <Utilitites/Containers/CaseInsensitiveMap.hpp>

namespace SDE {
namespace detail_ {
/** @brief Class accumulating the various objects comprising a module's state
 *
 *  A module's state appears in several places including:
 *  - PropertyType - Defines property type API
 *  - ModuleBase - Defines the module's full API
 *  - ModulePIMPL - Defines the module state the user wants
 */
class ModuleState {
private:
    template<typename T>
    using CIM = Utilities::CaseInsensitiveMap<T>;

public:
    using input_value      = ModuleInput;
    using output_value     = ModuleOutput;
    using submod_value     = SubmoduleRequest;
    using input_map        = CIM<input_value>;
    using output_map       = CIM<output_value>;
    using submod_map       = CIM<submod_value>;
    using key_type         = typename input_map::key_type;
    using description_type = typename ModuleInput::description_type;

    input_map inputs;
    output_map outputs;
    submod_map submods;

    description_type desc;
};

} // namespace detail_
} // namespace SDE
