#pragma once
#include "SDE/ModuleInput.hpp"
#include "SDE/ModuleOutput.hpp"
#include <Utilities/Containers/CaseInsensitiveMap.hpp>

namespace SDE {
class SubmoduleRequest;
namespace detail_ {
class ModulePIMPL;
}

class Module {
public:
    using input_type  = ModuleInput;
    using input_map   = Utilities::CaseInsensitiveMap<input_type>;
    using output_type = ModuleOutput;
    using output_map  = Utilities::CaseInsensitiveMap<output_type>;
    using submod_type = SubmoduleRequest;
    using submod_map  = Utilities::CaseInsensitiveMap<submod_type>;
    using key_type    = std::string;

    Module();
    Module(const Module& rhs);
    Module& operator=(const Module& rhs);
    Module(Module&& rhs) noexcept;
    Module& operator=(Module&& rhs) noexcept;
    Module(std::unique_ptr<detail_::ModulePIMPL> base) noexcept;
    ~Module() noexcept;

    template<typename property_type, typename... Args>
    auto run_as(Args&&... args) const {
        auto temp = inputs();
        temp = property_type::wrap_inputs(temp, std::forward<Args>(args)...);
        return property_type::unwrap_outputs(run(temp));
    }

    const input_map& inputs() const;
    input_map& inputs() {
        const auto& input = const_cast<const Module&>(*this).inputs();
        return const_cast<input_map&>(input);
    }
    const output_map& outputs() const;
    const submod_map& submods() const;
    submod_map& submods() {
        const auto& submod = const_cast<const Module&>(*this).submods();
        return const_cast<submod_map&>(submod);
    }

    output_map run(input_map ps) const;
    bool is_ready() const;
    void hash(bphash::Hasher& h) const;

private:
    std::unique_ptr<detail_::ModulePIMPL> pimpl_;
};

} // namespace SDE
