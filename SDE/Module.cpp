#include "SDE/detail_/ModulePIMPL.hpp"

namespace SDE {

using output_type = typename Module::output_type;
using output_map  = typename Module::output_map;
using input_type  = typename Module::input_type;
using input_map   = typename Module::input_map;

Module::Module() : pimpl_(std::make_unique<detail_::ModulePIMPL>()) {}
Module::Module(const Module& rhs) :
    pimpl_(std::make_unique<detail_::ModulePIMPL>(*rhs.pimpl_))
Module& operator=(const Module& rhs) {
    return *this = std::move(Module(rhs));
}
Module::Module(Module&& rhs) = default;
Module& operator=(Module&& rhs) = default;
Module::~Module()               = default;

output_map Module::run(input_map ps) { return pimpl_->run(std::move(ps)); }

} // namespace SDE
