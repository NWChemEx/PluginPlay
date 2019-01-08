#include "SDE/Module.hpp"
#include "SDE/ModuleBase.hpp"

namespace SDE {
using input_type  = typename Module::input_type;
using input_map   = typename Module::input_map;
using output_type = typename Module::output_type;
using output_map  = typename Module::output_map;
using submod_type = typename Module::submod_type;
using submod_map  = typename Module::submod_map;

namespace detail_ {

/**
 *
 *  The wrapped ModuleBase created what the developer declared as the default
 *  module state. The PIMPL class takes ownership of the input and uses it as
 *  its default. Subsequent manipulations by the end user change the state held
 *  in the PIMPL. Copies of a module copy the state in the PIMPL, while aliasing
 *  the ModuleBase. This avoids the need to copy the ModuleBase while being able
 *  to copy its state. As a note, the output_map from a module is read only
 *  so it's fine for all PIMPLs to use the shared copy. Using the same run is
 *  an interesting consideration. If the derived module has no additional class
 *  members and does not access any global state then the run is thread-safe and
 *  it doesn't matter that we're using a shared copy; however, if the module
 *  has additional state or accesses global state then data races can occur. It
 *  is strongly advised that modules do not do this....
 */
class ModulePIMPL {
public:
    using input_type  = typename Module::input_type;
    using input_map   = typename Module::input_map;
    using output_type = typename Module::output_type;
    using output_map  = typename Module::output_map;
    using submod_type = typename Module::submod_type;
    using submod_map  = typename Module::submod_map;
    using base_type   = ModuleBase;
    using base_ptr    = std::shared_ptr<ModuleBase>;
    using cache_type  = std::map<std::string, output_map>;
    using cache_ptr   = std::shared_ptr<Cache>;

    ModulePIMPL()                       = default;
    ModulePIMPL(const ModulePIMPL& rhs) = default;
    ModulePIMPL& operator=(const ModulePIMPL& rhs) = default;
    ModulePIMPL(ModulePIMPL&& rhs)                 = default;
    ModulePIMPL& operator=(ModulePIMPL&& rhs) = default;
    ModulePIMPL(base_ptr base, cache_ptr cache = cache_ptr{}) :
      base_(base),
      cache_(cache),
      inputs_(std::move(base->inputs())),
      submods_(std::move(base->submods())) {}
    ~ModulePIMPL() = default;

    output_map run(input_map ps) const {
        auto hv = base_->memoize(ps, submods_);
        if(cache_ && cache_->count(hv)) return cache_->at(hv);
        auto rv = base_->run(std::move(ps), submods_);
        if(!cache_) return rv;
        // cache_ is internal detail so okay to break const
        cache_->emplace(hv, std::move(rv));

        return cache_->at(rv);
    }

private:
    base_ptr base_;
    cache_ptr cache_;
    input_map inputs_;
    submod_map submods_;
};

} // namespace detail_

Module::Module() = default;
Module::Module(const Module& rhs) :
  pimpl_(std::make_unique<detail_::ModulePIMPL>(*rhs.pimpl_)) {}
Module& operator=(const Module& rhs) { return *this = std::move(Module(rhs)); }
Module::Module(Module&& rhs) = default;
Module& operator=(Module&& rhs) = default;
Module::~Module()               = default;

output_map Module::run(input_map ps) { return pimpl_->run(std::move(ps)); }
const input_map& Module::inputs() const { return pimpl_->inputs(); }
const output_map& Module::outputs() const { return pimpl_->outputs(); }
const submod_map& Module::submods() const { return pimpl_->submods(); }

} // namespace SDE
